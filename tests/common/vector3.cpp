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
 *  Unit tests for the Vector3 class.
 */

#include "gtest/gtest.h"

#include "src/common/vector3.h"
#include "src/common/maths.h"

GTEST_TEST(Vector3, constructorFloats) {
	const Common::Vector3 v(1.0f, 2.0f, 3.0f);

	EXPECT_EQ(v._x, 1.0f);
	EXPECT_EQ(v._y, 2.0f);
	EXPECT_EQ(v._z, 3.0f);
}

GTEST_TEST(Vector3, constructorFloatArray) {
	static const float kFloats[3] = { 1.0f, 2.0f, 3.0f };
	const Common::Vector3 v(kFloats);

	EXPECT_EQ(v._x, 1.0f);
	EXPECT_EQ(v._y, 2.0f);
	EXPECT_EQ(v._z, 3.0f);
}

GTEST_TEST(Vector3, constructorCopy) {
	const Common::Vector3 v1(1.0f, 2.0f, 3.0f);
	const Common::Vector3 v2(v1);

	EXPECT_EQ(v2._x, 1.0f);
	EXPECT_EQ(v2._y, 2.0f);
	EXPECT_EQ(v2._z, 3.0f);
}

GTEST_TEST(Vector3, operatorArrayRead) {
	const Common::Vector3 v(1.0f, 2.0f, 3.0f);

	EXPECT_EQ(v[0], 1.0f);
	EXPECT_EQ(v[1], 2.0f);
	EXPECT_EQ(v[2], 3.0f);
}

GTEST_TEST(Vector3, operatorArrayWrite) {
	Common::Vector3 v(1.0f, 2.0f, 3.0f);

	v[0] = 4.0f;
	v[1] = 5.0f;
	v[2] = 6.0f;

	EXPECT_EQ(v._x, 4.0f);
	EXPECT_EQ(v._y, 5.0f);
	EXPECT_EQ(v._z, 6.0f);
}

GTEST_TEST(Vector3, operatorAssign) {
	const Common::Vector3 v1(1.0f, 2.0f, 3.0f);
	Common::Vector3 v2(4.0f, 5.0f, 6.0f);

	v2 = v1;

	EXPECT_EQ(v2._x, 1.0f);
	EXPECT_EQ(v2._y, 2.0f);
	EXPECT_EQ(v2._z, 3.0f);
}

GTEST_TEST(Vector3, operatorAdd) {
	const Common::Vector3 v1(1.0f, 2.0f, 3.0f);
	Common::Vector3 v2(4.0f, 5.0f, 6.0f);

	v2 += v1;

	EXPECT_EQ(v2._x, 5.0f);
	EXPECT_EQ(v2._y, 7.0f);
	EXPECT_EQ(v2._z, 9.0f);
}

GTEST_TEST(Vector3, operatorSubtract) {
	const Common::Vector3 v1(1.0f, 2.0f, 3.0f);
	Common::Vector3 v2(4.0f, 5.0f, 6.0f);

	v2 -= v1;

	EXPECT_EQ(v2._x, 3.0f);
	EXPECT_EQ(v2._y, 3.0f);
	EXPECT_EQ(v2._z, 3.0f);
}

GTEST_TEST(Vector3, operatorMultiply) {
	Common::Vector3 v(1.0f, 2.0f, 3.0f);

	v *= 2.0f;

	EXPECT_EQ(v._x, 2.0f);
	EXPECT_EQ(v._y, 4.0f);
	EXPECT_EQ(v._z, 6.0f);
}

GTEST_TEST(Vector3, operatorDivide) {
	Common::Vector3 v(2.0f, 4.0f, 6.0f);

	v /= 2.0f;

	EXPECT_EQ(v._x, 1.0f);
	EXPECT_EQ(v._y, 2.0f);
	EXPECT_EQ(v._z, 3.0f);
}

GTEST_TEST(Vector3, multiply) {
	const Common::Vector3 v1(1.0f, 2.0f, 3.0f);
	Common::Vector3 v2(4.0f, 5.0f, 6.0f);

	v2.multiply(v1);

	EXPECT_EQ(v2._x,  4.0f);
	EXPECT_EQ(v2._y, 10.0f);
	EXPECT_EQ(v2._z, 18.0f);
}

GTEST_TEST(Vector3, divide) {
	const Common::Vector3 v1(2.0f, 3.0f,  4.0f);
	Common::Vector3 v2(4.0f, 9.0f, 16.0f);

	v2.divide(v1);

	EXPECT_EQ(v2._x, 2.0f);
	EXPECT_EQ(v2._y, 3.0f);
	EXPECT_EQ(v2._z, 4.0f);
}

GTEST_TEST(Vector3, operatorAddConst) {
	const Common::Vector3 v1(1.0f, 2.0f, 3.0f);
	const Common::Vector3 v2(4.0f, 5.0f, 6.0f);

	const Common::Vector3 v3 = v2 + v1;

	EXPECT_EQ(v3._x, 5.0f);
	EXPECT_EQ(v3._y, 7.0f);
	EXPECT_EQ(v3._z, 9.0f);
}

GTEST_TEST(Vector3, operatorSubtractConst) {
	const Common::Vector3 v1(1.0f, 2.0f, 3.0f);
	const Common::Vector3 v2(4.0f, 5.0f, 6.0f);

	const Common::Vector3 v3 = v2 - v1;

	EXPECT_EQ(v3._x, 3.0f);
	EXPECT_EQ(v3._y, 3.0f);
	EXPECT_EQ(v3._z, 3.0f);
}

GTEST_TEST(Vector3, operatorMultiplyConstFloat) {
	const Common::Vector3 v1(1.0f, 0.0f, 0.0f);
	const Common::Vector3 v2(0.0f, 1.0f, 0.0f);

	const Common::Vector3 v3 = v1 * v2;

	EXPECT_FLOAT_EQ(v3._x, 0.0f);
	EXPECT_FLOAT_EQ(v3._y, 0.0f);
	EXPECT_FLOAT_EQ(v3._z, 1.0f);
}

GTEST_TEST(Vector3, cross) {
	const Common::Vector3 v1(1.0f, 0.0f, 0.0f);
	const Common::Vector3 v2(0.0f, 1.0f, 0.0f);

	const Common::Vector3 v3 = v1.cross(v2);

	EXPECT_FLOAT_EQ(v3._x,  0.0f);
	EXPECT_FLOAT_EQ(v3._y,  0.0f);
	EXPECT_FLOAT_EQ(v3._z,  1.0f);

	const Common::Vector3 v4 = v2.cross(v1);

	EXPECT_FLOAT_EQ(v4._x,  0.0f);
	EXPECT_FLOAT_EQ(v4._y,  0.0f);
	EXPECT_FLOAT_EQ(v4._z, -1.0f);
}

GTEST_TEST(Vector3, dot) {
	const Common::Vector3 v1(1.0f, 0.0f, 0.0f);
	const Common::Vector3 v2(0.0f, 1.0f, 0.0f);

	EXPECT_FLOAT_EQ(v1.dot(v2), 0.0f);
	EXPECT_FLOAT_EQ(v2.dot(v1), 0.0f);

	const Common::Vector3 v3( 1.0f, 0.0f, 0.0f);
	const Common::Vector3 v4(-1.0f, 0.0f, 0.0f);

	EXPECT_FLOAT_EQ(v3.dot(v4), -1.0f);
	EXPECT_FLOAT_EQ(v4.dot(v3), -1.0f);

	const Common::Vector3 v5( 1.0f, 2.0f, 3.0f);
	const Common::Vector3 v6( 4.0f,-5.0f, 6.0f);

	EXPECT_FLOAT_EQ(v5.dot(v6), 12.0f);
	EXPECT_FLOAT_EQ(v6.dot(v5), 12.0f);
}

GTEST_TEST(Vector3, length) {
	const Common::Vector3 v1(1.0f, 0.0f, 0.0f);
	const Common::Vector3 v2(0.0f, 1.0f, 0.0f);
	const Common::Vector3 v3(0.0f, 0.0f, 1.0f);

	EXPECT_FLOAT_EQ(v1.length(), 1.0f);
	EXPECT_FLOAT_EQ(v2.length(), 1.0f);
	EXPECT_FLOAT_EQ(v3.length(), 1.0f);

	const Common::Vector3 v4(1.0f, 1.0f, 1.0f);
	EXPECT_FLOAT_EQ(v4.length(), std::sqrt(3.0f));
}

GTEST_TEST(Vector3, norm) {
	Common::Vector3 v1(1.0f, 0.0f, 0.0f);
	v1.norm();

	EXPECT_FLOAT_EQ(v1._x, 1.0f);
	EXPECT_FLOAT_EQ(v1._y, 0.0f);
	EXPECT_FLOAT_EQ(v1._z, 0.0f);

	Common::Vector3 v2(0.0f, 1.0f, 0.0f);
	v2.norm();

	EXPECT_FLOAT_EQ(v2._x, 0.0f);
	EXPECT_FLOAT_EQ(v2._y, 1.0f);
	EXPECT_FLOAT_EQ(v2._z, 0.0f);

	Common::Vector3 v3(0.0f, 0.0f, 1.0f);
	v3.norm();

	EXPECT_FLOAT_EQ(v3._x, 0.0f);
	EXPECT_FLOAT_EQ(v3._y, 0.0f);
	EXPECT_FLOAT_EQ(v3._z, 1.0f);

	Common::Vector3 v4(1.0f, 1.0f, 1.0f);
	v4.norm();

	EXPECT_FLOAT_EQ(v4._x, 1.0f / std::sqrt(3));
	EXPECT_FLOAT_EQ(v4._y, 1.0f / std::sqrt(3));
	EXPECT_FLOAT_EQ(v4._z, 1.0f / std::sqrt(3));
}

GTEST_TEST(Vector3, angle) {
	const Common::Vector3 v1(1.0f, 0.0f, 0.0f);
	const Common::Vector3 v2(0.0f, 1.0f, 0.0f);

	EXPECT_FLOAT_EQ(v1.angle(v2), 90.0f * M_PI / 180.0f);
	EXPECT_FLOAT_EQ(v2.angle(v1), 90.0f * M_PI / 180.0f);

	const Common::Vector3 v3(1.0f, 0.0f, 0.0f);
	const Common::Vector3 v4(1.0f, std::sqrt(3.0f), 0.0f);

	EXPECT_FLOAT_EQ(v3.angle(v4), 60.0f * M_PI / 180.0f);
	EXPECT_FLOAT_EQ(v4.angle(v3), 60.0f * M_PI / 180.0f);
}
