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

/** @file video/actimagine.h
 *  Decoding Actimagine videos.
 */

#include "common/error.h"
#include "common/stream.h"

#include "video/actimagine.h"

namespace Video {

ActimagineDecoder::ActimagineDecoder(Common::SeekableReadStream *vx) : _vx(vx) {
	assert(_vx);

	load();
}

ActimagineDecoder::~ActimagineDecoder() {
	VideoDecoder::deinit();

	delete _vx;
}

uint32 ActimagineDecoder::getTimeToNextFrame() const {
	return 0;
}

void ActimagineDecoder::startVideo() {
}

void ActimagineDecoder::processData() {
	throw Common::Exception("STUB: ActimagineDecoder::processData()");
}

void ActimagineDecoder::load() {
	if (_vx->readUint32BE() != MKTAG('V', 'X', 'D', 'S'))
		throw Common::Exception("Not a valid Actimagine video");

	_vx->readUint32LE(); // header size?
	_vx->readUint32LE(); // version? (0x100)
	_vx->readUint32LE(); // unknown
	_vx->readUint32LE(); // unknown
	_vx->readUint32LE(); // unknown
	uint32 sampleRate = _vx->readUint32LE(); // Oh yeah! I'm good at this!

	throw Common::Exception("STUB: ActimagineDecoder::load(): Sample rate = %d", sampleRate);
}


} // End of namespace Video
