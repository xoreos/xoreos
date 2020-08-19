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
#include "src/common/ptrvector.h"
#include "src/common/strutil.h"
#include "src/common/timestamp.h"

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

XboxMediaVideo::XboxMediaVideo(Common::SeekableReadStream *xmv) :
	_xmv(xmv), _videoTrack(0), _audioTrackCount(0) {

	assert(_xmv);

	load();
}

XboxMediaVideo::~XboxMediaVideo() {
}

void XboxMediaVideo::queueNewAudio(PacketAudio &audioPacket) {
	// New data available that we should play?
	if (!audioPacket.newSlice || !audioPacket.track)
		return;

	// Seek to it
	_xmv->seek(audioPacket.dataOffset);

	// Read and queue it
	audioPacket.track->queueAudio(_xmv->readStream(audioPacket.dataSize));

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

	uint32_t frameHeader = _xmv->readUint32LE();

	videoPacket.currentFrameSize      = (frameHeader & 0x1FFFF) * 4 + 4;
	videoPacket.currentFrameTimestamp = (frameHeader >> 17) + videoPacket.lastFrameTime;

	if (videoPacket.currentFrameSize > videoPacket.dataSize)
		throw Common::Exception("XboxMediaVideo::processNextFrame(): Frame data overrun");

	// Decode the frame

	if (videoPacket.currentFrameSize > 0) {
		Common::SeekableSubReadStream frameData(_xmv.get(), _xmv->pos(), _xmv->pos() + videoPacket.currentFrameSize);
		_needCopy = _videoTrack->decodeFrame(*_surface, frameData);

		if (!_needCopy)
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

	uint32_t thisPacketSize = _xmv->readUint32LE();

	_xmv->skip(4); // Max packet size

	uint32_t tag = _xmv->readUint32LE();
	if (tag != MKTAG('X', 'b', 'o', 'x'))
		throw Common::Exception("XboxMediaVideo::load(): No 'Xbox' tag (%s)", Common::debugTag(tag).c_str());

	uint32_t version = _xmv->readUint32LE();

	if ((version == 0) || (version > 4))
		throw Common::Exception("XboxMediaVideo::load(): Unsupported version %d", version);

	uint32_t width    = _xmv->readUint32LE();
	uint32_t height   = _xmv->readUint32LE();

	_xmv->skip(4); // Duration in ms

	_audioTrackCount = _xmv->readUint16LE();
	std::vector<AudioInfo> audioTrackInfo;
	audioTrackInfo.resize(_audioTrackCount);

	_xmv->skip(2); // Unknown

	// Audio track info
	for (uint32_t i = 0; i < _audioTrackCount; i++) {
		audioTrackInfo[i].compression   = _xmv->readUint16LE();
		audioTrackInfo[i].channels      = _xmv->readUint16LE();
		audioTrackInfo[i].rate          = _xmv->readUint32LE();
		audioTrackInfo[i].bitsPerSample = _xmv->readUint16LE();
		audioTrackInfo[i].flags         = _xmv->readUint16LE();
	}

	// Initialize the audio tracks
	Common::PtrVector<XMVAudioTrack> audioTracks;
	audioTracks.resize(_audioTrackCount);
	for (uint16_t i = 0; i < _audioTrackCount; i++) {
		XMVAudioTrack *track;

		// Try creating the track; if we can't, move on.
		try {
			track = new XMVAudioTrack(audioTrackInfo[i]);
		} catch (Common::Exception &e) {
			warning("Failed to initialize audio track: %s", e.what());
			continue;
		}

		audioTracks[i] = track;
	}

	// Initialize the packet data

	_curPacket.nextPacketOffset = _xmv->pos();

	_curPacket.nextPacketSize = thisPacketSize - _curPacket.nextPacketOffset;
	_curPacket.thisPacketSize = 0;

	_curPacket.video.frameCount    = 0;
	_curPacket.video.lastFrameTime = 0;

	_curPacket.video.currentFrameTimestamp = 0;

	_curPacket.audio.resize(_audioTrackCount);
	for (uint32_t i = 0; i < _audioTrackCount; i++)
		_curPacket.audio[i].track = 0;


	// We can only use the first audio track. Try to create it.
	for (uint32_t i = 0; i < _audioTrackCount; i++) {
		if (!audioTracks[i])
			continue;

		if ((audioTrackInfo[i].flags & kAudioFlagADPCM51) != 0 && (i + 2) < _audioTrackCount) {
			// Make sure the other tracks are valid
			if (audioTracks[i + 1] && audioTracks[i + 2]) {
				_curPacket.audio[i].track = audioTracks[i];
				_curPacket.audio[i + 1].track = audioTracks[i];
				_curPacket.audio[i + 2].track = audioTracks[i];

				addTrack(new XMVAudioTrack51(audioTracks[i], audioTracks[i + 1], audioTracks[i + 2]));

				audioTracks[i] = 0;
				audioTracks[i + 1] = 0;
				audioTracks[i + 2] = 0;
			} else {
				warning("Could not create 5.1 track");
			}
		} else {
			_curPacket.audio[i].track = audioTracks[i];
			addTrack(audioTracks[i]);
			audioTracks[i] = 0;
		}

		break;
	}

	// Add the video track
	_videoTrack = new XMVVideoTrack(width, height, _curPacket.video.currentFrameTimestamp);
	addTrack(_videoTrack);

	// Fetch the first packet
	fetchNextPacket(_curPacket);

	// Feed audio
	queueNewAudio(_curPacket);

	// Initialize video
	initVideo();
}

void XboxMediaVideo::fetchNextPacket(Packet &packet) {
	// Seek to it
	packet.thisPacketOffset = packet.nextPacketOffset;
	_xmv->seek(packet.thisPacketOffset);

	// Update the size
	packet.thisPacketSize = packet.nextPacketSize;
	if (packet.thisPacketSize < (12 + _audioTrackCount * 4))
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
	// the ADPCM block). Subtracting _audioTrackCount * 4 bytes from the video
	// data works at least for the audio. Probably some alignment thing?
	// The video data has (always?) lots of padding, so it should work out regardless.
	packet.video.dataSize -= _audioTrackCount * 4;

	// Packet audio header

	packet.audio.resize(_audioTrackCount);
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

	if (hasExtraData) {
		if (packet.video.dataSize >= 4) {
			Common::SeekableSubReadStream extraData(_xmv.get(), _xmv->pos(), _xmv->pos() + 4);

			_videoTrack->initCodec(extraData);

			packet.video.dataSize   -= 4;
			packet.video.dataOffset += 4;
		} else {
			warning("XboxMediaVideo::processPacketHeader(): Video extra data doesn't fit");
		}
	}
}

void XboxMediaVideo::decodeNextTrackFrame(VideoTrack &track) {
	// No frames left => we finished playing
	if (_curPacket.video.frameCount == 0) {
		static_cast<XMVVideoTrack &>(track).finish();

		for (uint32_t i = 0; i < _audioTrackCount; i++)
			if (_curPacket.audio[i].track)
				_curPacket.audio[i].track->finish();

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

XboxMediaVideo::XMVVideoTrack::XMVVideoTrack(uint32_t width, uint32_t height, uint32_t &timestamp) : _width(width), _height(height), _timestamp(timestamp), _curFrame(-1), _finished(false) {
}

bool XboxMediaVideo::XMVVideoTrack::decodeFrame(Graphics::Surface &surface, Common::SeekableReadStream &frameData) {
	_curFrame++;

	if (!_videoCodec)
		return false;

	_videoCodec->decodeFrame(surface, frameData);
	return true;
}

void XboxMediaVideo::XMVVideoTrack::initCodec(Common::SeekableReadStream &extraData) {
	_videoCodec = std::make_unique<XMVWMV2Codec>(_width, _height, extraData);
}

Common::Timestamp XboxMediaVideo::XMVVideoTrack::getNextFrameStartTime() const {
	if (_curFrame < 0)
		return 0;

	return Common::Timestamp(0, _timestamp, 1000);
}

XboxMediaVideo::XMVAudioTrack::XMVAudioTrack(const XboxMediaVideo::AudioInfo &info) : _info(info) {
	_audioStream.reset(createStream());
}

bool XboxMediaVideo::XMVAudioTrack::canBufferData() const {
	return !_audioStream->endOfStream();
}

void XboxMediaVideo::XMVAudioTrack::queueAudio(Common::SeekableReadStream *stream) {
	_audioStream->queuePacket(stream);
}

void XboxMediaVideo::XMVAudioTrack::finish() {
	_audioStream->finish();
}

Sound::AudioStream *XboxMediaVideo::XMVAudioTrack::getAudioStream() const {
	return _audioStream.get();
}

Sound::PacketizedAudioStream *XboxMediaVideo::XMVAudioTrack::createStream() const {
	// Check some parameters
	if (_info.channels == 0 || _info.channels > 2)
		throw Common::Exception("Invalid channel count: %d", _info.channels);

	switch (_info.compression) {
	case Sound::kWavePCM: {
		byte flags = Sound::FLAG_LITTLE_ENDIAN;

		if (_info.bitsPerSample == 16) {
			flags |= Sound::FLAG_16BITS;
		} else if (_info.bitsPerSample != 8) {
			throw Common::Exception("Invalid PCM sample size: %d", _info.bitsPerSample);
		}

		return Sound::makePacketizedPCMStream(_info.rate, flags, _info.channels);
	}
	case Sound::kWaveMSIMAADPCM2:
		if (_info.bitsPerSample != 4)
			throw Common::Exception("Invalid ADPCM sample size: %d", _info.bitsPerSample);

		return Sound::makePacketizedADPCMStream(Sound::kADPCMMSIma, _info.rate, _info.channels, _info.channels * 36);
	default:
		throw Common::Exception("Unhandled XMV wave format: %d", _info.compression);
	}
}

XboxMediaVideo::XMVAudioTrack51::XMVAudioTrack51(XMVAudioTrack *track1, XMVAudioTrack *track2, XMVAudioTrack *track3) {
	_realTracks[0].reset(track1);
	_realTracks[1].reset(track2);
	_realTracks[2].reset(track3);

	std::vector<Sound::AudioStream *> interleavedStreams;

	for (int i = 0; i < 3; i++)
		interleavedStreams.push_back(_realTracks[i]->getAudioStream());

	_interleaved.reset(Sound::makeInterleaver(interleavedStreams[0]->getRate(), interleavedStreams, false));
}

bool XboxMediaVideo::XMVAudioTrack51::canBufferData() const {
	return !_interleaved->endOfStream();
}

Sound::AudioStream *XboxMediaVideo::XMVAudioTrack51::getAudioStream() const {
	return _interleaved.get();
}

} // End of namespace Video
