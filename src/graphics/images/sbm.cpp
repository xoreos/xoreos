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

/** @file
 *  Decoding SBM (font bitmap data).
 */

#include <cstring>

#include "src/common/util.h"
#include "src/common/readstream.h"
#include "src/common/error.h"

#include "src/graphics/images/sbm.h"

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

	} catch (Common::Exception &e) {
		e.add("Failed reading SBM file");
		throw;
	}
}

void SBM::readData(Common::SeekableReadStream &sbm) {
	if ((sbm.size() % 1024) != 0)
		throw Common::Exception("Invalid SBM (%u)", (uint)sbm.size());

	size_t rowCount = (sbm.size() / 1024);

	_mipMaps.push_back(new MipMap(this));

	_mipMaps[0]->width  = 4 * 32;
	_mipMaps[0]->height = NEXTPOWER2((uint32) rowCount * 32);
	_mipMaps[0]->size   = _mipMaps[0]->width * _mipMaps[0]->height * 4;

	_mipMaps[0]->data.reset(new byte[_mipMaps[0]->size]);

	// SBM data consists of character sized 32 * 32 pixels, with 2 bits per pixel.
	// 4 characters each are on top of each other, occupying the same x/y
	// coordinates but different planes.
	// We'll unpack them and draw them next to each other instead.

	int masks [4] = { 0x03, 0x0C, 0x30, 0xC0 };
	int shifts[4] = {    0,    2,    4,    6 };

	byte *data = _mipMaps[0]->data.get();
	byte buffer[1024];
	for (size_t c = 0; c < rowCount; c++) {

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

	byte *dataEnd = _mipMaps[0]->data.get() + _mipMaps[0]->size;
	std::memset(data, 0, dataEnd - data);
}

} // End of namespace Graphics
