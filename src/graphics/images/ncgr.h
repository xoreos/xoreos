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

#ifndef GRAPHICS_IMAGES_NCGR_H
#define GRAPHICS_IMAGES_NCGR_H

#include <vector>
#include <memory>

#include "src/aurora/nitrofile.h"

#include "src/graphics/images/decoder.h"

namespace Graphics {

/** Image decoder for Nintendo's tile-based graphics format.
 *
 *  2D graphics on the Nintendo DS is fundamentally tile-based.
 *  Specifically, the graphics data in NCGR is ordered in tiles of 8x8
 *  pixels. We unwrap them, so this is transparent to the user of the
 *  NCGR class.
 *
 *  Moreover, an NCGR file can itself be a tile of a larger image.
 *  By specifying a vector of NCGR streams and the dimensions of the
 *  grid they span (in NCGR files, not pixels), the NCGR class can
 *  automatically stitch them together into a single big image.
 *
 *  If a pointer in the NCGR stream vector is 0, the cell in the grid
 *  will stay empty, filled with entry 0 of the palette.
 *
 *  Please note that, in accordance with other image decoder classes,
 *  NCGR will never take over any of the streams. The caller has to
 *  manage stream deletion itself.
 */
class NCGR : public ImageDecoder, public Aurora::NitroFile {
public:
	NCGR(Common::SeekableReadStream &ncgr, Common::SeekableReadStream &nclr);
	NCGR(const std::vector<Common::SeekableReadStream *> &ncgrs, uint32_t width, uint32_t height,
	     Common::SeekableReadStream &nclr);
	~NCGR();

private:
	struct NCGRFile {
		Common::SeekableSubReadStreamEndian *ncgr;
		Common::SeekableReadStream *image;

		/** Offset to the CHAR section within the NCGR file. */
		uint32_t offsetCHAR;

		uint32_t width;  ///< Width in pixels of this NCGR.
		uint32_t height; ///< Height in pixels of this NCGR.
		uint8_t  depth;  ///< Color depth in bits.

		uint32_t offsetX; ///< X offset in pixels into the final image.
		uint32_t offsetY; ///< Y offset in pixels into the final image.

		NCGRFile();
		~NCGRFile();
	};

	struct ReadContext {
		uint32_t width;  ///< Width of the NCGR grid, in NCGR.
		uint32_t height; ///< Height of the NCGR grid, in NCGR.

		std::unique_ptr<const byte[]> pal;

		std::vector<NCGRFile> ncgrs;
	};


	void load(const std::vector<Common::SeekableReadStream *> &ncgrs, uint32_t width, uint32_t height,
	          Common::SeekableReadStream &nclr);

	void load      (NCGRFile &ctx);
	void readHeader(NCGRFile &ctx);
	void readChar  (NCGRFile &ctx);

	void calculateGrid(ReadContext &ctx, uint32_t &imageWidth, uint32_t &imageHeight);
	void draw(ReadContext &ctx);
};

} // End of namespace Graphics

#endif // GRAPHICS_IMAGES_NCGR_H
