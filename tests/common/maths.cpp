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
 *  Unit tests for our mathematical helpers.
 */

#include "gtest/gtest.h"

#include "src/common/maths.h"

GTEST_TEST(Maths, intLog2) {
	EXPECT_EQ(Common::intLog2( 2),  1);
	EXPECT_EQ(Common::intLog2( 4),  2);
	EXPECT_EQ(Common::intLog2( 8),  3);
	EXPECT_EQ(Common::intLog2(16),  4);
	EXPECT_EQ(Common::intLog2(32),  5);

	EXPECT_EQ(Common::intLog2( 0), -1);

	EXPECT_EQ(Common::intLog2( 3),  1);
	EXPECT_EQ(Common::intLog2( 5),  2);
	EXPECT_EQ(Common::intLog2(10),  3);
}

GTEST_TEST(Maths, pi) {
	EXPECT_DOUBLE_EQ(M_PI, 3.14159265358979323846);
}

GTEST_TEST(Maths, deg2rad) {
	EXPECT_FLOAT_EQ(Common::deg2rad(   0.0f),         0.0f);
	EXPECT_FLOAT_EQ(Common::deg2rad( 180.0f),  M_PI       );
	EXPECT_FLOAT_EQ(Common::deg2rad(-180.0f), -M_PI       );
	EXPECT_FLOAT_EQ(Common::deg2rad(  90.0f),  M_PI / 2.0f);
	EXPECT_FLOAT_EQ(Common::deg2rad(- 90.0f), -M_PI / 2.0f);
	EXPECT_FLOAT_EQ(Common::deg2rad( 360.0f),  M_PI * 2.0f);
	EXPECT_FLOAT_EQ(Common::deg2rad(-360.0f), -M_PI * 2.0f);
}

GTEST_TEST(Maths, rad2deg) {
	EXPECT_FLOAT_EQ(Common::rad2deg(        0.0f),    0.0f);
	EXPECT_FLOAT_EQ(Common::rad2deg( M_PI       ),  180.0f);
	EXPECT_FLOAT_EQ(Common::rad2deg(-M_PI       ), -180.0f);
	EXPECT_FLOAT_EQ(Common::rad2deg( M_PI / 2.0f),   90.0f);
	EXPECT_FLOAT_EQ(Common::rad2deg(-M_PI / 2.0f), - 90.0f);
	EXPECT_FLOAT_EQ(Common::rad2deg( M_PI * 2.0f),  360.0f);
	EXPECT_FLOAT_EQ(Common::rad2deg(-M_PI * 2.0f), -360.0f);
}
