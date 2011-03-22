/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/images/tga.cpp
 *  Decoding TGA (TarGa) images.
 */

#include "common/util.h"
#include "common/stream.h"
#include "common/error.h"

#include "graphics/images/tga.h"

namespace Graphics {

TGA::TGA(Common::SeekableReadStream *tga) : _tga(tga),
	_hasAlpha(false), _format(kPixelFormatBGRA), _formatRaw(kPixelFormatRGBA8) {

	assert(_tga);
}

TGA::~TGA() {
}

void TGA::load() {
	if (!_tga)
		return;

	try {

		byte imageType;
		readHeader(*_tga, imageType);
		readData(*_tga, imageType);

		if (_tga->err())
			throw Common::Exception(Common::kReadError);

	} catch (Common::Exception &e) {
		e.add("Failed reading TGA file");
		throw e;
	}

	delete _tga;
	_tga = 0;
}

void TGA::readHeader(Common::SeekableReadStream &tga, byte &imageType) {
	if (!tga.seek(0))
		throw Common::Exception(Common::kSeekError);

	// TGAs have an optional "id" string in the header
	uint32 idLength = tga.readByte();

	// Number of colors in the color map / palette
	if (tga.readByte() != 0)
		throw Common::Exception("Unsupported feature: Color map");

	// Image type. 2 == unmapped RGB, 3 == Grayscale
	imageType = tga.readByte();
	if ((imageType != 2) && (imageType != 3))
		throw Common::Exception("Unsupported image type: %d", imageType);

	// Color map specifications + X + Y
	tga.skip(5 + 2 + 2);

	// Image dimensions
	_image.width  = tga.readUint16LE();
	_image.height = tga.readUint16LE();

	// Bits per pixel
	byte pixelDepth = tga.readByte();

	if (imageType == 2) {
		if      (pixelDepth == 24) {
			_hasAlpha  = false;
			_format    = kPixelFormatBGR;
			_formatRaw = kPixelFormatRGB8;
		} else if (pixelDepth == 32) {
			_hasAlpha  = true;
			_format    = kPixelFormatBGRA;
			_formatRaw = kPixelFormatRGBA8;
		} else
			throw Common::Exception("Unsupported pixel depth: %d, %d", imageType, pixelDepth);
	} else if (imageType == 3) {
		if (pixelDepth != 8)
			throw Common::Exception("Unsupported pixel depth: %d, %d", imageType, pixelDepth);

		_hasAlpha  = false;
		_format    = kPixelFormatBGRA;
		_formatRaw = kPixelFormatRGBA8;
	}

	// Image descriptor
	tga.skip(1);

	// Skip the id string
	tga.skip(idLength);
}

void TGA::readData(Common::SeekableReadStream &tga, byte imageType) {
	if (imageType == 2) {
		_image.size = _image.width * _image.height;
		if      (_format == kPixelFormatBGR)
			_image.size *= 3;
		else if (_format == kPixelFormatBGRA)
			_image.size *= 4;

		_image.data = new byte[_image.size];

		tga.read(_image.data, _image.size);

	} else if (imageType == 3) {
		_image.size = _image.width * _image.height * 4;
		_image.data = new byte[_image.size];

		byte  *data  = _image.data;
		uint32 count = _image.width * _image.height;

		while (count-- > 0) {
			byte g = tga.readByte();

			memset(data, g, 3);
			data[3] = 0xFF;

			data += 4;
		}

	}
}

bool TGA::isCompressed() const {
	// TGAs are never compressed
	return false;
}

bool TGA::hasAlpha() const {
	return _hasAlpha;
}

PixelFormat TGA::getFormat() const {
	return _format;
}

PixelFormatRaw TGA::getFormatRaw() const {
	return _formatRaw;
}

PixelDataType TGA::getDataType() const {
	// TGA pixels always 888(8)
	return kPixelDataType8;
}

int TGA::getMipMapCount() const {
	// Just one image in TGAs
	return 1;
}

const TGA::MipMap &TGA::getMipMap(int mipMap) const {
	return _image;
}

TGA::MipMap &TGA::getMipMap(int mipMap) {
	return _image;
}

} // End of namespace Graphics
