/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
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
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file graphics/images/sbm.cpp
 *  Decoding SBM (font bitmap data).
 */

#include "common/util.h"
#include "common/stream.h"
#include "common/error.h"

#include "graphics/images/sbm.h"

namespace Graphics {

SBM::SBM(Common::SeekableReadStream &sbm) {
	_compressed = false;
	_hasAlpha   = true;
	_format     = kPixelFormatBGRA;
	_formatRaw  = kPixelFormatRGBA8;
	_dataType   = kPixelDataType8;

	load(sbm);
}

SBM::~SBM() {
}

void SBM::load(Common::SeekableReadStream &sbm) {
	try {

		readData(sbm);

		if (sbm.err())
			throw Common::Exception(Common::kReadError);

	} catch (Common::Exception &e) {
		e.add("Failed reading SBM file");
		throw;
	}
}

void SBM::readData(Common::SeekableReadStream &sbm) {
	if ((sbm.size() % 1024) != 0)
		throw Common::Exception("Invalid SBM (%d)", sbm.size());

	uint32 rowCount = (sbm.size() / 1024);

	_mipMaps.push_back(new MipMap);

	_mipMaps[0]->width  = 4 * 32;
	_mipMaps[0]->height = NEXTPOWER2((uint32) rowCount * 32);
	_mipMaps[0]->size   = _mipMaps[0]->width * _mipMaps[0]->height * 4;

	_mipMaps[0]->data = new byte[_mipMaps[0]->size];

	// SBM data consists of character sized 32 * 32 pixels, with 2 bits per pixel.
	// 4 characters each are on top of each other, occupying the same x/y
	// coordinates but different planes.
	// We'll unpack them and draw them next to each other instead.

	int masks [4] = { 0x03, 0x0C, 0x30, 0xC0 };
	int shifts[4] = {    0,    2,    4,    6 };

	byte *data = _mipMaps[0]->data;
	byte buffer[1024];
	for (uint32 c = 0; c < rowCount; c++) {

		if (sbm.read(buffer, 1024) != 1024)
			throw Common::Exception(Common::kReadError);

		for (int y = 0; y < 32; y++) {
			byte *src = buffer + y * 32;

			for (int plane = 0; plane < 4; plane++) {
				for (int x = 0; x < 32; x++) {
					byte a = ((src[x] & masks[plane]) >> shifts[plane]) * 0x55;

					*data++ = 0xFF; // B
					*data++ = 0xFF; // G
					*data++ = 0xFF; // R
					*data++ = a;    // A
				}
			}
		}
	}

	byte *dataEnd = _mipMaps[0]->data + _mipMaps[0]->size;
	memset(data, 0, dataEnd - data);
}

} // End of namespace Graphics
