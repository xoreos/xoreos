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

	uint32 size = _tga->size();
	if (_tga->size() < 26)
		throw Common::Exception("Not a TGA file");

	try {

		if (!_tga->seek(size - 18))
			throw Common::Exception(Common::kSeekError);

		char buf[17];

		if (_tga->read(buf, 17) != 17)
			throw Common::Exception(Common::kReadError);

		if (strncmp(buf, "TRUEVISION-XFILE.", 17))
			throw Common::Exception("Signature not found");

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

	if      (pixelDepth == 24)
		_format = kPixelFormatBGR;
	else if (pixelDepth == 32)
		_format = kPixelFormatBGRA;
	else
		throw Common::Exception("Unsupported pixel depth: %d", pixelDepth);

	byte imageDescriptor = tga.readByte();
	if (((pixelDepth == 24) && (imageDescriptor != 0)) ||
	    ((pixelDepth == 32) && (imageDescriptor != 8)))
		throw Common::Exception("Unsupported image descriptor: 0x%02X", imageDescriptor);

	tga.skip(idLength);
}

void TGA::readData(Common::SeekableReadStream &tga) {
	uint32 rowBytes = _width;
	if      (_format == kPixelFormatBGR)
		rowBytes *= 3;
	else if (_format == kPixelFormatBGRA)
		rowBytes *= 4;

	_data = new byte[rowBytes * _height];

	byte *data = _data + ((_height - 1) * rowBytes);
	for (int i = 0; i < _height; i++) {
		byte *rowData = data;

		if (tga.read(rowData, rowBytes) != rowBytes)
			throw Common::Exception(Common::kReadError);

		data -= rowBytes;
	}
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
