/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/images/tga.h
 *  Decoding TGA (TarGa) images.
 */

#include "common/util.h"
#include "common/stream.h"
#include "common/error.h"

#include "graphics/images/tga.h"

namespace Graphics {

TGA::TGA(Common::SeekableReadStream *tga) : _tga(tga), _width(0), _height(0), _format(kPixelFormatRGB), _data(0) {
	assert(_tga);
}

TGA::~TGA() {
	delete[] _data;
}

void TGA::load() {
	if (!_tga)
		return;

	try {

		readHeader(*_tga);
		readData(*_tga);

		if (_tga->err())
			throw Common::Exception(Common::kReadError);

	} catch (Common::Exception &e) {
		e.add("Failed reading TGA file");
		throw e;
	}

	delete _tga;
	_tga = 0;
}

void TGA::readHeader(Common::SeekableReadStream &tga) {
	if (!tga.seek(0))
		throw Common::Exception(Common::kSeekError);

	uint32 idLength = tga.readByte();

	if (tga.readByte() != 0)
		throw Common::Exception("Unsupported feature: Color map");

	byte imageType = tga.readByte();
	if (imageType != 2)
		throw Common::Exception("Unsupported image type: %d", imageType);

	// Color map specifications + X + Y
	tga.skip(5 + 2 + 2);

	_width  = tga.readUint16LE();
	_height = tga.readUint16LE();

	byte pixelDepth = tga.readByte();
	byte imageDescriptor = tga.readByte();

	warning("--> %d, %d", pixelDepth, imageDescriptor);

	if      (pixelDepth == 24)
		_format = kPixelFormatBGR;
	else if (pixelDepth == 32)
		_format = kPixelFormatBGRA;
	else
		throw Common::Exception("Unsupported pixel depth: %d", pixelDepth);

	tga.skip(idLength);
}

void TGA::readData(Common::SeekableReadStream &tga) {
	uint32 bytes = _width * _height;
	if      (_format == kPixelFormatBGR)
		bytes *= 3;
	else if (_format == kPixelFormatBGRA)
		bytes *= 4;

	_data = new byte[bytes];

	tga.read(_data, bytes);
}

int TGA::getWidth() const {
	return _width;
}

int TGA::getHeight() const {
	return _height;
}

PixelFormat TGA::getFormat() const {
	return _format;
}

const byte *TGA::getData() const {
	return _data;
}

} // End of namespace Graphics
