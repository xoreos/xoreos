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

/** @file graphics/images/tga.cpp
 *  Decoding TGA (TarGa) images.
 */

#include "common/util.h"
#include "common/stream.h"
#include "common/error.h"

#include "graphics/images/tga.h"

namespace Graphics {

TGA::TGA(Common::SeekableReadStream &tga) {
	_compressed = false;

	load(tga);
}

TGA::~TGA() {
}

void TGA::load(Common::SeekableReadStream &tga) {
	try {

		byte imageType;
		readHeader(tga, imageType);
		readData  (tga, imageType);

		if (tga.err())
			throw Common::Exception(Common::kReadError);

	} catch (Common::Exception &e) {
		e.add("Failed reading TGA file");
		throw e;
	}
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

	_mipMaps.push_back(new MipMap);

	// Image dimensions
	_mipMaps[0]->width  = tga.readUint16LE();
	_mipMaps[0]->height = tga.readUint16LE();

	// Bits per pixel
	byte pixelDepth = tga.readByte();

	if (imageType == 2) {
		if      (pixelDepth == 24) {
			_hasAlpha  = false;
			_format    = kPixelFormatBGR;
			_formatRaw = kPixelFormatRGB8;
			_dataType  = kPixelDataType8;
		} else if (pixelDepth == 32) {
			_hasAlpha  = true;
			_format    = kPixelFormatBGRA;
			_formatRaw = kPixelFormatRGBA8;
			_dataType  = kPixelDataType8;
		} else
			throw Common::Exception("Unsupported pixel depth: %d, %d", imageType, pixelDepth);
	} else if (imageType == 3) {
		if (pixelDepth != 8)
			throw Common::Exception("Unsupported pixel depth: %d, %d", imageType, pixelDepth);

		_hasAlpha  = false;
		_format    = kPixelFormatBGRA;
		_formatRaw = kPixelFormatRGBA8;
		_dataType  = kPixelDataType8;
	}

	// Image descriptor
	tga.skip(1);

	// Skip the id string
	tga.skip(idLength);
}

void TGA::readData(Common::SeekableReadStream &tga, byte imageType) {
	if (imageType == 2) {
		_mipMaps[0]->size = _mipMaps[0]->width * _mipMaps[0]->height;
		if      (_format == kPixelFormatBGR)
			_mipMaps[0]->size *= 3;
		else if (_format == kPixelFormatBGRA)
			_mipMaps[0]->size *= 4;

		_mipMaps[0]->data = new byte[_mipMaps[0]->size];

		tga.read(_mipMaps[0]->data, _mipMaps[0]->size);

	} else if (imageType == 3) {
		_mipMaps[0]->size = _mipMaps[0]->width * _mipMaps[0]->height * 4;
		_mipMaps[0]->data = new byte[_mipMaps[0]->size];

		byte  *data  = _mipMaps[0]->data;
		uint32 count = _mipMaps[0]->width * _mipMaps[0]->height;

		while (count-- > 0) {
			byte g = tga.readByte();

			memset(data, g, 3);
			data[3] = 0xFF;

			data += 4;
		}

	}
}

} // End of namespace Graphics
