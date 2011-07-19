/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file video/xmv.cpp
 *  Decoding Microsoft Xbox XMV videos.
 */

#include "common/error.h"
#include "common/stream.h"

#include "sound/audiostream.h"
#include "sound/interleaver.h"

#include "sound/decoders/wave_types.h"
#include "sound/decoders/pcm.h"
#include "sound/decoders/adpcm.h"

#include "events/events.h"

#include "video/xmv.h"

static const int kAudioFlagADPCM51FrontLeftRight = 1;
static const int kAudioFlagADPCM51FrontCenterLow = 2;
static const int kAudioFlagADPCM51RearLeftRight  = 4;
static const int kAudioFlagADPCM51               = kAudioFlagADPCM51FrontLeftRight |
                                                   kAudioFlagADPCM51FrontCenterLow |
                                                   kAudioFlagADPCM51RearLeftRight;

namespace Video {

XboxMediaVideo::ADPCM51Streams::ADPCM51Streams() : enabled(false) {
	streams.resize(3);
	streams[0] = 0;
	streams[1] = 0;
	streams[2] = 0;
}

XboxMediaVideo::ADPCM51Streams::~ADPCM51Streams() {
	delete streams[0];
	delete streams[1];
	delete streams[2];
}


XboxMediaVideo::XboxMediaVideo(Common::SeekableReadStream *xmv) : _xmv(xmv), _startTime(0) {
	assert(_xmv);

	load();
}

XboxMediaVideo::~XboxMediaVideo() {
	VideoDecoder::deinit();

	delete _xmv;
}

uint32 XboxMediaVideo::getTimeToNextFrame() const {
	if (!_started)
		return 0;

	uint32 curTime = EventMan.getTimestamp() - _startTime;
	if (curTime >= _curPacket.video.currentFrameTimestamp)
		return 0;

	return _curPacket.video.currentFrameTimestamp - curTime;
}

void XboxMediaVideo::startVideo() {
	queueNewAudio(_curPacket);

	_startTime = EventMan.getTimestamp();
	_started   = true;
}

void XboxMediaVideo::queueNewAudio(PacketAudio &audioPacket) {
	// New data available that we should play?
	if (!audioPacket.newSlice || !audioPacket.track->enabled)
		return;

	// Seek to it
	if (!_xmv->seek(audioPacket.dataOffset))
		throw Common::Exception(Common::kSeekError);

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
	if (!_xmv->seek(videoPacket.dataOffset))
		throw Common::Exception(Common::kSeekError);

	// Read the frame header

	uint32 frameHeader = _xmv->readUint32LE();

	videoPacket.currentFrameSize      = (frameHeader & 0x1FFFF) * 4 + 4;
	videoPacket.currentFrameTimestamp = (frameHeader >> 17) + videoPacket.lastFrameTime;

	if (videoPacket.currentFrameSize > videoPacket.dataSize)
		throw Common::Exception("XboxMediaVideo::processNextFrame(): Frame data overrun");

	// TODO: Process the frame data

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

	if (_xmv->readUint32LE() != MKID_BE('Xbox'))
		throw Common::Exception("XboxMediaVideo::load(): No 'Xbox' tag");

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
	if (!_xmv->seek(packet.thisPacketOffset))
		throw Common::Exception(Common::kSeekError);

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

	packet.video.hasKeyFrame = (videoHeaderData[3] & 0x80) != 0;

	// Adding the audio data sizes and the video data size keeps you 4 bytes short
	// for every audio track. But as playing around with XMV files with ADPCM audio
	// showed, taking the extra 4 bytes from the audio data gives you either
	// completely distorted audio or click (when skipping the remaining 68 bytes of
	// the ADPCM block). Substracting _audioTracks.size() * 4 bytes from the video
	// data works at least for the audio. Probably some alignment thing?
	// The video data has (always?) lots of padding, so it should work out regardless.
	packet.video.dataSize -= _audioTracks.size() * 4;

	// Packet audio header

	packet.audio.resize(_audioTracks.size());
	for (uint i = 0; i < packet.audio.size(); i++) {
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

	uint32 dataOffset = _xmv->pos();

	packet.video.dataOffset = dataOffset;
	dataOffset += packet.video.dataSize;

	for (uint i = 0; i < packet.audio.size(); i++) {
		packet.audio[i].dataOffset = dataOffset;
		dataOffset += packet.audio[i].dataSize;
	}

	// Video frames header

	memset(packet.video.keyFrameFlags, 0, 4);

	if (packet.video.dataSize > 0) {
		if (packet.video.hasKeyFrame) {
			_xmv->read(packet.video.keyFrameFlags, 4);

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

	// Check for read errors
	if (_xmv->err() || _xmv->eos())
		throw Common::Exception(Common::kReadError);
}

void XboxMediaVideo::queueAudioStream(Common::SeekableReadStream *stream,
                                      const AudioTrack &track) {

	// No stream or not a supported track
	if (!stream || !track.supported) {
		delete stream;
		return;
	}

	Sound::AudioStream *audioStream = 0;

	// Create the audio stream
	switch (track.compression) {
		case Sound::kWavePCM:
			audioStream =
				Sound::makePCMStream(stream, track.rate, track.audioStreamFlags, track.channels, true);
			break;

		case Sound::kWaveMSIMAADPCM2:
			audioStream =
				Sound::makeADPCMStream(stream, true, stream->size(),
				                       Sound::kADPCMMSIma, track.rate,
				                       track.channels, 36 * track.channels);
			break;

		default:
			delete stream;
			break;
	}

	// Queue it
	if (audioStream) {

		// Look if we have to do ADPCM 5.1 interleaving
		if (_adpcm51Streams.enabled) {
			Sound::AudioStream **targetStream = 0;

			// Find the right target for this stream
			     if (track.flags & kAudioFlagADPCM51FrontLeftRight)
				targetStream = &_adpcm51Streams.streams[0];
			else if (track.flags & kAudioFlagADPCM51FrontCenterLow)
				targetStream = &_adpcm51Streams.streams[1];
			else if (track.flags & kAudioFlagADPCM51RearLeftRight)
				targetStream = &_adpcm51Streams.streams[2];

			if (!targetStream) {
				warning("XboxMediaVideo::queueAudioStream(): Broken ADPCM 5.1 flags: 0x%04X",
				        track.flags);
				delete audioStream;
				return;
			}

			// Assign it
			delete *targetStream;
			*targetStream = audioStream;

			// If we have all 3 streams filled, create an interleaver and queue that
			if (_adpcm51Streams.streams[0] &&
			    _adpcm51Streams.streams[1] &&
			    _adpcm51Streams.streams[2]) {

				audioStream = Sound::makeInterleaver(track.rate, _adpcm51Streams.streams, true);

				_adpcm51Streams.streams[0] = 0;
				_adpcm51Streams.streams[1] = 0;
				_adpcm51Streams.streams[2] = 0;

				queueSound(audioStream);
			}

		} else
			queueSound(audioStream);
	}
}

} // End of namespace Video
