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
 *  Unit tests for our utility templates and functions.
 */

#include "gtest/gtest.h"

#include "src/common/util.h"

GTEST_TEST(Util, ABS) {
	EXPECT_EQ(ABS<int>( 23), 23);
	EXPECT_EQ(ABS<int>(-23), 23);
}

GTEST_TEST(Util, MIN) {
	EXPECT_EQ(MIN<int>(-23, -5), -23);
	EXPECT_EQ(MIN<int>( 23,  5),   5);
	EXPECT_EQ(MIN<int>(-23, 23), -23);
}

GTEST_TEST(Util, MAX) {
	EXPECT_EQ(MAX<int>(-23, -5), - 5);
	EXPECT_EQ(MAX<int>( 23,  5),  23);
	EXPECT_EQ(MAX<int>(-23, 23),  23);
}

GTEST_TEST(Util, CLIP) {
	EXPECT_EQ(CLIP<int>( 23, -5, 5),  5);
	EXPECT_EQ(CLIP<int>(-23, -5, 5), -5);
	EXPECT_EQ(CLIP<int>(- 1, -5, 5), -1);
	EXPECT_EQ(CLIP<int>(  1, -5, 5),  1);
}

GTEST_TEST(Util, ABSFloat) {
	EXPECT_EQ(ABS<float>( 23.0f), 23.0f);
	EXPECT_EQ(ABS<float>(-23.0f), 23.0f);
}

GTEST_TEST(Util, MINFloat) {
	EXPECT_EQ(MIN<float>(-23.0f, -5.0f), -23.0f);
	EXPECT_EQ(MIN<float>( 23.0f,  5.0f),   5.0f);
	EXPECT_EQ(MIN<float>(-23.0f, 23.0f), -23.0f);
}

GTEST_TEST(Util, MAXFloat) {
	EXPECT_EQ(MAX<float>(-23.0f, -5.0f), - 5.0f);
	EXPECT_EQ(MAX<float>( 23.0f,  5.0f),  23.0f);
	EXPECT_EQ(MAX<float>(-23.0f, 23.0f),  23.0f);
}

GTEST_TEST(Util, ABSDouble) {
	EXPECT_EQ(ABS<double>( 23.0), 23.0);
	EXPECT_EQ(ABS<double>(-23.0), 23.0);
}

GTEST_TEST(Util, MINDouble) {
	EXPECT_EQ(MIN<double>(-23.0, -5.0), -23.0);
	EXPECT_EQ(MIN<double>( 23.0,  5.0),   5.0);
	EXPECT_EQ(MIN<double>(-23.0, 23.0), -23.0);
}

GTEST_TEST(Util, MAXDouble) {
	EXPECT_EQ(MAX<double>(-23.0, -5.0), - 5.0);
	EXPECT_EQ(MAX<double>( 23.0,  5.0),  23.0);
	EXPECT_EQ(MAX<double>(-23.0, 23.0),  23.0);
}

GTEST_TEST(Util, ISPOWER2) {
	EXPECT_TRUE(ISPOWER2( 2));
	EXPECT_TRUE(ISPOWER2( 4));
	EXPECT_TRUE(ISPOWER2( 8));

	EXPECT_FALSE(ISPOWER2(-2));
	EXPECT_FALSE(ISPOWER2(-4));
	EXPECT_FALSE(ISPOWER2(-8));

	EXPECT_FALSE(ISPOWER2( 0));
	EXPECT_FALSE(ISPOWER2( 3));
	EXPECT_FALSE(ISPOWER2( 5));
	EXPECT_FALSE(ISPOWER2(-3));
	EXPECT_FALSE(ISPOWER2(-5));
}

GTEST_TEST(Util, NEXTPOWER2) {
	EXPECT_EQ(NEXTPOWER2((uint32_t)  3),  4);
	EXPECT_EQ(NEXTPOWER2((uint32_t)  5),  8);
	EXPECT_EQ(NEXTPOWER2((uint32_t) 10), 16);

	EXPECT_EQ(NEXTPOWER2((uint32_t)  0),  1);

	EXPECT_EQ(NEXTPOWER2((uint32_t)  2),  2);
	EXPECT_EQ(NEXTPOWER2((uint32_t)  4),  4);
	EXPECT_EQ(NEXTPOWER2((uint32_t)  8),  8);
	EXPECT_EQ(NEXTPOWER2((uint32_t) 16), 16);
}

GTEST_TEST(Util, ARRAYSIZE) {
	static const uint32_t kArray[5] = { 0 };

	EXPECT_EQ(ARRAYSIZE(kArray), 5);
}

GTEST_TEST(Util, convertIEEEFloatToFloat) {
	EXPECT_FLOAT_EQ(convertIEEEFloat((uint32_t) 0x00000000),   0.00f);
	EXPECT_FLOAT_EQ(convertIEEEFloat((uint32_t) 0x3F800000),   1.00f);
	EXPECT_FLOAT_EQ(convertIEEEFloat((uint32_t) 0xBF800000), - 1.00f);
	EXPECT_FLOAT_EQ(convertIEEEFloat((uint32_t) 0x41BC0000),  23.50f);
	EXPECT_FLOAT_EQ(convertIEEEFloat((uint32_t) 0x40A75C29),   5.23f);
}

GTEST_TEST(Util, convertIEEEDoubleToDouble) {
	EXPECT_DOUBLE_EQ(convertIEEEDouble(UINT64_C(0x0000000000000000)),   0.00);
	EXPECT_DOUBLE_EQ(convertIEEEDouble(UINT64_C(0x3FF0000000000000)),   1.00);
	EXPECT_DOUBLE_EQ(convertIEEEDouble(UINT64_C(0xBFF0000000000000)), - 1.00);
	EXPECT_DOUBLE_EQ(convertIEEEDouble(UINT64_C(0x4037800000000000)),  23.50);
	EXPECT_DOUBLE_EQ(convertIEEEDouble(UINT64_C(0x4014EB851EB851EC)),   5.23);
}

GTEST_TEST(Util, convertIEEEFloatFromFloat) {
	EXPECT_EQ(convertIEEEFloat(  0.00f), (uint32_t) 0x00000000);
	EXPECT_EQ(convertIEEEFloat(  1.00f), (uint32_t) 0x3F800000);
	EXPECT_EQ(convertIEEEFloat(- 1.00f), (uint32_t) 0xBF800000);
	EXPECT_EQ(convertIEEEFloat( 23.50f), (uint32_t) 0x41BC0000);
	EXPECT_EQ(convertIEEEFloat(  5.23f), (uint32_t) 0x40A75C29);
}

GTEST_TEST(Util, convertIEEEDoubleFromDouble) {
	EXPECT_EQ(convertIEEEDouble(  0.00), UINT64_C(0x0000000000000000));
	EXPECT_EQ(convertIEEEDouble(  1.00), UINT64_C(0x3FF0000000000000));
	EXPECT_EQ(convertIEEEDouble(- 1.00), UINT64_C(0xBFF0000000000000));
	EXPECT_EQ(convertIEEEDouble( 23.50), UINT64_C(0x4037800000000000));
	EXPECT_EQ(convertIEEEDouble(  5.23), UINT64_C(0x4014EB851EB851EC));
}

GTEST_TEST(Util, readIEEEFloat16) {
	EXPECT_FLOAT_EQ(readIEEEFloat16(0x0000),   0.00f);
	EXPECT_FLOAT_EQ(readIEEEFloat16(0x3C00),   1.00f);
	EXPECT_FLOAT_EQ(readIEEEFloat16(0xBC00), - 1.00f);
	EXPECT_FLOAT_EQ(readIEEEFloat16(0x4DE0),  23.50f);

	// 16-bit half-precision floats can't represent 5.23 more accurately
	EXPECT_NEAR(readIEEEFloat16(0x453B), 5.23f, 0.0005);
}

GTEST_TEST(Util, readNintendoFixedPoint) {
	EXPECT_DOUBLE_EQ(readNintendoFixedPoint(0x00000000,  true, 15, 16),      0.00);
	EXPECT_DOUBLE_EQ(readNintendoFixedPoint(0x00010000,  true, 15, 16),      1.00);
	EXPECT_DOUBLE_EQ(readNintendoFixedPoint(0xFFFF0000,  true, 15, 16), -    1.00);
	EXPECT_DOUBLE_EQ(readNintendoFixedPoint(0xFFFF0000, false, 16, 16),  65535.00);

	/* The fractional part in Nintendo's fixed point format is expressed in simple
	 * fractions of 2^fBits. That's pretty different from IEEE floats, so their
	 * representation of non-trivial fractions diverge quite a bit. Which means
	 * checking that the result is within a few double ULP doesn't make much sense.
	 */
	EXPECT_NEAR(readNintendoFixedPoint(0x00053AE2, true, 15, 16), 5.23, 0.00005);
}
