/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * xoreos is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * xoreos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xoreos. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file
 *  Decoding Microsoft Xbox XMV videos.
 */

#include <cassert>

#include "src/common/error.h"
#include "src/common/memreadstream.h"
#include "src/common/strutil.h"

#include "src/sound/audiostream.h"
#include "src/sound/interleaver.h"

#include "src/sound/decoders/wave_types.h"
#include "src/sound/decoders/pcm.h"
#include "src/sound/decoders/adpcm.h"

#include "src/video/xmv.h"

#include "src/video/codecs/xmvwmv2.h"

static const int kAudioFlagADPCM51FrontLeftRight = 1;
static const int kAudioFlagADPCM51FrontCenterLow = 2;
static const int kAudioFlagADPCM51RearLeftRight  = 4;
static const int kAudioFlagADPCM51               = kAudioFlagADPCM51FrontLeftRight |
                                                   kAudioFlagADPCM51FrontCenterLow |
                                                   kAudioFlagADPCM51RearLeftRight;

namespace Video {

XboxMediaVideo::ADPCM51Streams::ADPCM51Streams() : enabled(false) {
	streams.resize(3, 0);
}

XboxMediaVideo::ADPCM51Streams::~ADPCM51Streams() {
	delete streams[0];
	delete streams[1];
	delete streams[2];
}


XboxMediaVideo::XboxMediaVideo(Common::SeekableReadStream *xmv) :
	_xmv(xmv) {

	assert(_xmv);

	load();
}

XboxMediaVideo::~XboxMediaVideo() {
}

uint32 XboxMediaVideo::getNextFrameStartTime() const {
	if (!_started)
		return 0;

	return _curPacket.video.currentFrameTimestamp;
}

void XboxMediaVideo::startVideo() {
	queueNewAudio(_curPacket);

	_started   = true;
}

void XboxMediaVideo::queueNewAudio(PacketAudio &audioPacket) {
	// New data available that we should play?
	if (!audioPacket.newSlice || !audioPacket.track->enabled)
		return;

	// Seek to it
	_xmv->seek(audioPacket.dataOffset);

	// Read and queue it
	queueAudioStream(_xmv->readStream(audioPacket.dataSize), *audioPacket.track);

	audioPacket.newSlice = false;
}

void XboxMediaVideo::queueNewAudio(Packet &packet) {
	// Go over all audio packets and try to queue their data.

	for (std::vector<PacketAudio>::iterator audio = packet.audio.begin();
	     audio != packet.audio.end(); ++audio)
		queueNewAudio(*audio);
}

void XboxMediaVideo::processNextFrame(PacketVideo &videoPacket) {
	// No frame left, nothing to do
	if (videoPacket.frameCount == 0)
		return;

	// Seek
	_xmv->seek(videoPacket.dataOffset);

	// Read the frame header

	uint32 frameHeader = _xmv->readUint32LE();

	videoPacket.currentFrameSize      = (frameHeader & 0x1FFFF) * 4 + 4;
	videoPacket.currentFrameTimestamp = (frameHeader >> 17) + videoPacket.lastFrameTime;

	if (videoPacket.currentFrameSize > videoPacket.dataSize)
		throw Common::Exception("XboxMediaVideo::processNextFrame(): Frame data overrun");

	// Decode the frame

	if (videoPacket.currentFrameSize > 0) {
		if (_videoCodec) {
			assert(_surface);

			Common::SeekableSubReadStream frameData(_xmv.get(), _xmv->pos(),
			                                        _xmv->pos() + videoPacket.currentFrameSize);

			_videoCodec->decodeFrame(*_surface, frameData);
			_needCopy = true;
		} else
			warning("XboxMediaVideo::processNextFrame(): Video frame without a decoder");
	}

	// Update the frame time
	videoPacket.lastFrameTime = videoPacket.currentFrameTimestamp;

	// Advance the data
	videoPacket.dataSize   -= videoPacket.currentFrameSize + 4;
	videoPacket.dataOffset += videoPacket.currentFrameSize + 4;

	// One less frame to worry about
	videoPacket.frameCount--;
}

void XboxMediaVideo::load() {
	// Read the XMV header

	_xmv->skip(4); // Next packet size

	uint32 thisPacketSize = _xmv->readUint32LE();

	_xmv->skip(4); // Max packet size

	uint32 tag = _xmv->readUint32LE();
	if (tag != MKTAG('X', 'b', 'o', 'x'))
		throw Common::Exception("XboxMediaVideo::load(): No 'Xbox' tag (%s)", Common::debugTag(tag).c_str());

	uint32 version = _xmv->readUint32LE();

	if ((version == 0) || (version > 4))
		throw Common::Exception("XboxMediaVideo::load(): Unsupported version %d", version);

	uint32 width    = _xmv->readUint32LE();
	uint32 height   = _xmv->readUint32LE();

	_xmv->skip(4); // Duration in ms

	uint32 audioTrackCount = _xmv->readUint16LE();
	_audioTracks.resize(audioTrackCount);

	_xmv->skip(2); // Unknown

	// Audio tracks
	for (uint32 i = 0; i < audioTrackCount; i++) {
		_audioTracks[i].compression   = _xmv->readUint16LE();
		_audioTracks[i].channels      = _xmv->readUint16LE();
		_audioTracks[i].rate          = _xmv->readUint32LE();
		_audioTracks[i].bitsPerSample = _xmv->readUint16LE();
		_audioTracks[i].flags         = _xmv->readUint16LE();

		evaluateAudioTrack(_audioTracks[i]);
	}


	// Initialize the video
	initVideo(width, height);

	// Initialize the sound: Find the first supported audio track for now
	for (uint32 i = 0; i < audioTrackCount; i++) {
		if (_audioTracks[i].supported) {
			uint32 channels = _audioTracks[i].channels;

			_audioTracks[i].enabled = true;

			if ((_audioTracks[i].flags & kAudioFlagADPCM51) && ((i + 2) < audioTrackCount)) {
				_adpcm51Streams.enabled = true;

				_audioTracks[i + 1].enabled = true;
				_audioTracks[i + 2].enabled = true;

				channels = 6;
			}

			initSound(_audioTracks[i].rate, channels, true);
			break;
		}
	}


	// Initialize the packet data

	_curPacket.nextPacketOffset = _xmv->pos();

	_curPacket.nextPacketSize = thisPacketSize - _curPacket.nextPacketOffset;
	_curPacket.thisPacketSize = 0;

	_curPacket.video.frameCount    = 0;
	_curPacket.video.lastFrameTime = 0;

	_curPacket.video.currentFrameTimestamp = 0;

	_curPacket.audio.resize(audioTrackCount);
	for (uint32 i = 0; i < audioTrackCount; i++)
		_curPacket.audio[i].track = &_audioTracks[i];


	// Fetch the first packet
	fetchNextPacket(_curPacket);
}

void XboxMediaVideo::evaluateAudioTrack(AudioTrack &track) {
	// Assume it's supported first
	track.supported = true;
	track.enabled   = false;

	track.audioStreamFlags = Sound::FLAG_LITTLE_ENDIAN;

	// Check channel count
	if ((track.channels == 0) || (track.channels > 2)) {
		warning("XboxMediaVideo::evaluateAudioTrack(): Unsupported channel count %d",
		        track.channels);

		track.supported = false;
	}

	// Check compression method
	switch (track.compression) {
		case Sound::kWavePCM:
			if        (track.bitsPerSample == 16) {
				track.audioStreamFlags |= Sound::FLAG_16BITS;
			} else if (track.bitsPerSample !=  8) {
				warning("XboxMediaVideo::evaluateAudioTrack(): Invalid bits per sample value for "
				        "raw PCM audio: %d", track.bitsPerSample);
				track.supported = false;
			}

			break;

		case Sound::kWaveMSIMAADPCM2:
			track.audioStreamFlags |= Sound::FLAG_16BITS;

			if (track.bitsPerSample != 4) {
				warning("XboxMediaVideo::evaluateAudioTrack(): Invalid bits per sample value for "
				        "MS IMA ADPCM audio: %d", track.bitsPerSample);
				track.supported = false;
			}
			break;

		default:
			warning("XboxMediaVideo::evaluateAudioTrack(): Unknown audio compression 0x%04x",
			        track.compression);
			track.supported = false;
			break;
	}

}

void XboxMediaVideo::fetchNextPacket(Packet &packet) {
	// Seek to it
	packet.thisPacketOffset = packet.nextPacketOffset;
	_xmv->seek(packet.thisPacketOffset);

	// Update the size
	packet.thisPacketSize = packet.nextPacketSize;
	if (packet.thisPacketSize < (12 + _audioTracks.size() * 4))
		return;

	// Process the header
	processPacketHeader(packet);

	// Update the offset
	packet.nextPacketOffset = packet.thisPacketOffset + packet.thisPacketSize;
}

void XboxMediaVideo::processPacketHeader(Packet &packet) {
	// Next packet size
	packet.nextPacketSize = _xmv->readUint32LE();

	// Packet video header

	byte videoHeaderData[8];
	_xmv->read(videoHeaderData, 8);

	packet.video.dataSize   =  READ_LE_UINT32(videoHeaderData) & 0x007FFFFF;
	packet.video.frameCount = (READ_LE_UINT32(videoHeaderData) >> 23) & 0xFF;

	bool hasExtraData = (videoHeaderData[3] & 0x80) != 0;

	// Adding the audio data sizes and the video data size keeps you 4 bytes short
	// for every audio track. But as playing around with XMV files with ADPCM audio
	// showed, taking the extra 4 bytes from the audio data gives you either
	// completely distorted audio or click (when skipping the remaining 68 bytes of
	// the ADPCM block). Subtracting _audioTracks.size() * 4 bytes from the video
	// data works at least for the audio. Probably some alignment thing?
	// The video data has (always?) lots of padding, so it should work out regardless.
	packet.video.dataSize -= _audioTracks.size() * 4;

	// Packet audio header

	packet.audio.resize(_audioTracks.size());
	for (size_t i = 0; i < packet.audio.size(); i++) {
		PacketAudio &audioHeader = packet.audio[i];

		byte audioHeaderData[4];
		_xmv->read(audioHeaderData, 4);

		audioHeader.dataSize = READ_LE_UINT32(audioHeaderData) & 0x007FFFFF;
		if ((audioHeader.dataSize == 0) && (i != 0))
			// This happens when I create an XMV with several identical audio
			// streams. From the size calculations, duplicating the previous
			// stream's size works out, but the track data itself is silent.
			// Maybe this should also redirect the offset to the previous track?
			audioHeader.dataSize = packet.audio[i - 1].dataSize;

		audioHeader.newSlice = audioHeader.dataSize > 0;
	}

	// Packet data offsets

	size_t dataOffset = _xmv->pos();

	packet.video.dataOffset = dataOffset;
	dataOffset += packet.video.dataSize;

	for (size_t i = 0; i < packet.audio.size(); i++) {
		packet.audio[i].dataOffset = dataOffset;
		dataOffset += packet.audio[i].dataSize;
	}

	// If we have extra data, (re)create the video codec

	if (hasExtraData && (packet.video.dataSize < 4))
		warning("XboxMediaVideo::processPacketHeader(): Video extra data doesn't fit");

	if (packet.video.dataSize >= 4) {
		if (hasExtraData) {
			Common::SeekableSubReadStream extraData(_xmv.get(), _xmv->pos(), _xmv->pos() + 4);

			_videoCodec.reset(new XMVWMV2Codec(_width, _height, extraData));

			packet.video.dataSize   -= 4;
			packet.video.dataOffset += 4;
		}
	}

}

void XboxMediaVideo::processData() {
	// No frames left => we finished playing
	if (_curPacket.video.frameCount == 0) {
		finish();
		return;
	}

	// Process the next frame
	processNextFrame(_curPacket.video);

	// Got all frames in the current packet?
	if (_curPacket.video.frameCount == 0) {
		// Fetch the next one and queue the audio

		fetchNextPacket(_curPacket);
		queueNewAudio(_curPacket);
	}
}

void XboxMediaVideo::queueAudioStream(Common::SeekableReadStream *stream,
                                      const AudioTrack &track) {

	Common::ScopedPtr<Common::SeekableReadStream> dataStream(stream);

	// No stream or not a supported track
	if (!stream || !track.supported)
		return;

	Common::ScopedPtr<Sound::AudioStream> audioStream;

	// Create the audio stream
	switch (track.compression) {
		case Sound::kWavePCM:
			audioStream.reset(Sound::makePCMStream(dataStream.release(),
			                  track.rate, track.audioStreamFlags, track.channels, true));
			break;

		case Sound::kWaveMSIMAADPCM2:
			audioStream.reset(Sound::makeADPCMStream(dataStream.release(),
			                  true, stream->size(), Sound::kADPCMMSIma, track.rate,
			                  track.channels, 36 * track.channels));
			break;

		default:
			break;
	}

	if (!audioStream)
		return;

	// If we don't have to do ADPCM 5.1 interleaving, just queue the sound
	if (!_adpcm51Streams.enabled) {
		queueSound(audioStream.release());
		return;
	}

	Sound::AudioStream **targetStream = 0;

	// Find the right target for this stream
	     if (track.flags & kAudioFlagADPCM51FrontLeftRight)
		targetStream = &_adpcm51Streams.streams[0];
	else if (track.flags & kAudioFlagADPCM51FrontCenterLow)
		targetStream = &_adpcm51Streams.streams[1];
	else if (track.flags & kAudioFlagADPCM51RearLeftRight)
		targetStream = &_adpcm51Streams.streams[2];

	if (!targetStream) {
		warning("XboxMediaVideo::queueAudioStream(): Broken ADPCM 5.1 flags: 0x%04X", track.flags);
		return;
	}

	// Assign it
	delete *targetStream;
	*targetStream = audioStream.release();

	// At least one stream still not filled? We're done for now
	if (!_adpcm51Streams.streams[0] && !_adpcm51Streams.streams[1] && !_adpcm51Streams.streams[2])
		return;

	// All 3 streams filled? Create an interleaver and queue that

	audioStream.reset(Sound::makeInterleaver(track.rate, _adpcm51Streams.streams, true));

	_adpcm51Streams.streams[0] = 0;
	_adpcm51Streams.streams[1] = 0;
	_adpcm51Streams.streams[2] = 0;

	queueSound(audioStream.release());
}

} // End of namespace Video
