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
 *  Compressed BackGround Tiles, a BioWare image format found in Sonic.
 */

#include <cstdio>
#include <cstring>

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/readstream.h"

#include "src/aurora/2dafile.h"
#include "src/aurora/smallfile.h"

#include "src/graphics/images/cbgt.h"

namespace Graphics {

CBGT::ReadContext::ReadContext(Common::SeekableReadStream &c,
                               Common::SeekableReadStream &p,
                               Common::SeekableReadStream &t) :
	cbgt(&c), pal(&p), twoda(&t) {
}


CBGT::CBGT(Common::SeekableReadStream &cbgt, Common::SeekableReadStream &pal,
           Common::SeekableReadStream &twoda) {

	ReadContext ctx(cbgt, pal, twoda);

	try {
		load(ctx);
	} catch (Common::Exception &e) {
		e.add("Failed reading CBGT file");
		throw;
	}
}

CBGT::~CBGT() {
}

void CBGT::load(ReadContext &ctx) {
	readPalettes(ctx);
	readPaletteIndices(ctx);
	readCells(ctx);

	checkConsistency(ctx);

	drawImage(ctx);
}

void CBGT::readPalettes(ReadContext &ctx) {
	/* Read the palette data, several plain palettes of 256 BGR555 entries. */

	try {
		ctx.pal->seek(0);

		size_t size = ctx.pal->size();
		ctx.palettes.reserve((size + 1) / 512);

		while (size > 0) {
			const uint32_t paletteSize = MIN<size_t>(512, size);

			ctx.palettes.push_back(new byte[768]);
			byte *palette = ctx.palettes.back();

			const uint32_t colorCount = (paletteSize / 2) * 3;
			for (uint32_t i = 0; i < colorCount; i += 3) {
				const uint16_t color = ctx.pal->readUint16LE();

				palette[i + 0] = ((color >> 10) & 0x1F) << 3;
				palette[i + 1] = ((color >>  5) & 0x1F) << 3;
				palette[i + 2] = ( color        & 0x1F) << 3;
			}

			size -= paletteSize;
		}

		if (ctx.palettes.empty())
			throw Common::Exception("No palettes");

	} catch (Common::Exception &e) {
		e.add("Failed reading PAL file");
		throw e;
	}
}

void CBGT::readPaletteIndices(ReadContext &ctx) {
	/* Read the 2DA file providing the palette index mapping, as well as the width
	 * and height of the final image (by way of number of cells in X and Y direction).
	 *
	 * Basically, the 2DA file contains data in this format:
	 *
	 *   0             1             2             3
	 * 0 palette00.pal palette00.pal palette03.pal palette03.pal
	 * 1 palette00.pal palette00.pal palette03.pal palette03.pal
	 * 2 palette01.pal palette01.pal palette04.pal palette04.pal
	 * 3 palette01.pal palette01.pal palette04.pal palette04.pal
	 * 4 palette02.pal palette02.pal palette05.pal palette05.pal
	 * 5 palette02.pal palette02.pal palette05.pal palette05.pal
	 *
	 * This tells out that
	 * a) There's 4 cells in X direction, so the width of the image is 256 pixels
	 * b) There's 6 cells in Y direction, so the height of the image is 384 pixels
	 * c) Which cell uses which palette
	 */

	try {
		Aurora::TwoDAFile twoDA(*ctx.twoda);

		ctx.width  = twoDA.getColumnCount() * 64;
		ctx.height = twoDA.getRowCount()    * 64;

		if ((ctx.width == 0) || (ctx.width >= 0x8000) || (ctx.height == 0) || (ctx.height >= 0x8000))
			throw Common::Exception("Dimensions of %ux%u", ctx.width, ctx.height);

		ctx.maxPaletteIndex = 0;

		ctx.paletteIndices.reserve(twoDA.getColumnCount() * twoDA.getRowCount());
		for (uint32_t i = 0; i < twoDA.getRowCount(); i++) {
			const Aurora::TwoDARow &row = twoDA.getRow(i);

			for (uint32_t j = 0; j < twoDA.getColumnCount(); j++) {
				const Common::UString &palette = row.getString(j);

				int index = -1;

				int n = std::sscanf(palette.c_str(), "palette%d.pal", &index);
				if ((n != 1) || (index < 0))
					throw Common::Exception("Failed to parse \"%s\" into a palette index", palette.c_str());

				ctx.paletteIndices.push_back((size_t)index);

				ctx.maxPaletteIndex = MAX<size_t>(ctx.maxPaletteIndex, index);
			}
		}

	} catch (Common::Exception &e) {
		e.add("Failed reading 2DA file");
		throw e;
	}
}

void CBGT::readCells(ReadContext &ctx) {
	/* Read the cell data, each containing 64x64 pixels. Of course, since this
	 * is a *compressed* format, the data is compressed using the LZSS algorithm
	 * also used for .small files. */

	ctx.cells.reserve(4096);

	try {
		// Read the cell offset and sizes
		for (size_t i = 0; i < 4096; i++) {
			const uint32_t size   = ctx.cbgt->readUint16LE();
			const uint32_t offset = ctx.cbgt->readUint16LE() * 512;

			if (offset < 0x4000)
				break;

			ctx.cells.push_back(0);
			if (size == 0)
				continue;

			size_t pos = ctx.cbgt->pos();

			Common::SeekableSubReadStream cellData(ctx.cbgt, offset, offset + size);
			ctx.cells.back() = Aurora::Small::decompress(cellData);

			if (ctx.cells.back()->size() != 4096)
				throw Common::Exception("Invalid size for cell %u: %u", (uint)i, (uint)ctx.cells.back()->size());

			ctx.cbgt->seek(pos);
		}

		if (ctx.cells.empty())
			throw Common::Exception("No cells");

	} catch (Common::Exception &e) {
		e.add("Failed reading CBGT file");
		throw e;
	}
}

void CBGT::checkConsistency(ReadContext &ctx) {
	if (ctx.cells.size() != ctx.paletteIndices.size())
		throw Common::Exception("%u palette indices for %u cells",
		                        (uint)ctx.cells.size(), (uint)ctx.paletteIndices.size());

	if (ctx.maxPaletteIndex >= ctx.palettes.size())
		throw Common::Exception("Palette index %u out of range (%u)",
		                        (uint)ctx.maxPaletteIndex, (uint)ctx.palettes.size());
}

void CBGT::createImage(uint32_t width, uint32_t height) {
	_format    = kPixelFormatBGRA;
	_formatRaw = kPixelFormatRGBA8;
	_dataType  = kPixelDataType8;

	_mipMaps.push_back(new MipMap);
	_mipMaps.back()->width  = width;
	_mipMaps.back()->height = height;
	_mipMaps.back()->size   = width * height * 4;

	_mipMaps.back()->data = std::make_unique<byte[]>(_mipMaps.back()->size);
	std::memset(_mipMaps.back()->data.get(), 0, _mipMaps.back()->size);
}

void CBGT::drawImage(ReadContext &ctx) {
	/* Draw the actual image data.
	 *
	 * The image is made up of 64x64 pixel cells, each consisting of 64 8x8 pixel tiles.
	 * This would be 2x1 cells, each with 64 tiles, and each of those would be 8x8 pixel wide:
	 *
	 * C0T00 C0T01 C0T02 C0T03 C0T04 C0T05 C0T06 C0T07 C1T00 C1T01 C1T02 C1T03 C1T04 C1T05 C1T06 C1T07
	 * C0T08 C0T09 C0T10 C0T11 C0T12 C0T13 C0T14 C0T15 C1T08 C1T09 C1T10 C1T11 C1T12 C1T13 C1T14 C1T15
	 * C0T16 C0T17 C0T18 C0T19 C0T20 C0T21 C0T22 C0T23 C1T16 C1T17 C1T18 C1T19 C1T20 C1T21 C1T22 C1T23
	 * C0T24 C0T25 C0T26 C0T27 C0T28 C0T29 C0T30 C0T31 C1T24 C1T25 C1T26 C1T27 C1T28 C1T29 C1T30 C1T31
	 * C0T32 C0T33 C0T34 C0T35 C0T36 C0T37 C0T38 C0T39 C1T32 C1T33 C1T34 C1T35 C1T36 C1T37 C1T38 C1T39
	 * C0T40 C0T41 C0T42 C0T43 C0T44 C0T45 C0T46 C0T47 C1T40 C1T41 C1T42 C1T43 C1T44 C1T45 C1T46 C1T47
	 * C0T48 C0T49 C0T50 C0T51 C0T52 C0T53 C0T54 C0T55 C1T48 C1T49 C1T50 C1T51 C1T52 C1T53 C1T54 C1T55
	 * C0T56 C0T57 C0T58 C0T59 C0T60 C0T61 C0T62 C0T63 C1T56 C1T57 C1T58 C1T59 C1T60 C1T61 C1T62 C1T63
	 *
	 * This unswizzling of the data makes it...a bit complex. */

	createImage(ctx.width, ctx.height);


	const uint32_t cellWidth  = 64;
	const uint32_t cellHeight = 64;
	const uint32_t cellsX     = ctx.width  / cellWidth;

	const uint32_t tileWidth  = 8;
	const uint32_t tileHeight = 8;
	const uint32_t tilesX     = cellWidth  / tileWidth;
	const uint32_t tilesY     = cellHeight / tileHeight;

	byte *data = _mipMaps.back()->data.get();
	for (size_t i = 0; i < ctx.cells.size(); i++) {
		Common::SeekableReadStream *cell = ctx.cells[i];
		if (!cell)
			continue;

		const uint32_t xC = i % cellsX;
		const uint32_t yC = i / cellsX;

		const byte *palette = ctx.palettes[ctx.paletteIndices[i]];
		const bool is0Transp = (palette[0] == 0xF8) && (palette[1] == 0x00) && (palette[2] == 0xF8);

		// Pixel position of this cell within the big image
		const uint32_t imagePos = yC * cellHeight * ctx.width + xC * cellWidth;

		// Go over all tiles
		for (uint32_t yT = 0; yT < tilesY; yT++) {
			for (uint32_t xT = 0; xT < tilesX; xT++) {

				// Position of the tile within the cell
				const uint32_t tilePos = xT * tileWidth + yT * tileHeight * ctx.width;

				// Go over all pixels in the tile
				for (uint32_t y = 0; y < tileHeight; y++) {
					for (uint32_t x = 0; x < tileWidth; x++) {

						// Position of the pixel within the tile
						const uint32_t pos   = imagePos + tilePos + x + y * ctx.width;
						const uint8_t  pixel = cell->readByte();

						if (pos > (ctx.width * ctx.height))
							continue;

						data[pos * 4 + 0] = palette[pixel * 3 + 0];
						data[pos * 4 + 1] = palette[pixel * 3 + 1];
						data[pos * 4 + 2] = palette[pixel * 3 + 2];
						data[pos * 4 + 3] = ((pixel == 0) && is0Transp) ? 0x00 : 0xFF;

					}
				}

			}
		}

	}
}

} // End of namespace Graphics
