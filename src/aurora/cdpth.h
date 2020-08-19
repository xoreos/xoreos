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

#ifndef AURORA_CDPTH_H
#define AURORA_CDPTH_H

#include "src/common/types.h"

namespace Common {
	class SeekableReadStream;
}

namespace Aurora {

/** Loader for CDPTH, BioWare's Compressed DePTH, into a linear array
 *  of uint16_t values.
 *
 *  CDPTH are found in Sonic, where they are used to indicate the
 *  depth information of each pixel of the area background images
 *  (which are in CBGT format).
 *
 *  Layout-wise, a CDPTH is stored similar to CBGT: cells of 64x64
 *  pixels, compressed using Nintendo's 0x10 LZSS algorithm. Unlike
 *  CBGT, though, the cells themselves are *not* swizzled into 8x8
 *  tiles, and the pixel value in CDPTH is a 16bit integer specifying
 *  a depth.
 *
 *  The width and height of the image is not stored within the CDPTH
 *  file, and have to be provided from the outside. And because of
 *  the cell nature of the data, we do need to know these dimensions
 *  during loading.
 */
class CDPTH {
public:
	/** Read a CDPTH out of this stream. */
	static const uint16_t *load(Common::SeekableReadStream &cdpth, uint32_t width, uint32_t height);
	/** Read a CDPTH out of this stream and delete it afterwards. */
	static const uint16_t *load(Common::SeekableReadStream *cdpth, uint32_t width, uint32_t height);
};

} // End of namespace Aurora

#endif // AURORA_CDPTH_H
