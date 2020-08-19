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

#ifndef GRAPHICS_IMAGES_CBGT_H
#define GRAPHICS_IMAGES_CBGT_H

#include <vector>

#include "src/common/ptrvector.h"

#include "src/graphics/images/decoder.h"

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {

/** Loader for CBGT, BioWare's Compressed BackGround Tiles, an image
 *  format found in Sonic, used as area background images.
 *
 *  A CBGT is similar to a collection of NCGR files: cells of 64x64
 *  pixels, divided in tiles of 8x8 pixels. Each cell is compressed
 *  using Nintendo's 0x10 LZSS algorithm, though. Moreover, PAL files
 *  with the same name as the CBGT file contain the palette data, and
 *  2DA files with the same name yet again contain the mapping of
 *  palette indices onto cells.
 *
 *  The width and height of the final image is not stored within the
 *  CBGT file, but the palette mapping 2DA does contain the number
 *  cells in X and Y direction, since it stores the palette indices
 *  in a two-dimensional array of those dimensions. Therefore, multi-
 *  plying the number of columns and rows in said 2DA by 64 yields
 *  the width and height, respectively, of the final image.
 */
class CBGT : public ImageDecoder {
public:
	CBGT(Common::SeekableReadStream &cbgt, Common::SeekableReadStream &pal,
	     Common::SeekableReadStream &twoda);
	~CBGT();

private:
	typedef Common::PtrVector<byte, Common::DeallocatorArray> Palettes;
	typedef std::vector<size_t> PaletteIndices;
	typedef Common::PtrVector<Common::SeekableReadStream> Cells;

	struct ReadContext {
		Common::SeekableReadStream *cbgt;
		Common::SeekableReadStream *pal;
		Common::SeekableReadStream *twoda;

		Palettes palettes;
		PaletteIndices paletteIndices;
		Cells cells;

		uint32_t width;
		uint32_t height;

		size_t maxPaletteIndex;

		ReadContext(Common::SeekableReadStream &c,
		            Common::SeekableReadStream &p,
		            Common::SeekableReadStream &t);
	};

	void load(ReadContext &ctx);

	void readPalettes(ReadContext &ctx);
	void readPaletteIndices(ReadContext &ctx);
	void readCells(ReadContext &ctx);

	void checkConsistency(ReadContext &ctx);

	void createImage(uint32_t width, uint32_t height);
	void drawImage(ReadContext &ctx);
};

} // End of namespace Graphics

#endif // GRAPHICS_IMAGES_CBGT_H
