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
 *  Compressed DePTH, found in Sonic, holding image depth values.
 */

#include <cstring>

#include "src/common/util.h"
#include "src/common/scopedptr.h"
#include "src/common/ptrvector.h"
#include "src/common/error.h"
#include "src/common/readstream.h"

#include "src/aurora/cdpth.h"
#include "src/aurora/smallfile.h"

namespace Aurora {

typedef Common::PtrVector<Common::SeekableReadStream> Cells;

struct ReadContext {
	Common::SeekableReadStream *cdpth;

	Cells cells;

	uint32 width;
	uint32 height;

	Common::ScopedArray<uint16> depth;

	ReadContext(Common::SeekableReadStream &c, uint32 w, uint32 h) :
		cdpth(&c), width(w), height(h) {
	}
};

static void loadCDPTH(ReadContext &ctx);
static void readCells(ReadContext &ctx);
static void checkConsistency(ReadContext &ctx);
static void createDepth(ReadContext &ctx);

const uint16 *CDPTH::load(Common::SeekableReadStream &cdpth, uint32 width, uint32 height) {
	ReadContext ctx(cdpth, width, height);

	try {
		if ((ctx.width == 0) || (ctx.width >= 0x8000) || (ctx.height == 0) || (ctx.height >= 0x8000))
			throw Common::Exception("Invalid dimensions of %ux%u", ctx.width, ctx.height);

		if (((ctx.width % 64) != 0) || ((ctx.height % 64) != 0))
			throw Common::Exception("Dimensions need to be divisible by 64");

		loadCDPTH(ctx);
	} catch (Common::Exception &e) {
		e.add("Failed reading CDPTH file");
		throw;
	}

	return ctx.depth.release();
}

const uint16 *CDPTH::load(Common::SeekableReadStream *cdpth, uint32 width, uint32 height) {
	Common::ScopedPtr<Common::SeekableReadStream> stream(cdpth);

	assert(stream);
	return load(*stream, width, height);
}

static void loadCDPTH(ReadContext &ctx) {
	readCells(ctx);
	checkConsistency(ctx);
	createDepth(ctx);
}

static void readCells(ReadContext &ctx) {
	/* Read the cell data, each containing 64x64 pixels. Of course, since this
	 * is a *compressed* format, the data is compressed using the LZSS algorithm
	 * also used for .small files. */

	ctx.cells.reserve(4096);

	try {
		// Read the cell offset and sizes
		for (size_t i = 0; i < 4096; i++) {
			const uint32 size   = ctx.cdpth->readUint16LE();
			const uint32 offset = ctx.cdpth->readUint16LE() * 512;

			if (offset < 0x4000)
				break;

			ctx.cells.push_back(0);
			if (size == 0)
				continue;

			const size_t pos = ctx.cdpth->pos();

			Common::SeekableSubReadStream cellData(ctx.cdpth, offset, offset + size);
			ctx.cells.back() = Aurora::Small::decompress(cellData);

			if (ctx.cells.back()->size() != 8192)
				throw Common::Exception("Invalid size for cell %u: %u", (uint)i, (uint)ctx.cells.back()->size());

			ctx.cdpth->seek(pos);
		}

		if (ctx.cells.empty())
			throw Common::Exception("No cells");

	} catch (Common::Exception &e) {
		e.add("Failed reading CDPTH file");
		throw e;
	}
}

static void checkConsistency(ReadContext &ctx) {
	if (((ctx.width / 64) * (ctx.height / 64)) != ctx.cells.size())
		throw Common::Exception("%u cells for an image of %ux%u", (uint)ctx.cells.size(), ctx.width, ctx.height);
}

static void createDepth(ReadContext &ctx) {
	/* Create the actual depth data, which is made up of 64x64 pixel cells. */

	ctx.depth.reset(new uint16[ctx.width * ctx.height]);
	std::memset(ctx.depth.get(), 0xFF, ctx.width * ctx.height * sizeof(uint16));

	const uint32 cellWidth  = 64;
	const uint32 cellHeight = 64;
	const uint32 cellsX     = ctx.width  / cellWidth;

	uint16 *data = ctx.depth.get();
	for (size_t i = 0; i < ctx.cells.size(); i++) {
		Common::SeekableReadStream *cell = ctx.cells[i];
		if (!cell)
			continue;

		const uint32 xC = i % cellsX;
		const uint32 yC = i / cellsX;

		// Pixel position of this cell within the big image
		const uint32 imagePos = yC * cellHeight * ctx.width + xC * cellWidth;

		for (uint32 y = 0; y < cellHeight; y++) {
			for (uint32 x = 0; x < cellWidth; x++) {
				const uint32 pos   = imagePos + y * ctx.width + x;
				const uint16 pixel = cell->readUint16LE();

				if (pos > (ctx.width * ctx.height))
					continue;

				data[pos] = pixel;
			}
		}
	}
}

} // End of namespace Aurora
