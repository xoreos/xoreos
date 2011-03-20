/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/video/xmv.cpp
 *  Decoding Microsoft Xbox XMV videos.
 */

#include "common/error.h"
#include "common/stream.h"

#include "graphics/video/xmv.h"

namespace Graphics {

XboxMediaVideo::XboxMediaVideo(Common::SeekableReadStream *xmv) : _xmv(xmv) {
	assert(_xmv);

	load();
}

XboxMediaVideo::~XboxMediaVideo() {
	delete _xmv;
}

bool XboxMediaVideo::hasTime() const {
	return false;
}

void XboxMediaVideo::processData() {
	throw Common::Exception("STUB: XboxMediaVideo::processData()");
}

void XboxMediaVideo::load() {
	_xmv->skip(12); // unknown

	if (_xmv->readUint32LE() != MKID_BE('Xbox'))
		throw Common::Exception("XboxMediaVideo::load(): No 'Xbox' tag");

	_xmv->skip(4); // unknown
	uint32 width = _xmv->readUint32LE();
	uint32 height = _xmv->readUint32LE();
	_xmv->skip(10); // unknown
	uint16 audioChannelCount = _xmv->readUint16LE();
	uint32 audioRate = _xmv->readUint32LE();

	throw Common::Exception("STUB: XboxMediaVideo::load() - Video: %dx%d, Audio: %dch @%dHz", width, height, audioChannelCount, audioRate);
}

} // End of namespace Graphics
