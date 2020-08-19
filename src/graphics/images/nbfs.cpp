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
 *  Nitro Basic File Screen, a simple raw Nintendo DS image.
 */

#include <cstring>

#include <memory>

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/readstream.h"

#include "src/graphics/images/nbfs.h"

namespace Graphics {

NBFS::NBFS(Common::SeekableReadStream &nbfs, Common::SeekableReadStream &nbfp,
           uint32_t width, uint32_t height) {

	load(nbfs, nbfp, width, height);
}

NBFS::~NBFS() {
}

void NBFS::load(Common::SeekableReadStream &nbfs, Common::SeekableReadStream &nbfp,
                uint32_t width, uint32_t height) {

	try {
		if (nbfs.size() != (width * height))
			throw Common::Exception("Dimensions mismatch (%u * %u != %u)", width, height, (uint)nbfs.size());

		if ((width >= 0x8000) || (height >= 0x8000))
			throw Common::Exception("Invalid dimensions of %ux%u", width, height);

		if (nbfp.size() > 512)
			throw Common::Exception("Too much palette data (%u bytes)", (uint)nbfp.size());

		std::unique_ptr<const byte[]> palette(readPalette(nbfp));
		readImage(nbfs, palette.get(), width, height);

	} catch (Common::Exception &e) {
		e.add("Failed reading NBFS file");
		throw;
	}
}

const byte *NBFS::readPalette(Common::SeekableReadStream &nbfp) {
	std::unique_ptr<byte[]> palette = std::make_unique<byte[]>(768);
	std::memset(palette.get(), 0, 768);

	const size_t count = MIN<size_t>(nbfp.size() / 2, 256) * 3;
	for (size_t i = 0; i < count; i += 3) {
		const uint16_t color = nbfp.readUint16LE();

		palette[i + 0] = ((color >> 10) & 0x1F) << 3;
		palette[i + 1] = ((color >>  5) & 0x1F) << 3;
		palette[i + 2] = ( color        & 0x1F) << 3;
	}

	return palette.release();
}

void NBFS::readImage(Common::SeekableReadStream &nbfs, const byte *palette,
                     uint32_t width, uint32_t height) {

	_format    = kPixelFormatBGRA;
	_formatRaw = kPixelFormatRGBA8;
	_dataType  = kPixelDataType8;

	_mipMaps.push_back(new MipMap);

	_mipMaps.back()->width  = width;
	_mipMaps.back()->height = height;
	_mipMaps.back()->size   = width * height * 4;

	_mipMaps.back()->data = std::make_unique<byte[]>(_mipMaps.back()->size);

	bool is0Transp = (palette[0] == 0xF8) && (palette[1] == 0x00) && (palette[2] == 0xF8);

	byte *data = _mipMaps.back()->data.get();
	for (uint32_t i = 0; i < (width * height); i++, data += 4) {
		uint8_t pixel = nbfs.readByte();

		data[0] = palette[pixel * 3 + 0];
		data[1] = palette[pixel * 3 + 1];
		data[2] = palette[pixel * 3 + 2];
		data[3] = ((pixel == 0) && is0Transp) ? 0x00 : 0xFF;
	}
}

} // End of namespace Graphics
