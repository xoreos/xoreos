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
 *  Unit tests for our own intermediate texture format.
 */

#include "gtest/gtest.h"

#include "src/common/error.h"
#include "src/common/memreadstream.h"

#include "src/graphics/images/xoreositex.h"

void expectData(const byte *data, size_t n, size_t t = 0) {
	for (size_t i = 0; i < n; i++)
		EXPECT_EQ(data[i], i) << "At case " << t << ", index " << i;
}

// --- 3 bytes per pixel ---

static const byte kXEOSITEX_3[] = {
	0x58,0x45,0x4F,0x53,0x49,0x54,0x45,0x58,0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x04,0x00,
	0x00,0x00,0x30,0x00,0x00,0x00,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,
	0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,
	0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,
	0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,0x02,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x0C,0x00,
	0x00,0x00,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x01,0x00,
	0x00,0x00,0x01,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x00,0x01,0x02
};

GTEST_TEST(XEOSITEX_3, isCompressed) {
	Common::MemoryReadStream stream(kXEOSITEX_3);
	const Graphics::XEOSITEX image(stream);

	EXPECT_FALSE(image.isCompressed());
}

GTEST_TEST(XEOSITEX_3, hasAlpha) {
	Common::MemoryReadStream stream(kXEOSITEX_3);
	const Graphics::XEOSITEX image(stream);

	EXPECT_FALSE(image.hasAlpha());
}

GTEST_TEST(XEOSITEX_3, getFormat) {
	Common::MemoryReadStream stream(kXEOSITEX_3);
	const Graphics::XEOSITEX image(stream);

	EXPECT_EQ(image.getFormat()   , Graphics::kPixelFormatBGR);
	EXPECT_EQ(image.getFormatRaw(), Graphics::kPixelFormatRGB8);
	EXPECT_EQ(image.getDataType() , Graphics::kPixelDataType8);
}

GTEST_TEST(XEOSITEX_3, getMipMapCount) {
	Common::MemoryReadStream stream(kXEOSITEX_3);
	const Graphics::XEOSITEX image(stream);

	EXPECT_EQ(image.getMipMapCount(), 3);
}

GTEST_TEST(XEOSITEX_3, getLayerCount) {
	Common::MemoryReadStream stream(kXEOSITEX_3);
	const Graphics::XEOSITEX image(stream);

	EXPECT_EQ(image.getLayerCount(), 1);
}

GTEST_TEST(XEOSITEX_3, isCubeMap) {
	Common::MemoryReadStream stream(kXEOSITEX_3);
	const Graphics::XEOSITEX image(stream);

	EXPECT_FALSE(image.isCubeMap());
}

GTEST_TEST(XEOSITEX_3, getMipMap) {
	Common::MemoryReadStream stream(kXEOSITEX_3);
	const Graphics::XEOSITEX image(stream);

	{
		const Graphics::XEOSITEX::MipMap &mipMap = image.getMipMap(0);

		EXPECT_EQ(mipMap.width , 4);
		EXPECT_EQ(mipMap.height, 4);
		EXPECT_EQ(mipMap.size  , 4 * 4 * 3);

		EXPECT_TRUE(mipMap.data);

		EXPECT_EQ(mipMap.image, &image);

		expectData(mipMap.data.get(), 4 * 4 * 3, 0);
	}

	{
		const Graphics::XEOSITEX::MipMap &mipMap = image.getMipMap(1);

		EXPECT_EQ(mipMap.width , 2);
		EXPECT_EQ(mipMap.height, 2);
		EXPECT_EQ(mipMap.size  , 2 * 2 * 3);

		EXPECT_TRUE(mipMap.data);

		EXPECT_EQ(mipMap.image, &image);

		expectData(mipMap.data.get(), 2 * 2 * 3, 1);
	}

	{
		const Graphics::XEOSITEX::MipMap &mipMap = image.getMipMap(2);

		EXPECT_EQ(mipMap.width , 1);
		EXPECT_EQ(mipMap.height, 1);
		EXPECT_EQ(mipMap.size  , 1 * 1 * 3);

		EXPECT_TRUE(mipMap.data);

		EXPECT_EQ(mipMap.image, &image);

		expectData(mipMap.data.get(), 1 * 1 * 3, 2);
	}
}

// --- 4 bytes per pixel ---

static const byte kXEOSITEX_4[] = {
	0x58,0x45,0x4F,0x53,0x49,0x54,0x45,0x58,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x04,0x00,
	0x00,0x00,0x40,0x00,0x00,0x00,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,
	0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,
	0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,
	0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,
	0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,0x02,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x10,0x00,
	0x00,0x00,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,
	0x0E,0x0F,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x01,
	0x02,0x03
};


GTEST_TEST(XEOSITEX_4, isCompressed) {
	Common::MemoryReadStream stream(kXEOSITEX_4);
	const Graphics::XEOSITEX image(stream);

	EXPECT_FALSE(image.isCompressed());
}

GTEST_TEST(XEOSITEX_4, hasAlpha) {
	Common::MemoryReadStream stream(kXEOSITEX_4);
	const Graphics::XEOSITEX image(stream);

	EXPECT_TRUE(image.hasAlpha());
}

GTEST_TEST(XEOSITEX_4, getFormat) {
	Common::MemoryReadStream stream(kXEOSITEX_4);
	const Graphics::XEOSITEX image(stream);

	EXPECT_EQ(image.getFormat()   , Graphics::kPixelFormatBGRA);
	EXPECT_EQ(image.getFormatRaw(), Graphics::kPixelFormatRGBA8);
	EXPECT_EQ(image.getDataType() , Graphics::kPixelDataType8);
}

GTEST_TEST(XEOSITEX_4, getMipMapCount) {
	Common::MemoryReadStream stream(kXEOSITEX_4);
	const Graphics::XEOSITEX image(stream);

	EXPECT_EQ(image.getMipMapCount(), 3);
}

GTEST_TEST(XEOSITEX_4, getLayerCount) {
	Common::MemoryReadStream stream(kXEOSITEX_4);
	const Graphics::XEOSITEX image(stream);

	EXPECT_EQ(image.getLayerCount(), 1);
}

GTEST_TEST(XEOSITEX_4, isCubeMap) {
	Common::MemoryReadStream stream(kXEOSITEX_4);
	const Graphics::XEOSITEX image(stream);

	EXPECT_FALSE(image.isCubeMap());
}

GTEST_TEST(XEOSITEX_4, getMipMap) {
	Common::MemoryReadStream stream(kXEOSITEX_4);
	const Graphics::XEOSITEX image(stream);

	{
		const Graphics::XEOSITEX::MipMap &mipMap = image.getMipMap(0);

		EXPECT_EQ(mipMap.width , 4);
		EXPECT_EQ(mipMap.height, 4);
		EXPECT_EQ(mipMap.size  , 4 * 4 * 4);

		EXPECT_TRUE(mipMap.data);

		EXPECT_EQ(mipMap.image, &image);

		expectData(mipMap.data.get(), 4 * 4 * 4, 0);
	}

	{
		const Graphics::XEOSITEX::MipMap &mipMap = image.getMipMap(1);

		EXPECT_EQ(mipMap.width , 2);
		EXPECT_EQ(mipMap.height, 2);
		EXPECT_EQ(mipMap.size  , 2 * 2 * 4);

		EXPECT_TRUE(mipMap.data);

		EXPECT_EQ(mipMap.image, &image);

		expectData(mipMap.data.get(), 2 * 2 * 4, 1);
	}

	{
		const Graphics::XEOSITEX::MipMap &mipMap = image.getMipMap(2);

		EXPECT_EQ(mipMap.width , 1);
		EXPECT_EQ(mipMap.height, 1);
		EXPECT_EQ(mipMap.size  , 1 * 1 * 4);

		EXPECT_TRUE(mipMap.data);

		EXPECT_EQ(mipMap.image, &image);

		expectData(mipMap.data.get(), 1 * 1 * 4, 2);
	}
}

// --- Variations ---

GTEST_TEST(XEOSITEX, broken) {
	Common::MemoryReadStream stream(kXEOSITEX_4, sizeof(kXEOSITEX_4) / 2);

	EXPECT_THROW(const Graphics::XEOSITEX image(stream), Common::Exception);
}
