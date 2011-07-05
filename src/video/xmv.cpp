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

#include "video/xmv.h"

#include "sound/decoders/adpcm.h"

namespace Video {

XboxMediaVideo::XboxMediaVideo(Common::SeekableReadStream *xmv) : _xmv(xmv) {
	assert(_xmv);

	load();
}

XboxMediaVideo::~XboxMediaVideo() {
	VideoDecoder::deinit();

	delete _xmv;
}

uint32 XboxMediaVideo::getTimeToNextFrame() const {
	return 0;
}

void XboxMediaVideo::startVideo() {
}

void XboxMediaVideo::processData() {
	throw Common::Exception("STUB: XboxMediaVideo::processData()");
}

void XboxMediaVideo::load() {
	_xmv->skip(8); // (???, min packet size?)

	uint32 packetSize = _xmv->readUint32LE();

	if (_xmv->readUint32LE() != MKID_BE('Xbox'))
		throw Common::Exception("XboxMediaVideo::load(): No 'Xbox' tag");

	_xmv->skip(4); // unknown (always 4? version?)
	uint32 width = _xmv->readUint32LE();
	uint32 height = _xmv->readUint32LE();
	uint32 duration = _xmv->readUint32LE();

	uint32 audioTrackCount = _xmv->readUint32LE();
	_audioTracks.resize(audioTrackCount);

	Common::UString exceptionString = "STUB: XboxMediaVideo::load():\n";
	exceptionString += Common::UString::sprintf("  Packet Size: %d\n", packetSize);
	exceptionString += Common::UString::sprintf("  Res: %dx%d\n", width, height);
	exceptionString += Common::UString::sprintf("  Duration: %dms\n", duration);
	exceptionString += Common::UString::sprintf("  Audio Track Count: %d\n", audioTrackCount);

	for (uint32 i = 0; i < audioTrackCount; i++) {
		_audioTracks[i].compression = _xmv->readUint16LE();
		_audioTracks[i].channels = _xmv->readUint16LE();
		_audioTracks[i].rate = _xmv->readUint32LE();
		_audioTracks[i].bitsPerSample = _xmv->readUint16LE();
		_audioTracks[i].unk = _xmv->readUint16LE(); // block align?
		exceptionString += Common::UString::sprintf("    Track %d: 0x%04x %dch @%dHz, %d bits per sample, %d unk\n",
				i, _audioTracks[i].compression, _audioTracks[i].channels, _audioTracks[i].rate, _audioTracks[i].bitsPerSample, _audioTracks[i].unk);
	}

	// Next four bytes seem to repeat the packet size?
	// Four unknown bytes
	// One uint32 per audio/video track? (some sort of buffer size?)

	// Can only use one track atm
	_audioTrack = 0;

	// Initialize our sound stuff
	if (audioTrackCount != 0)
		initSound(_audioTracks[_audioTrack].rate, _audioTracks[_audioTrack].channels, true);

	// TODO: Everything else
	// Like video being WMV2 and stuff

	throw Common::Exception(exceptionString.c_str());
}

void XboxMediaVideo::queueAudioStream(Common::SeekableReadStream *stream) {
	if (_audioTracks.empty())
		return;

	switch (_audioTracks[_audioTrack].compression) {
	case 1:    // PCM
		// TODO: Where's the flags? Anyone have samples?
		warning("XboxMediaVideo::createAudioStream(): PCM not yet handled");
		break;
	case 0x69: // MS IMA ADPCM
		// TODO: Where's block align?
		warning("XboxMediaVideo::createAudioStream(): ADPCM not yet handled");
		//queueSound(new Sound::makeADPCMStream(stream, true, stream->size(), Sound::kADPCMMSIma, _audioRate, _audioChannels /* , _audioBlockAlign */));
		break;
	default:
		warning("XboxMediaVideo::createAudioStream(): Unknown audio compression 0x%04x", _audioTracks[_audioTrack].compression);
		break;
	}
}

} // End of namespace Video
