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

/** @file graphics/video/xmv.cpp
 *  Decoding Microsoft Xbox XMV videos.
 */

#include "common/error.h"
#include "common/stream.h"

#include "graphics/video/xmv.h"

#include "sound/decoders/adpcm.h"

namespace Graphics {

XboxMediaVideo::XboxMediaVideo(Common::SeekableReadStream *xmv) : _xmv(xmv) {
	assert(_xmv);

	load();
}

XboxMediaVideo::~XboxMediaVideo() {
	removeFromQueue(kQueueGLContainer);
	removeFromQueue(kQueueVideo);

	delete _xmv;
}

bool XboxMediaVideo::hasTime() const {
	return false;
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
	_xmv->skip(4); // unknown (audio-related? audio stream count might be the second one)
	_audioCompression = _xmv->readUint16LE(); // same as the WAVEFORMATEX compression field
	_audioChannels = _xmv->readUint16LE();
	_audioRate = _xmv->readUint32LE();
	/* uint32 audioBitsPerSample = */ _xmv->readUint32LE();
	// I'd wager the next four bytes are the audio flags, but I have no samples with this

	// Initialize our sound stuff
	initSound(_audioRate, _audioChannels, true);

	// TODO: Everything else
	// Like video being WMV2 and stuff

	throw Common::Exception("STUB: XboxMediaVideo::load() - Packet Size: %d, Duration: %dms, Video: %dx%d, Audio: 0x%04x %dch @%dHz", packetSize, duration, width, height, _audioCompression, _audioChannels, _audioRate);
}

void XboxMediaVideo::queueAudioStream(Common::SeekableReadStream *stream) {
	switch (_audioCompression) {
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
		warning("XboxMediaVideo::createAudioStream(): Unknown audio compression 0x%04x", _audioCompression);
		break;
	}
}

} // End of namespace Graphics
