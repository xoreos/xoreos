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

TGA::TGA() : _width(0), _height(0), _hasAlpha(false), _data(0) {
}

TGA::~TGA() {
	delete[] _data;
}

void TGA::clear() {
	delete[] _data;

	_width  = 0;
	_height = 0;

	_hasAlpha = false;

	_data = 0;
}

void TGA::load(Common::SeekableReadStream &tga) {
	clear();

	uint32 size = tga.size();
	if (tga.size() < 26)
		throw Common::Exception("Not a TGA file");

	try {

		if (!tga.seek(size - 18))
			throw Common::Exception(Common::kSeekError);

		char buf[17];

		if (tga.read(buf, 17) != 17)
			throw Common::Exception(Common::kReadError);

		if (strncmp(buf, "TRUEVISION-XFILE.", 17))
			throw Common::Exception("Signature not found");

		readHeader(tga);
		readData(tga);

		if (tga.err())
			throw Common::Exception(Common::kReadError);

	} catch (Common::Exception &e) {
		e.add("Failed reading TGA file");
		throw e;
	}

}

void TGA::readHeader(Common::SeekableReadStream &tga) {
	if (!tga.seek(1))
		throw Common::Exception(Common::kSeekError);

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
	if (pixelDepth != 3)
		throw Common::Exception("Unsupported pixel depth: %d", pixelDepth);

	byte imageDescriptor = tga.readByte();
	if (imageDescriptor != 0)
		throw Common::Exception("Unsupported image descriptor: 0x%02X", imageDescriptor);
}

void TGA::readData(Common::SeekableReadStream &tga) {
	uint32 dataLength = _width * _height;

	_data = new byte[dataLength];

	if (tga.read(_data, dataLength) != dataLength)
		throw Common::Exception(Common::kReadError);
}

int TGA::getWidth() const {
	return _width;
}

int TGA::getHeight() const {
	return _height;
}

bool TGA::hasAlpha() const {
	return _hasAlpha;
}

const byte *TGA::getData() const {
	return _data;
}

} // End of namespace Graphics
