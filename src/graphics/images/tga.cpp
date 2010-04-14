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

TGA::TGA(Common::SeekableReadStream *tga) : _tga(tga), _format(kPixelFormatBGRA), _formatRaw(kPixelFormatRGBA8) {
	assert(_tga);
}

TGA::~TGA() {
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

	_image.width  = tga.readUint16LE();
	_image.height = tga.readUint16LE();

	byte pixelDepth = tga.readByte();

	if      (pixelDepth == 24) {
		_format    = kPixelFormatBGR;
		_formatRaw = kPixelFormatRGB8;
	} else if (pixelDepth == 32) {
		_format    = kPixelFormatBGRA;
		_formatRaw = kPixelFormatRGBA8;
	} else
		throw Common::Exception("Unsupported pixel depth: %d", pixelDepth);

	// Image descriptor
	tga.skip(1);

	tga.skip(idLength);
}

void TGA::readData(Common::SeekableReadStream &tga) {
	_image.size = _image.width * _image.height;
	if      (_format == kPixelFormatBGR)
		_image.size *= 3;
	else if (_format == kPixelFormatBGRA)
		_image.size *= 4;

	_image.data = new byte[_image.size];

	tga.read(_image.data, _image.size);
}

bool TGA::isCompressed() const {
	return false;
}

PixelFormat TGA::getFormat() const {
	return _format;
}

PixelFormatRaw TGA::getFormatRaw() const {
	return _formatRaw;
}

PixelDataType TGA::getDataType() const {
	return kPixelDataType8;
}

int TGA::getMipMapCount() const {
	return 1;
}

const TGA::MipMap &TGA::getMipMap(int mipMap) const {
	return _image;
}

TGA::MipMap &TGA::getMipMap(int mipMap) {
	return _image;
}

} // End of namespace Graphics
