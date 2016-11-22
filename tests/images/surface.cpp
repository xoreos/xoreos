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
 *  Unit tests for our Surface class.
 */

#include "gtest/gtest.h"

#include "src/common/error.h"

#include "src/graphics/images/surface.h"

void compareData(const byte *data1, byte data2, size_t n, size_t t = 0) {
	for (size_t i = 0; i < n; i++)
		EXPECT_EQ(data1[i], data2) << "At case " << t << ", index " << i;
}

GTEST_TEST(Surface, getWidth) {
	const Graphics::Surface surface(4, 4);

	EXPECT_EQ(surface.getWidth(), 4);
}

GTEST_TEST(Surface, getHeight) {
	const Graphics::Surface surface(4, 4);

	EXPECT_EQ(surface.getHeight(), 4);
}

GTEST_TEST(Surface, getData) {
	Graphics::Surface surface(4, 4);

	EXPECT_NE(surface.getData(), static_cast<byte *>(0));
	EXPECT_NE(const_cast<const Graphics::Surface &>(surface).getData(), static_cast<const byte *>(0));
}

GTEST_TEST(Surface, fill) {
	Graphics::Surface surface(4, 4);

	surface.fill(0xAA, 0xAA, 0xAA, 0xAA);

	const byte *data = surface.getData();
	ASSERT_NE(data, static_cast<const byte *>(0));

	compareData(data, 0xAA, 4 * 4 * 4);
}

GTEST_TEST(Surface, getMipMap) {
	const Graphics::Surface surface(4, 4);

	const Graphics::Surface::MipMap &mipMap = surface.getMipMap();

	EXPECT_EQ(mipMap.width , 4);
	EXPECT_EQ(mipMap.height, 4);
	EXPECT_EQ(mipMap.size  , 4 * 4 * 4);

	EXPECT_TRUE(mipMap.data);

	EXPECT_EQ(mipMap.image, &surface);
}

GTEST_TEST(Surface, isCompressed) {
	const Graphics::Surface surface(4, 4);

	EXPECT_FALSE(surface.isCompressed());
}

GTEST_TEST(Surface, hasAlpha) {
	const Graphics::Surface surface(4, 4);

	EXPECT_TRUE(surface.hasAlpha());
}

GTEST_TEST(Surface, getFormat) {
	const Graphics::Surface surface(4, 4);

	EXPECT_EQ(surface.getFormat()   , Graphics::kPixelFormatBGRA);
	EXPECT_EQ(surface.getFormatRaw(), Graphics::kPixelFormatRGBA8);
	EXPECT_EQ(surface.getDataType() , Graphics::kPixelDataType8);
}

GTEST_TEST(Surface, getMipMapCount) {
	const Graphics::Surface surface(4, 4);

	EXPECT_EQ(surface.getMipMapCount(), 1);
}

GTEST_TEST(Surface, getLayerCount) {
	const Graphics::Surface surface(4, 4);

	EXPECT_EQ(surface.getLayerCount(), 1);
}

GTEST_TEST(Surface, isCubeMap) {
	const Graphics::Surface surface(4, 4);

	EXPECT_FALSE(surface.isCubeMap());
}
