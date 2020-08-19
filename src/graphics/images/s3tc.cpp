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
 *  Manual S3TC DXTn decompression methods.
 */

#include "src/common/util.h"
#include "src/common/readstream.h"

#include "src/graphics/images/s3tc.h"

namespace Graphics {

static inline uint32_t convert565To8888(uint16_t color) {
	return ((color & 0x1F) << 11) | ((color & 0x7E0) << 13) | ((color & 0xF800) << 16) | 0xFF;
}

static inline uint32_t interpolate32(double weight, uint32_t color_0, uint32_t color_1) {
	byte r[3], g[3], b[3], a[3];
	r[0] = color_0 >> 24;
	r[1] = color_1 >> 24;
	r[2] = (byte)((1.0f - weight) * (double)r[0] + weight * (double)r[1]);
	g[0] = (color_0 >> 16) & 0xFF;
	g[1] = (color_1 >> 16) & 0xFF;
	g[2] = (byte)((1.0f - weight) * (double)g[0] + weight * (double)g[1]);
	b[0] = (color_0 >> 8) & 0xFF;
	b[1] = (color_1 >> 8) & 0xFF;
	b[2] = (byte)((1.0f - weight) * (double)b[0] + weight * (double)b[1]);
	a[0] = color_0 & 0xFF;
	a[1] = color_1 & 0xFF;
	a[2] = (byte)((1.0f - weight) * (double)a[0] + weight * (double)a[1]);
	return r[2] << 24 | g[2] << 16 | b[2] << 8 | a[2];
}

struct DXT1Texel {
	uint16_t color_0;
	uint16_t color_1;
	uint32_t pixels;
};

#define READ_DXT1_TEXEL(x) \
	x.color_0 = src.readUint16LE(); \
	x.color_1 = src.readUint16LE(); \
	x.pixels = src.readUint32BE()

void decompressDXT1(byte *dest, Common::SeekableReadStream &src, uint32_t width, uint32_t height, uint32_t pitch) {
	for (int32_t ty = height; ty > 0; ty -= 4) {
		for (uint32_t tx = 0; tx < width; tx += 4) {
			DXT1Texel tex;
			READ_DXT1_TEXEL(tex);
			uint32_t blended[4];

			blended[0] = convert565To8888(tex.color_0);
			blended[1] = convert565To8888(tex.color_1);

			if (tex.color_0 > tex.color_1) {
				blended[2] = interpolate32(0.333333f, blended[0], blended[1]);
				blended[3] = interpolate32(0.666666f, blended[0], blended[1]);
			} else {
				blended[2] = interpolate32(0.5f, blended[0], blended[1]);
				blended[3] = 0;
			}

			uint32_t cpx = tex.pixels;
			uint32_t blockWidth = MIN<uint32_t>(width, 4);
			uint32_t blockHeight = MIN<uint32_t>(height, 4);

			for (byte y = 0; y < blockHeight; ++y) {
				for (byte x = 0; x < blockWidth; ++x) {
					const uint32_t destX = tx + x;
					const uint32_t destY = height - 1 - (ty - blockHeight + y);

					const uint32_t pixel =  blended[cpx & 3];

					cpx >>= 2;

					if ((destX < width) && (destY < height))
						WRITE_BE_UINT32(dest + destY * pitch + destX * 4, pixel);
				}
			}
		}
	}
}

struct DXT23Texel : public DXT1Texel {
	uint16_t alpha[4];
};

#define READ_DXT3_TEXEL(x) \
	x.alpha[0] = src.readUint16LE(); \
	x.alpha[1] = src.readUint16LE(); \
	x.alpha[2] = src.readUint16LE(); \
	x.alpha[3] = src.readUint16LE(); \
	READ_DXT1_TEXEL(x)

void decompressDXT3(byte *dest, Common::SeekableReadStream &src, uint32_t width, uint32_t height, uint32_t pitch) {
	for (int32_t ty = height; ty > 0; ty -= 4) {
		for (uint32_t tx = 0; tx < width; tx += 4) {
			DXT23Texel tex;
			uint32_t blended[4];
			READ_DXT3_TEXEL(tex);

			blended[0] = convert565To8888(tex.color_0) & 0xFFFFFF00;
			blended[1] = convert565To8888(tex.color_1) & 0xFFFFFF00;
			blended[2] = interpolate32(0.333333f, blended[0], blended[1]);
			blended[3] = interpolate32(0.666666f, blended[0], blended[1]);

			uint32_t cpx = tex.pixels;
			uint32_t blockWidth = MIN<uint32_t>(width, 4);
			uint32_t blockHeight = MIN<uint32_t>(height, 4);

			for (byte y = 0; y < blockHeight; ++y) {
				for (byte x = 0; x < blockWidth; ++x) {
					const uint32_t destX = tx + x;
					const uint32_t destY = height - 1 - (ty - blockHeight + y);

					const uint32_t alpha = (tex.alpha[y] >> (x * 4)) & 0xF;
					const uint32_t pixel = blended[cpx & 3] | alpha << 4;

					cpx >>= 2;

					if ((destX < width) && (destY < height))
						WRITE_BE_UINT32(dest + destY * pitch + destX * 4, pixel);
				}
			}
		}
	}
}

struct DXT45Texel : public DXT1Texel {
	byte alpha_0;
	byte alpha_1;
	uint64_t alphabl;
};

static uint64_t readUint48LE(Common::SeekableReadStream &src) {
	uint64_t output = src.readUint32LE();
	return output | ((uint64_t)src.readUint16LE() << 32);
}

#define READ_DXT5_TEXEL(x) \
	x.alpha_0 = src.readByte(); \
	x.alpha_1 = src.readByte(); \
	x.alphabl = readUint48LE(src); \
	READ_DXT1_TEXEL(x)

void decompressDXT5(byte *dest, Common::SeekableReadStream &src, uint32_t width, uint32_t height, uint32_t pitch) {
	for (int32_t ty = height; ty > 0; ty -= 4) {
		for (uint32_t tx = 0; tx < width; tx += 4) {
			uint32_t blended[4];
			byte alphab[8];
			DXT45Texel tex;
			READ_DXT5_TEXEL(tex);

			alphab[0] = tex.alpha_0;
			alphab[1] = tex.alpha_1;

			if (tex.alpha_0 > tex.alpha_1) {
				alphab[2] = (byte)((6.0f * (double)alphab[0] + 1.0f * (double)alphab[1] + 3.0f) / 7.0f);
				alphab[3] = (byte)((5.0f * (double)alphab[0] + 2.0f * (double)alphab[1] + 3.0f) / 7.0f);
				alphab[4] = (byte)((4.0f * (double)alphab[0] + 3.0f * (double)alphab[1] + 3.0f) / 7.0f);
				alphab[5] = (byte)((3.0f * (double)alphab[0] + 4.0f * (double)alphab[1] + 3.0f) / 7.0f);
				alphab[6] = (byte)((2.0f * (double)alphab[0] + 5.0f * (double)alphab[1] + 3.0f) / 7.0f);
				alphab[7] = (byte)((1.0f * (double)alphab[0] + 6.0f * (double)alphab[1] + 3.0f) / 7.0f);
			} else {
				alphab[2] = (byte)((4.0f * (double)alphab[0] + 1.0f * (double)alphab[1] + 2.0f) / 5.0f);
				alphab[3] = (byte)((3.0f * (double)alphab[0] + 2.0f * (double)alphab[1] + 2.0f) / 5.0f);
				alphab[4] = (byte)((2.0f * (double)alphab[0] + 3.0f * (double)alphab[1] + 2.0f) / 5.0f);
				alphab[5] = (byte)((1.0f * (double)alphab[0] + 4.0f * (double)alphab[1] + 2.0f) / 5.0f);
				alphab[6] = 0;
				alphab[7] = 255;
			}

			blended[0] = convert565To8888(tex.color_0) & 0xFFFFFF00;
			blended[1] = convert565To8888(tex.color_1) & 0xFFFFFF00;
			blended[2] = interpolate32(0.333333f, blended[0], blended[1]);
			blended[3] = interpolate32(0.666666f, blended[0], blended[1]);

			uint32_t cpx = tex.pixels;
			uint32_t blockWidth = MIN<uint32_t>(width, 4);
			uint32_t blockHeight = MIN<uint32_t>(height, 4);

			for (byte y = 0; y < blockHeight; ++y) {
				for (byte x = 0; x < blockWidth; ++x) {
					const uint32_t destX = tx + x;
					const uint32_t destY = height - 1 - (ty - blockHeight + y);

					const uint32_t alpha = alphab[(tex.alphabl >> (3 * (4 * (3 - y) + x))) & 7];
					const uint32_t pixel = blended[cpx & 3] | alpha;

					cpx >>= 2;

					if ((destX < width) && (destY < height))
						WRITE_BE_UINT32(dest + destY * pitch + destX * 4, pixel);
				}
			}
		}
	}
}

} // End of namespace Graphics
