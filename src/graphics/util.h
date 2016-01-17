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
 *  Graphics related utility functions.
 */

#ifndef GRAPHICS_UTIL_H
#define GRAPHICS_UTIL_H

#include <cassert>
#include <cstring>

#include "src/common/types.h"
#include "src/common/util.h"
#include "src/common/maths.h"
#include "src/common/error.h"

#include "src/graphics/types.h"

namespace Graphics {

/** Return the number of bytes necessary to hold an image of these dimensions
  * and in this format. */
static inline uint32 getDataSize(PixelFormatRaw format, int32 width, int32 height) {
	if ((width < 0) || (width >= 0x8000) || (height < 0) || (height >= 0x8000))
		throw Common::Exception("Invalid dimensions %dx%d", width, height);

	switch (format) {
		case kPixelFormatRGB8:
			return width * height * 3;

		case kPixelFormatRGBA8:
			return width * height * 4;

		case kPixelFormatRGB5A1:
		case kPixelFormatRGB5:
			return width * height * 2;

		case kPixelFormatDXT1:
			return MAX<uint32>( 8, ((width + 3) / 4) * ((height + 3) / 4) *  8);

		case kPixelFormatDXT3:
		case kPixelFormatDXT5:
			return MAX<uint32>(16, ((width + 3) / 4) * ((height + 3) / 4) * 16);

		default:
			break;
	}

	throw Common::Exception("Invalid pixel format %u", (uint) format);
}

/** Are these image dimensions valid for this format? */
static inline bool hasValidDimensions(PixelFormatRaw format, int32 width, int32 height) {
	if ((width < 0) || (height < 0))
		return false;

	switch (format) {
		case kPixelFormatRGB8:
		case kPixelFormatRGBA8:
		case kPixelFormatRGB5A1:
		case kPixelFormatRGB5:
			return true;

		case kPixelFormatDXT1:
		case kPixelFormatDXT3:
		case kPixelFormatDXT5:
			/* The DXT algorithms work on 4x4 pixel blocks. Textures smaller than one
			 * block will be padded, but larger textures need to be correctly aligned. */
			return ((width < 4) && (height < 4)) || (((width % 4) == 0) && ((height % 4) == 0));

		default:
			break;
	}

	return false;
}

/** Flip an image horizontally. */
static inline void flipHorizontally(byte *data, int width, int height, int bpp) {
	if ((width <= 0) || (height <= 0) || (bpp <= 0))
		return;

	int halfWidth = width / 2;
	int pitch     = bpp * width;

	byte *buffer = new byte[bpp];

	while (height-- > 0) {
		byte *dataStart = data;
		byte *dataEnd   = data + pitch - bpp;

		for (int j = 0; j < halfWidth; j++) {
			memcpy(buffer   , dataStart, bpp);
			memcpy(dataStart, dataEnd  , bpp);
			memcpy(dataEnd  , buffer   , bpp);

			dataStart += bpp;
			dataEnd   -= bpp;
		}

		data += pitch;
	}

	delete[] buffer;
}

/** Flip an image vertically. */
static inline void flipVertically(byte *data, int width, int height, int bpp) {
	if ((width <= 0) || (height <= 0) || (bpp <= 0))
		return;

	int halfHeight = height / 2;
	int pitch      = bpp * width;

	byte *dataStart = data;
	byte *dataEnd   = data + (pitch * height) - pitch;

	byte *buffer = new byte[pitch];

	while (halfHeight--) {
		memcpy(buffer   , dataStart, pitch);
		memcpy(dataStart, dataEnd  , pitch);
		memcpy(dataEnd  , buffer   , pitch);

		dataStart += pitch;
		dataEnd   -= pitch;
	}

	delete[] buffer;
}

/** Rotate a square image in 90° steps. */
static inline void rotate90(byte *data, int width, int height, int bpp, int steps) {
	if ((width <= 0) || (height <= 0) || (bpp <= 0))
		return;

	assert(width == height);

	while (steps-- > 0) {
		const int n = width;

		const int w =  n      / 2;
		const int h = (n + 1) / 2;

		for (int x = 0; x < w; x++) {
			for (int y = 0; y < h; y++) {
				const int d0 = ( y          * n +  x         ) * bpp;
				const int d1 = ((n - 1 - x) * n +  y         ) * bpp;
				const int d2 = ((n - 1 - y) * n + (n - 1 - x)) * bpp;
				const int d3 = ( x          * n + (n - 1 - y)) * bpp;

				for (int p = 0; p < bpp; p++) {
					const byte tmp = data[d0 + p];

					data[d0 + p] = data[d1 + p];
					data[d1 + p] = data[d2 + p];
					data[d2 + p] = data[d3 + p];
					data[d3 + p] = tmp;
				}
			}
		}

	}
}

/** De-"swizzle" a texture pixel offset. */
static inline uint32 deSwizzleOffset(uint32 x, uint32 y, uint32 width, uint32 height) {
	width  = Common::intLog2(width);
	height = Common::intLog2(height);

	uint32 offset     = 0;
	uint32 shiftCount = 0;

	while (width | height) {
		if (width) {
			offset |= (x & 0x01) << shiftCount;

			x >>= 1;

			shiftCount++;
			width--;
		}

		if (height) {
			offset |= (y & 0x01) << shiftCount;

			y >>= 1;

			shiftCount++;
			height--;
		}
	}

	return offset;
}

} // End of namespace Graphics

#endif // GRAPHICS_UTIL_H
