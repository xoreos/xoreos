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
 *  Unit tests for the BoundingBox class.
 */

#include "gtest/gtest.h"

#include "src/common/boundingbox.h"
#include "src/common/matrix4x4.h"
#include "src/common/maths.h"

static void compareULP(const Common::Matrix4x4 &m, const float *f, size_t t = 0) {
	for (size_t i = 0; i < 16; i++)
		EXPECT_FLOAT_EQ(m.get()[i], f[i]) << "At case " << t << ", index " << i;
}

static void compareNear(const Common::Matrix4x4 &m, const float *f, float range, size_t t = 0) {
	for (size_t i = 0; i < 16; i++)
		EXPECT_NEAR(m.get()[i], f[i], range) << "At case " << t << ", index " << i;
}

GTEST_TEST(BoundingBox, empty) {
	const Common::BoundingBox b;

	EXPECT_TRUE(b.empty());
}

GTEST_TEST(BoundingBox, addFloats) {
	Common::BoundingBox b;

	b.add(1.0f, 0.0f, 0.0f);

	EXPECT_FALSE(b.empty());
}

GTEST_TEST(BoundingBox, addBox) {
	Common::BoundingBox b1;
	Common::BoundingBox b2;

	b1.add(1.0f, 0.0f, 0.0f);
	b2.add(b1);

	EXPECT_FALSE(b2.empty());
}

GTEST_TEST(BoundingBox, clear) {
	Common::BoundingBox b;

	b.add(1.0f, 0.0f, 0.0f);

	EXPECT_FALSE(b.empty());

	b.clear();

	EXPECT_TRUE(b.empty());
}

GTEST_TEST(BoundingBox, getMin) {
	Common::BoundingBox b;

	b.add(1.0f,  2.0f,  3.0f);
	b.add(2.0f, -2.0f,  5.0f);
	b.add(1.0f,  9.0f, -3.0f);

	float x, y, z;
	b.getMin(x, y, z);

	EXPECT_FLOAT_EQ(x,  1.0f);
	EXPECT_FLOAT_EQ(y, -2.0f);
	EXPECT_FLOAT_EQ(z, -3.0f);
}

GTEST_TEST(BoundingBox, getMax) {
	Common::BoundingBox b;

	b.add(1.0f,  2.0f,  3.0f);
	b.add(2.0f, -2.0f,  5.0f);
	b.add(1.0f,  9.0f, -3.0f);

	float x, y, z;
	b.getMax(x, y, z);

	EXPECT_FLOAT_EQ(x,  2.0f);
	EXPECT_FLOAT_EQ(y,  9.0f);
	EXPECT_FLOAT_EQ(z,  5.0f);
}

GTEST_TEST(BoundingBox, getWidth) {
	Common::BoundingBox b;

	b.add(1.0f,  2.0f,  3.0f);
	b.add(2.0f, -2.0f,  5.0f);
	b.add(1.0f,  9.0f, -3.0f);

	EXPECT_FLOAT_EQ(b.getWidth(), 1.0f);
}

GTEST_TEST(BoundingBox, getHeight) {
	Common::BoundingBox b;

	b.add(1.0f,  2.0f,  3.0f);
	b.add(2.0f, -2.0f,  5.0f);
	b.add(1.0f,  9.0f, -3.0f);

	EXPECT_FLOAT_EQ(b.getHeight(), 11.0f);
}

GTEST_TEST(BoundingBox, getDepth) {
	Common::BoundingBox b;

	b.add(1.0f,  2.0f,  3.0f);
	b.add(2.0f, -2.0f,  5.0f);
	b.add(1.0f,  9.0f, -3.0f);

	EXPECT_FLOAT_EQ(b.getDepth(), 8.0f);
}

GTEST_TEST(BoundingBox, isIn2D) {
	Common::BoundingBox b;

	b.add(1.0f,  2.0f,  3.0f);
	b.add(2.0f, -2.0f,  5.0f);
	b.add(1.0f,  9.0f, -3.0f);

	EXPECT_TRUE(b.isIn(1.5f, 0.0f));
	EXPECT_FALSE(b.isIn(0.0f, 0.0f));

	EXPECT_TRUE(b.isIn(1.0f, -2.0f));
	EXPECT_TRUE(b.isIn(2.0f,  9.0f));
}

GTEST_TEST(BoundingBox, isIn3D) {
	Common::BoundingBox b;

	b.add(1.0f,  2.0f,  3.0f);
	b.add(2.0f, -2.0f,  5.0f);
	b.add(1.0f,  9.0f, -3.0f);

	EXPECT_TRUE(b.isIn(1.5f, 0.0f, 0.0f));
	EXPECT_FALSE(b.isIn(0.0f, 0.0f, 0.0f));

	EXPECT_TRUE(b.isIn(1.0f, -2.0f, -3.0f));
	EXPECT_TRUE(b.isIn(2.0f,  9.0f,  5.0f));
}

GTEST_TEST(BoundingBox, isInBox) {
	Common::BoundingBox b;

	b.add(1.0f,  2.0f,  3.0f);
	b.add(2.0f, -2.0f,  5.0f);
	b.add(1.0f,  9.0f, -3.0f);

	EXPECT_FALSE(b.isIn(- 5.0f, - 5.0f, - 5.0f, - 4.0f, - 4.0f, - 4.0f));
	EXPECT_FALSE(b.isIn( 10.0f,  10.0f,  10.0f,  11.0f,  11.0f,  11.0f));

	EXPECT_TRUE(b.isIn(1.5f, 0.0f, 0.0f, 1.6f, 0.1f, 0.1f));

	EXPECT_TRUE(b.isIn(-5.0f, -5.0f, -5.0f,  1.5f,  0.0f,  0.0f));
	EXPECT_TRUE(b.isIn( 1.5f,  0.0f,  0.0f, 11.0f, 11.0f, 11.0f));

	EXPECT_FALSE(b.isIn(-5.0f, -5.0f, -5.0f,  1.0f, - 2.0f, - 3.0f));
	EXPECT_FALSE(b.isIn( 2.0f,  9.0f,  5.0f, 11.0f,  11.0f,  11.0f));
}

GTEST_TEST(BoundingBox, getOrigin) {
	static const float kResult[] = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	const Common::BoundingBox b;

	compareULP(b.getOrigin(), kResult);
}

GTEST_TEST(BoundingBox, translate) {
	static const float kResult[] = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		2.0f, 3.0f, 4.0f, 1.0f
	};

	Common::BoundingBox b;
	b.translate(2.0f, 3.0f, 4.0f);

	compareULP(b.getOrigin(), kResult);
}

GTEST_TEST(BoundingBox, scale) {
	static const float kResult[] = {
		2.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 3.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 4.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	Common::BoundingBox b;
	b.scale(2.0f, 3.0f, 4.0f);

	compareULP(b.getOrigin(), kResult);
}

GTEST_TEST(BoundingBox, rotate) {
	static const float kResult[] = {
		1.0f,  0.0f,  0.0f, 0.0f,
		0.0f, -1.0f,  0.0f, 0.0f,
		0.0f,  0.0f, -1.0f, 0.0f,
		0.0f,  0.0f,  0.0f, 1.0f
	};

	Common::BoundingBox b;
	b.rotate(180.0f, 1.0f, 0.0f, 0.0f);

	compareNear(b.getOrigin(), kResult, 0.00001);
}

GTEST_TEST(BoundingBox, transform) {
	static const float kResult[] = {
		1.0f,  0.0f,  0.0f, 0.0f,
		0.0f, -1.0f,  0.0f, 0.0f,
		0.0f,  0.0f, -1.0f, 0.0f,
		0.0f,  0.0f,  0.0f, 1.0f
	};

	Common::BoundingBox b;

	Common::Matrix4x4 m;
	m.rotate(180.0f, 1.0f, 0.0f, 0.0f);

	b.transform(m);

	compareNear(b.getOrigin(), kResult, 0.00001);
}

GTEST_TEST(BoundingBox, absolutize) {
	Common::BoundingBox b;

	b.add(1.0f,  2.0f,  3.0f);
	b.add(2.0f, -2.0f,  5.0f);
	b.add(1.0f,  9.0f, -3.0f);

	b.translate(0.1f, 0.2f, 0.3f);
	b.absolutize();

	float x, y, z;
	b.getMin(x, y, z);

	EXPECT_FLOAT_EQ(x,   1.1f);
	EXPECT_FLOAT_EQ(y, - 1.8f);
	EXPECT_FLOAT_EQ(z, - 2.7f);

	b.getMax(x, y, z);

	EXPECT_FLOAT_EQ(x,   2.1f);
	EXPECT_FLOAT_EQ(y,   9.2f);
	EXPECT_FLOAT_EQ(z,   5.3f);

	b.scale(2.0f, 3.0f, 4.0f);
	b.absolutize();

	b.getMin(x, y, z);

	EXPECT_FLOAT_EQ(x,   2.2f);
	EXPECT_FLOAT_EQ(y, - 5.4f);
	EXPECT_FLOAT_EQ(z, -10.8f);

	b.getMax(x, y, z);

	EXPECT_FLOAT_EQ(x,   4.2f);
	EXPECT_FLOAT_EQ(y,  27.6f);
	EXPECT_FLOAT_EQ(z,  21.2f);

	b.rotate(180.0f, 1.0f, 0.0f, 0.0f);
	b.absolutize();

	b.getMin(x, y, z);

	EXPECT_FLOAT_EQ(x,   2.2f);
	EXPECT_FLOAT_EQ(y, -27.6f);
	EXPECT_FLOAT_EQ(z, -21.2f);

	b.getMax(x, y, z);

	EXPECT_FLOAT_EQ(x,   4.2f);
	EXPECT_FLOAT_EQ(y,   5.4f);
	EXPECT_FLOAT_EQ(z,  10.8f);
}

GTEST_TEST(BoundingBox, getAbsolute) {
	Common::BoundingBox b1;

	b1.add(1.0f,  2.0f,  3.0f);
	b1.add(2.0f, -2.0f,  5.0f);
	b1.add(1.0f,  9.0f, -3.0f);

	b1.translate(0.1f, 0.2f, 0.3f);

	Common::BoundingBox b2 = b1.getAbsolute();

	float x, y, z;
	b2.getMin(x, y, z);

	EXPECT_FLOAT_EQ(x,   1.1f);
	EXPECT_FLOAT_EQ(y, - 1.8f);
	EXPECT_FLOAT_EQ(z, - 2.7f);

	b2.getMax(x, y, z);

	EXPECT_FLOAT_EQ(x,   2.1f);
	EXPECT_FLOAT_EQ(y,   9.2f);
	EXPECT_FLOAT_EQ(z,   5.3f);
}
