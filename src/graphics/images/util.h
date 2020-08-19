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
 *  Image related utility functions.
 */

#ifndef GRAPHICS_IMAGES_UTIL_H
#define GRAPHICS_IMAGES_UTIL_H

#include <cassert>
#include <cstring>

#include <memory>

#include "src/common/types.h"
#include "src/common/util.h"
#include "src/common/maths.h"
#include "src/common/error.h"

#include "src/graphics/types.h"

namespace Graphics {

/** Return the number of bytes necessary to hold an image of these dimensions
  * and in this format. */
static inline uint32_t getDataSize(PixelFormatRaw format, int32_t width, int32_t height) {
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
			return MAX<uint32_t>( 8, ((width + 3) / 4) * ((height + 3) / 4) *  8);

		case kPixelFormatDXT3:
		case kPixelFormatDXT5:
			return MAX<uint32_t>(16, ((width + 3) / 4) * ((height + 3) / 4) * 16);

		default:
			break;
	}

	throw Common::Exception("Invalid pixel format %u", (uint) format);
}

/** Are these image dimensions valid for this format? */
static inline bool hasValidDimensions(PixelFormatRaw format, int32_t width, int32_t height) {
	if ((width < 0) || (width >= 0x8000) || (height < 0) || (height >= 0x8000))
		return false;

	switch (format) {
		case kPixelFormatRGB8:
		case kPixelFormatRGBA8:
		case kPixelFormatRGB5A1:
		case kPixelFormatRGB5:
		case kPixelFormatDXT1:
		case kPixelFormatDXT3:
		case kPixelFormatDXT5:
			return true;

		default:
			break;
	}

	return false;
}

/** Flip an image horizontally. */
static inline void flipHorizontally(byte *data, int width, int height, int bpp) {
	if ((width <= 0) || (height <= 0) || (bpp <= 0))
		return;

	const size_t halfWidth = width / 2;
	const size_t pitch     = bpp * width;

	std::unique_ptr<byte[]> buffer = std::make_unique<byte[]>(bpp);

	while (height-- > 0) {
		byte *dataStart = data;
		byte *dataEnd   = data + pitch - bpp;

		for (size_t j = 0; j < halfWidth; j++) {
			memcpy(buffer.get(), dataStart   , bpp);
			memcpy(dataStart   , dataEnd     , bpp);
			memcpy(dataEnd     , buffer.get(), bpp);

			dataStart += bpp;
			dataEnd   -= bpp;
		}

		data += pitch;
	}
}

/** Flip an image vertically. */
static inline void flipVertically(byte *data, int width, int height, int bpp) {
	if ((width <= 0) || (height <= 0) || (bpp <= 0))
		return;

	const size_t pitch = bpp * width;

	byte *dataStart = data;
	byte *dataEnd   = data + (pitch * height) - pitch;

	std::unique_ptr<byte[]> buffer = std::make_unique<byte[]>(pitch);

	size_t halfHeight = height / 2;
	while (halfHeight--) {
		memcpy(buffer.get(), dataStart   , pitch);
		memcpy(dataStart   , dataEnd     , pitch);
		memcpy(dataEnd     , buffer.get(), pitch);

		dataStart += pitch;
		dataEnd   -= pitch;
	}
}

/** Rotate a square image in 90° steps, clock-wise. */
static inline void rotate90(byte *data, int width, int height, int bpp, int steps) {
	if ((width <= 0) || (height <= 0) || (bpp <= 0))
		return;

	assert(width == height);

	while (steps-- > 0) {
		const size_t n = width;

		const size_t w =  n      / 2;
		const size_t h = (n + 1) / 2;

		for (size_t x = 0; x < w; x++) {
			for (size_t y = 0; y < h; y++) {
				const size_t d0 = ( y          * n +  x         ) * bpp;
				const size_t d1 = ((n - 1 - x) * n +  y         ) * bpp;
				const size_t d2 = ((n - 1 - y) * n + (n - 1 - x)) * bpp;
				const size_t d3 = ( x          * n + (n - 1 - y)) * bpp;

				for (size_t p = 0; p < (size_t) bpp; p++) {
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
static inline uint32_t deSwizzleOffset(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
	width  = Common::intLog2(width);
	height = Common::intLog2(height);

	uint32_t offset     = 0;
	uint32_t shiftCount = 0;

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

#endif // GRAPHICS_IMAGES_UTIL_H
