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
 *  Nitro Character Graphic Resource, a Nintendo DS image format.
 *  Uses NCLR, Nitro CoLoR, palette files.
 */

/* Based heavily on the NCGR reader found in the NDS file viewer
 * and editor Tinke by pleoNeX (<https://github.com/pleonex/tinke>),
 * which is licensed under the terms of the GPLv3.
 *
 * Tinke in turn is based on the NCGR documentation by lowlines
 * (<http://llref.emutalk.net/docs/?file=xml/ncgr.xml>).
 *
 * The original copyright note in Tinke reads as follows:
 *
 * Copyright (C) 2011  pleoNeX
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "src/common/util.h"
#include "src/common/strutil.h"
#include "src/common/readstream.h"
#include "src/common/error.h"

#include "src/graphics/images/ncgr.h"
#include "src/graphics/images/nclr.h"

static const uint32_t kNCGRID = MKTAG('N', 'C', 'G', 'R');
static const uint32_t kCHARID = MKTAG('C', 'H', 'A', 'R');

namespace Graphics {

NCGR::NCGRFile::NCGRFile() : ncgr(0), image(0), width(0), height(0) {
}

NCGR::NCGRFile::~NCGRFile() {
	delete ncgr;
	delete image;
}


NCGR::NCGR(const std::vector<Common::SeekableReadStream *> &ncgrs, uint32_t width, uint32_t height,
           Common::SeekableReadStream &nclr) {

	try {
		load(ncgrs, width, height, nclr);
	} catch (Common::Exception &e) {
		e.add("Failed reading NCGR files");
		throw;
	}
}

NCGR::NCGR(Common::SeekableReadStream &ncgr, Common::SeekableReadStream &nclr) {
	std::vector<Common::SeekableReadStream *> ncgrs;
	ncgrs.push_back(&ncgr);

	try {
		load(ncgrs, 1, 1, nclr);
	} catch (Common::Exception &e) {
		e.add("Failed reading NCGR file");
		throw;
	}
}

void NCGR::load(const std::vector<Common::SeekableReadStream *> &ncgrs, uint32_t width, uint32_t height,
                Common::SeekableReadStream &nclr) {

	if ((width * height) != ncgrs.size())
		throw Common::Exception("%u NCGRs won't fill a grid of %ux%u", (uint)ncgrs.size(), width, height);

	ReadContext ctx;

	ctx.width  = width;
	ctx.height = height;

	ctx.pal.reset(NCLR::load(nclr));

	ctx.ncgrs.resize(ncgrs.size());

	for (size_t i = 0; i < ncgrs.size(); i++) {
		if (!ncgrs[i])
			continue;

		ctx.ncgrs[i].ncgr = open(*ncgrs[i]);

		load(ctx.ncgrs[i]);
	}

	draw(ctx);
}

NCGR::~NCGR() {
}

void NCGR::load(NCGRFile &ctx) {
	readHeader(ctx);
	readChar  (ctx);
}

void NCGR::readHeader(NCGRFile &ctx) {
	const uint32_t tag = ctx.ncgr->readUint32();
	if (tag != kNCGRID)
		throw Common::Exception("Invalid NCGR file (%s)", Common::debugTag(tag).c_str());

	const uint16_t bom = ctx.ncgr->readUint16();
	if (bom != 0xFEFF)
		throw Common::Exception("Invalid BOM: %u", bom);

	const uint8_t versionMinor = ctx.ncgr->readByte();
	const uint8_t versionMajor = ctx.ncgr->readByte();
	if ((versionMajor != 1) || (versionMinor != 1))
		throw Common::Exception("Unsupported version %u.%u", versionMajor, versionMinor);

	const uint32_t fileSize = ctx.ncgr->readUint32();
	if (fileSize > ctx.ncgr->size())
		throw Common::Exception("Size too large (%u > %u)", fileSize, (uint)ctx.ncgr->size());

	const uint16_t headerSize = ctx.ncgr->readUint16();
	if (headerSize != 16)
		throw Common::Exception("Invalid header size (%u)", headerSize);

	const uint16_t sectionCount = ctx.ncgr->readUint16();
	if ((sectionCount != 1) && (sectionCount != 2))
		throw Common::Exception("Invalid number of sections (%u)", sectionCount);

	ctx.offsetCHAR = headerSize;
}

void NCGR::readChar(NCGRFile &ctx) {
	/* The CHAR section contains the actual graphics data. */

	ctx.ncgr->seek(ctx.offsetCHAR);

	const uint32_t tag = ctx.ncgr->readUint32();
	if (tag != kCHARID)
		throw Common::Exception("Invalid CHAR section (%s)", Common::debugTag(tag).c_str());

	ctx.ncgr->skip(4);

	// Width and height in tiles (8x8 pixels)
	ctx.height = ctx.ncgr->readUint16() * 8;
	ctx.width  = ctx.ncgr->readUint16() * 8;

	/* TODO: Something weird is going on when width and height are 0xFFFF.
	 *
	 * Fiddling with the data of one of those images, they're somehow
	 * double-tiled? E.g. cbt_victory.ncgr is obviously an image with
	 * dimensions 128x32 (no idea where to get that information from,
	 * though), but it needs to be drawn as 64x64, and then the lower
	 * 64x32 half needs to be moved to the left of the upper half.
	 * Bad ASCII drawing for visualization:
	 * ______
	 * |ORY!|
	 * |VICT|
	 * ''''''
	 */

	if ((ctx.width >= 0x8000) || (ctx.height >= 0x8000))
		throw Common::Exception("Unsupported image dimensions");

	// depthValue == 3 means 4 bit graphics. We don't need to support them.
	const uint32_t depthValue = ctx.ncgr->readUint32();
	if (depthValue != 4)
		throw Common::Exception("Unsupported image depth %u", depthValue);

	ctx.depth = 8;

	ctx.ncgr->skip(4); // Unknown

	/* tiled == 0xFF apparently means the graphics are non-tiled.
	 * Would certainly make things easier, but none of the files
	 * are in this format, it seems. */
	const uint8_t tiled = ctx.ncgr->readByte();

	/* part == 0xFF apparently means that the image is portioned somehow?
	 * None of the Sonic files have this flag set, though. */
	const uint8_t part  = ctx.ncgr->readByte();

	if ((tiled != 0) || (part != 0))
		throw Common::Exception("Unsupported layout 0x%02X 0x%02X", tiled, part);

	ctx.ncgr->skip(2); // Unknown

	const uint32_t dataSize   = ctx.ncgr->readUint32();
	const uint32_t dataOffset = ctx.ncgr->readUint32() + 24;

	if ((dataOffset >= ctx.ncgr->size()) || ((ctx.ncgr->size() - dataOffset) < dataSize))
		throw Common::Exception("Invalid data offset (%u, %u, %u)",
		                        dataOffset, dataSize, (uint)ctx.ncgr->size());

	ctx.image = new Common::SeekableSubReadStream(ctx.ncgr, dataOffset, dataOffset + dataSize);
}

void NCGR::calculateGrid(ReadContext &ctx, uint32_t &imageWidth, uint32_t &imageHeight) {
	imageWidth = imageHeight = 0;

	// Go through the whole image and get the max height of a row of NCGR
	for (uint32_t y = 0; y < ctx.height; y++) {
		uint32_t rowHeight = 0;
		uint32_t rowWidth  = 0;

		for (uint32_t x = 0; x < ctx.width; x++) {
			NCGRFile &ncgr = ctx.ncgrs[y * ctx.width + x];
			ncgr.offsetX = rowWidth;
			ncgr.offsetY = imageHeight;

			rowHeight = MAX(rowHeight, ncgr.height);
			rowWidth += ncgr.width;
		}

		imageHeight += rowHeight;
		imageWidth   = MAX(imageWidth, rowWidth);
	}
}

void NCGR::draw(ReadContext &ctx) {
	uint32_t imageWidth, imageHeight;
	calculateGrid(ctx, imageWidth, imageHeight);

	if ((imageWidth >= 0x8000) || (imageHeight >= 0x8000))
		throw Common::Exception("Unsupported full image dimensions");

	_format    = kPixelFormatBGRA;
	_formatRaw = kPixelFormatRGBA8;
	_dataType  = kPixelDataType8;

	_mipMaps.push_back(new MipMap);

	_mipMaps.back()->width  = imageWidth;
	_mipMaps.back()->height = imageHeight;
	_mipMaps.back()->size   = imageWidth * imageHeight * 4;

	_mipMaps.back()->data = std::make_unique<byte[]>(_mipMaps.back()->size);
	byte *data = _mipMaps.back()->data.get();

	const bool is0Transp = (ctx.pal[0] == 0xF8) && (ctx.pal[1] == 0x00) && (ctx.pal[2] == 0xF8);

	// Fill with palette entry 0. Some NCGR cells might be empty, or smaller
	for (uint32_t i = 0; i < (imageWidth * imageHeight); i++) {
		data[i * 4 + 0] = ctx.pal[0];
		data[i * 4 + 1] = ctx.pal[1];
		data[i * 4 + 2] = ctx.pal[2];
		data[i * 4 + 3] = is0Transp ? 0x00 : 0xFF;
	}

	/* The actual image data is stored in a "tiled" fashion, so we need to unswizzle
	 * this manually. Moreover, we ourselves stitch together several NCGR files into
	 * one image. */

	const uint32_t tileWidth  = 8;
	const uint32_t tileHeight = 8;

	for (std::vector<NCGRFile>::iterator n = ctx.ncgrs.begin(); n != ctx.ncgrs.end(); ++n) {
		if (!n->image)
			continue;

		n->image->seek(0);

		// Position of this NCGR within the big image
		const uint32_t imagePos = n->offsetX + n->offsetY * imageWidth;

		// Number of "tiles" in this image's rows/columns
		const uint32_t tilesX = n->width  / tileWidth;
		const uint32_t tilesY = n->height / tileHeight;

		// Go over all tiles
		for (uint32_t yT = 0; yT < tilesY; yT++) {
			for (uint32_t xT = 0; xT < tilesX; xT++) {

				// Position of the tile within the NCGR
				const uint32_t tilePos = xT * tileWidth + yT * tileHeight * imageWidth;

				// Go over all pixels in the tile
				for (uint32_t y = 0; y < tileHeight; y++) {
					for (uint32_t x = 0; x < tileWidth; x++) {

						// Position of the pixel within the tile
						const uint32_t pos   = imagePos + tilePos + x + y * imageWidth;
						const uint8_t  pixel = n->image->readByte();

						if (pos > (imageWidth * imageHeight))
							continue;

						data[pos * 4 + 0] = ctx.pal[pixel * 3 + 0];
						data[pos * 4 + 1] = ctx.pal[pixel * 3 + 1];
						data[pos * 4 + 2] = ctx.pal[pixel * 3 + 2];
						data[pos * 4 + 3] = ((pixel == 0) && is0Transp) ? 0x00 : 0xFF;

					}
				}

			}
		}

	}
}

} // End of namespace Graphics
