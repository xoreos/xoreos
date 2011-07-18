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

#include "sound/decoders/wave_types.h"
#include "sound/decoders/pcm.h"
#include "sound/decoders/adpcm.h"

#include "events/events.h"

#include "video/xmv.h"

namespace Video {

XboxMediaVideo::XboxMediaVideo(Common::SeekableReadStream *xmv) : _xmv(xmv),
	_nextFrameTime(0), _curPacket(0) {

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

	uint32 curTime = EventMan.getTimestamp();
	if (_nextFrameTime < curTime)
		return 0;

	return _nextFrameTime - curTime;
}

void XboxMediaVideo::startVideo() {
	_started = true;
}

void XboxMediaVideo::processPacketHeader(PacketHeader &packetHeader) {
	// Next packet size

	packetHeader.nextPacketSize = _xmv->readUint32LE();
	_thisPacketSize -= 4;

	// Packet video header

	_xmv->read(packetHeader.video.header, 8);
	_thisPacketSize -= 8;

	packetHeader.video.dataSize = READ_LE_UINT32(packetHeader.video.header) & 0x007FFFFF;

	// Adding the audio data sizes and the video data size keeps you 4 bytes short
	// for every audio track. But as playing around with XMV files with ADPCM audio
	// showed, taking the extra 4 bytes from the audio data gives you either
	// completely distorted audio or click (when skipping the remaining 68 bytes of
	// the ADPCM block). Substracting _audioTracks.size() * 4 bytes from the video
	// data works at least for the audio. No idea why or how, though...
	packetHeader.video.dataSize -= _audioTracks.size() * 4;

	// Packet audio header

	packetHeader.audio.resize(_audioTracks.size());

	for (uint i = 0; i < packetHeader.audio.size(); i++) {
		PacketAudioHeader &audioHeader = packetHeader.audio[i];

		_xmv->read(audioHeader.header, 4);
		_thisPacketSize -= 4;

		audioHeader.dataSize = READ_LE_UINT32(audioHeader.header) & 0x007FFFFF;
		if ((audioHeader.dataSize == 0) && (i != 0))
			// Size is 0, take the size from last track
			audioHeader.dataSize = packetHeader.audio[i - 1].dataSize;
	}
}

void XboxMediaVideo::processVideoData(PacketVideoHeader &videoHeader) {
	if (videoHeader.dataSize > _thisPacketSize)
		throw Common::Exception("XboxMediaVideo::processVideoData(): Packet data overrun");

	_xmv->skip(videoHeader.dataSize);
	_thisPacketSize -= videoHeader.dataSize;
}

void XboxMediaVideo::processAudioData(PacketAudioHeader &audioHeader,
                                      const AudioTrack &track) {

	if (audioHeader.dataSize > _thisPacketSize)
		throw Common::Exception("XboxMediaVideo::processAudioData(): Packet data overrun");

	if (!track.enabled) {
		// Not a track we want, so we skip it

		_xmv->skip(audioHeader.dataSize);
		_thisPacketSize -= audioHeader.dataSize;

		return;
	}

	Common::SeekableReadStream *stream = _xmv->readStream(audioHeader.dataSize);
	_thisPacketSize -= audioHeader.dataSize;

	queueAudioStream(stream, track);

	uint32 audioLength = ((audioHeader.dataSize << 3) * 1000) / track.bitRate;
	_audioLength = MAX(_audioLength, audioLength);
}

void XboxMediaVideo::processAudioData(std::vector<PacketAudioHeader> &audioHeader) {
	for (uint32 t = 0; t < audioHeader.size(); t++)
		processAudioData(audioHeader[t], _audioTracks[t]);
}

void XboxMediaVideo::processData() {
	_audioLength    = 0;
	_thisPacketSize = _nextPacketSize;

	if (_thisPacketSize < (12 + _audioTracks.size() * 4)) {
		_finished = true;
		return;
	}

	if (_xmv->err() || _xmv->eos() || (_thisPacketSize > (_xmv->size() - _xmv->pos())))
		throw Common::Exception(Common::kReadError);

	PacketHeader packetHeader;
	processPacketHeader(packetHeader);


	processVideoData(packetHeader.video);
	processAudioData(packetHeader.audio);


	_xmv->skip(_thisPacketSize);

	_nextPacketSize = packetHeader.nextPacketSize;

	_nextFrameTime = _audioLength + EventMan.getTimestamp();
	_curPacket++;
}

void XboxMediaVideo::load() {
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
	uint32 duration = _xmv->readUint32LE(); // In ms

	warning("XMV: %dx%d for %d ms", width, height, duration);

	uint32 audioTrackCount = _xmv->readUint16LE();
	_audioTracks.resize(audioTrackCount);

	_xmv->skip(2); // Unknown

	for (uint32 i = 0; i < audioTrackCount; i++) {
		_audioTracks[i].compression   = _xmv->readUint16LE();
		_audioTracks[i].channels      = _xmv->readUint16LE();
		_audioTracks[i].rate          = _xmv->readUint32LE();
		_audioTracks[i].bitsPerSample = _xmv->readUint16LE();
		_audioTracks[i].flags         = _xmv->readUint16LE();

		evalAudioTrack(_audioTracks[i]);
	}

	_nextPacketSize = thisPacketSize - _xmv->pos();

	std::vector<uint32> audioDataSize;
	audioDataSize.resize(audioTrackCount);


	// Initialize the video
	initVideo(width, height);

	// Initialize the sound: Find the first supported audio track for now
	for (uint32 i = 0; i < audioTrackCount; i++) {
		if (_audioTracks[i].supported) {
			_audioTracks[i].enabled = true;
			initSound(_audioTracks[i].rate, _audioTracks[i].channels, true);
			break;
		}
	}

	_nextFrameTime = 0;
	_curPacket     = 0;
}

void XboxMediaVideo::evalAudioTrack(AudioTrack &track) {
	track.supported = true;
	track.enabled   = false;

	track.bitRate = track.channels * track.bitsPerSample * track.rate;

	track.audioStreamFlags = Sound::FLAG_LITTLE_ENDIAN;
	if (track.channels == 2)
		track.audioStreamFlags |= Sound::FLAG_STEREO;

	if ((track.channels == 0) || (track.channels > 2)) {
		warning("XboxMediaVideo::evalAudioTrack(): Unsupported channel count %d",
		        track.channels);

		track.supported = false;
	}

	switch (track.compression) {
		case Sound::kWavePCM:
			if        (track.bitsPerSample == 16) {
				track.audioStreamFlags |= Sound::FLAG_16BITS;
			} else if (track.bitsPerSample !=  8) {
				warning("XboxMediaVideo::evalAudioTrack(): Invalid bits per sample value for "
				        "raw PCM audio: %d", track.bitsPerSample);
				track.supported = false;
			}

			break;

		case Sound::kWaveMSIMAADPCM2:
			track.audioStreamFlags |= Sound::FLAG_16BITS;

			if (track.bitsPerSample != 4) {
				warning("XboxMediaVideo::evalAudioTrack(): Invalid bits per sample value for "
				        "MS IMA ADPCM audio: %d", track.bitsPerSample);
				track.supported = false;
			}
			break;

		default:
			warning("XboxMediaVideo::evalAudioTrack(): Unknown audio compression 0x%04x",
			        track.compression);
			track.supported = false;
			break;
	}

}

void XboxMediaVideo::queueAudioStream(Common::SeekableReadStream *stream,
                                      const AudioTrack &track) {

	if (!track.supported)
		return;

	Sound::AudioStream *audioStream = 0;

	switch (track.compression) {
		case Sound::kWavePCM:
			audioStream =
				Sound::makePCMStream(stream, track.rate, track.audioStreamFlags, true);
			break;

		case Sound::kWaveMSIMAADPCM2:
			audioStream =
				Sound::makeADPCMStream(stream, true, stream->size(),
				                       Sound::kADPCMMSIma, track.rate,
				                       track.channels, 72);
			break;

		default:
			delete stream;
			break;
	}

	if (audioStream)
		queueSound(audioStream);

}

} // End of namespace Video
