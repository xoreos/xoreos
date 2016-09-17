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
 *  Unit tests for the Matrix4x4 class.
 */

#include "gtest/gtest.h"

#include "src/common/matrix4x4.h"
#include "src/common/vector3.h"
#include "src/common/maths.h"

static const float kIdentity[] = {
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
};

static const float kUniqueValues[] = {
	0.0f, 0.1f, 0.2f, 0.3f,
	1.0f, 1.1f, 1.2f, 1.3f,
	2.0f, 2.1f, 2.2f, 2.3f,
	3.0f, 3.1f, 3.2f, 3.3f
};

static void compareULP(const Common::Matrix4x4 &m, const float *f, size_t t = 0) {
	for (size_t i = 0; i < 16; i++)
		EXPECT_FLOAT_EQ(m.get()[i], f[i]) << "At case " << t << ", index " << i;
}

static void compareNear(const Common::Matrix4x4 &m, const float *f, float range, size_t t = 0) {
	for (size_t i = 0; i < 16; i++)
		EXPECT_NEAR(m.get()[i], f[i], range) << "At case " << t << ", index " << i;
}

GTEST_TEST(Matrix4x4, constructorDefault) {
	const Common::Matrix4x4 m;

	compareULP(m, kIdentity);
}

GTEST_TEST(Matrix4x4, constructorIdentity) {
	const Common::Matrix4x4 m(true);

	compareULP(m, kIdentity);
}

GTEST_TEST(Matrix4x4, constructorFloatArray) {
	const Common::Matrix4x4 m(kUniqueValues);

	compareULP(m, kUniqueValues);
}

GTEST_TEST(Matrix4x4, constructorCopy) {
	const Common::Matrix4x4 m1(kUniqueValues);
	const Common::Matrix4x4 m2(m1);

	compareULP(m2, kUniqueValues);
}

GTEST_TEST(Matrix4x4, loadIdentity) {
	Common::Matrix4x4 m(kUniqueValues);

	m.loadIdentity();
	compareULP(m, kIdentity);
}

GTEST_TEST(Matrix4x4, operatorArrayRead) {
	const Common::Matrix4x4 m(kUniqueValues);

	for (size_t i = 0; i < 16; i++)
		EXPECT_FLOAT_EQ(m[i], kUniqueValues[i]) << "At index " << i;
}

GTEST_TEST(Matrix4x4, operatorArrayWrite) {
	Common::Matrix4x4 m(true);
	for (size_t i = 0; i < 16; i++)
		m[i] = kUniqueValues[i];

	compareULP(m, kUniqueValues);
}

GTEST_TEST(Matrix4x4, operatorFunctionRead) {
	const Common::Matrix4x4 m(kUniqueValues);

	for (size_t i = 0; i < 4; i++)
		for (size_t j = 0; j < 4; j++)
			EXPECT_FLOAT_EQ(m(i, j), kUniqueValues[j * 4 + i]) << "At index " << i << "." << j;
}

GTEST_TEST(Matrix4x4, operatorFunctionWrite) {
	Common::Matrix4x4 m(true);
	for (size_t i = 0; i < 4; i++)
		for (size_t j = 0; j < 4; j++)
			m(i, j) = kUniqueValues[j * 4 + i];

	compareULP(m, kUniqueValues);
}

GTEST_TEST(Matrix4x4, operatorAssignArray) {
	Common::Matrix4x4 m(true);
	m = kUniqueValues;

	compareULP(m, kUniqueValues);
}

GTEST_TEST(Matrix4x4, operatorAssignMatrix) {
	const Common::Matrix4x4 m1(kUniqueValues);
	Common::Matrix4x4 m2(true);

	m2 = m1;

	compareULP(m2, kUniqueValues);
}

GTEST_TEST(Matrix4x4, getX) {
	const Common::Matrix4x4 m(kUniqueValues);

	EXPECT_FLOAT_EQ(m.getX(), 3.0f);
}

GTEST_TEST(Matrix4x4, getY) {
	const Common::Matrix4x4 m(kUniqueValues);

	EXPECT_FLOAT_EQ(m.getY(), 3.1f);
}

GTEST_TEST(Matrix4x4, getZ) {
	const Common::Matrix4x4 m(kUniqueValues);

	EXPECT_FLOAT_EQ(m.getZ(), 3.2f);
}

GTEST_TEST(Matrix4x4, getPositionFloats) {
	const Common::Matrix4x4 m(kUniqueValues);

	float x, y, z;
	m.getPosition(x, y, z);

	EXPECT_FLOAT_EQ(x, 3.0f);
	EXPECT_FLOAT_EQ(y, 3.1f);
	EXPECT_FLOAT_EQ(z, 3.2f);
}

GTEST_TEST(Matrix4x4, getPositionArray) {
	const Common::Matrix4x4 m(kUniqueValues);

	const float *pos = m.getPosition();

	EXPECT_FLOAT_EQ(pos[0], 3.0f);
	EXPECT_FLOAT_EQ(pos[1], 3.1f);
	EXPECT_FLOAT_EQ(pos[2], 3.2f);
}

GTEST_TEST(Matrix4x4, getXAxis) {
	const Common::Matrix4x4 m(kUniqueValues);

	const float *axis = m.getXAxis();

	EXPECT_FLOAT_EQ(axis[0], 0.0f);
	EXPECT_FLOAT_EQ(axis[1], 0.1f);
	EXPECT_FLOAT_EQ(axis[2], 0.2f);
}

GTEST_TEST(Matrix4x4, getYAxis) {
	const Common::Matrix4x4 m(kUniqueValues);

	const float *axis = m.getYAxis();

	EXPECT_FLOAT_EQ(axis[0], 1.0f);
	EXPECT_FLOAT_EQ(axis[1], 1.1f);
	EXPECT_FLOAT_EQ(axis[2], 1.2f);
}

GTEST_TEST(Matrix4x4, getZAxis) {
	const Common::Matrix4x4 m(kUniqueValues);

	const float *axis = m.getZAxis();

	EXPECT_FLOAT_EQ(axis[0], 2.0f);
	EXPECT_FLOAT_EQ(axis[1], 2.1f);
	EXPECT_FLOAT_EQ(axis[2], 2.2f);
}

GTEST_TEST(Matrix4x4, translateFloats) {
	static const float kResult[] = {
		 0.0f,  0.1f,  0.2f,   0.3f,
		 1.0f,  1.1f,  1.2f,   1.3f,
		 2.0f,  2.1f,  2.2f,   2.3f,
		83.0f, 89.1f, 95.2f, 101.3f
	};

	Common::Matrix4x4 m(kUniqueValues);

	m.translate(10.0f, 20.0f, 30.0f);

	compareULP(m, kResult);
}

GTEST_TEST(Matrix4x4, translateVector3) {
	static const float kResult[] = {
		 0.0f,  0.1f,  0.2f,   0.3f,
		 1.0f,  1.1f,  1.2f,   1.3f,
		 2.0f,  2.1f,  2.2f,   2.3f,
		83.0f, 89.1f, 95.2f, 101.3f
	};

	Common::Matrix4x4 m(kUniqueValues);

	m.translate(Common::Vector3(10.0f, 20.0f, 30.0f));

	compareULP(m, kResult);
}

GTEST_TEST(Matrix4x4, scaleFloats) {
	static const float kResult[] = {
		 0.0f,  1.0f,  2.0f,  3.0f,
		20.0f, 22.0f, 24.0f, 26.0f,
		60.0f, 63.0f, 66.0f, 69.0f,
		 3.0f,  3.1f,  3.2f,  3.3f
	};

	Common::Matrix4x4 m(kUniqueValues);

	m.scale(10.0f, 20.0f, 30.0f);

	compareULP(m, kResult);
}

GTEST_TEST(Matrix4x4, scaleVector3) {
	static const float kResult[] = {
		 0.0f,  1.0f,  2.0f,  3.0f,
		20.0f, 22.0f, 24.0f, 26.0f,
		60.0f, 63.0f, 66.0f, 69.0f,
		 3.0f,  3.1f,  3.2f,  3.3f
	};

	Common::Matrix4x4 m(kUniqueValues);

	m.scale(Common::Vector3(10.0f, 20.0f, 30.0f));

	compareULP(m, kResult);
}

GTEST_TEST(Matrix4x4, getTranspose) {
	static const float kResult[] = {
		0.0f, 1.0f, 2.0f, 3.0f,
		0.1f, 1.1f, 2.1f, 3.1f,
		0.2f, 1.2f, 2.2f, 3.2f,
		0.3f, 1.3f, 2.3f, 3.3f
	};

	const Common::Matrix4x4 m1(kUniqueValues);
	const Common::Matrix4x4 m2 = m1.getTranspose();

	compareULP(m2, kResult);
}

GTEST_TEST(Matrix4x4, getInverse) {
	const Common::Matrix4x4 m1;
	const Common::Matrix4x4 m1I = m1.getInverse();

	// Inverse of the identity matrix is the identity matrix
	compareULP(m1I, kIdentity, 0);

	const Common::Matrix4x4 m2(kUniqueValues);
	const Common::Matrix4x4 m2I = m2.getInverse();

	// This matrix isn't invertible, so the identity matrix is returned
	compareULP(m2I, kIdentity, 1);

	static const float kSTMatrix[] = {
		 2.0f,  0.0f,  0.0f,  0.0f,
		 0.0f,  2.0f,  0.0f,  0.0f,
		 0.0f,  0.0f,  2.0f,  0.0f,
		 5.0f,  6.0f,  7.0f,  1.0f
	};

	static const float kSTMatrixInverse[] = {
		 0.5f,  0.0f,  0.0f,  0.0f,
		 0.0f,  0.5f,  0.0f,  0.0f,
		 0.0f,  0.0f,  0.5f,  0.0f,
		-2.5f, -3.0f, -3.5f,  1.0f
	};

	const Common::Matrix4x4 m3(kSTMatrix);
	const Common::Matrix4x4 m3I = m3.getInverse();

	compareULP(m3I, kSTMatrixInverse, 3);
}

GTEST_TEST(Matrix4x4, getScale) {
	static const float kSMatrix[] = {
		2.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 3.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 4.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	const Common::Matrix4x4 m(kSMatrix);

	float x, y, z;
	m.getScale(x, y, z);

	EXPECT_FLOAT_EQ(x, 2.0f);
	EXPECT_FLOAT_EQ(y, 3.0f);
	EXPECT_FLOAT_EQ(z, 4.0f);
}

GTEST_TEST(Matrix4x4, rotateAxisAngle) {
	static const float kResult[] = {
		1.0f,  0.0f,  0.0f, 0.0f,
		0.0f, -1.0f,  0.0f, 0.0f,
		0.0f,  0.0f, -1.0f, 0.0f,
		0.0f,  0.0f,  0.0f, 1.0f
	};

	Common::Matrix4x4 m;

	m.rotate(180.0f, 1.0f, 0.0f, 0.0f);

	compareNear(m, kResult, 0.00001);
}

GTEST_TEST(Matrix4x4, getAxisAngle) {
	static const float kRMatrix[] = {
		1.0f,  0.0f,  0.0f, 0.0f,
		0.0f, -1.0f,  0.0f, 0.0f,
		0.0f,  0.0f, -1.0f, 0.0f,
		0.0f,  0.0f,  0.0f, 1.0f
	};

	const Common::Matrix4x4 m(kRMatrix);

	float angle, x, y, z;
	m.getAxisAngle(angle, x, y, z);

	EXPECT_FLOAT_EQ(angle, 180.0f);
	EXPECT_FLOAT_EQ(x    ,   1.0f);
	EXPECT_FLOAT_EQ(y    ,   0.0f);
	EXPECT_FLOAT_EQ(z    ,   0.0f);
}

GTEST_TEST(Matrix4x4, rotateAxisLocal) {
	static const float kRMatrix[] = {
		 0.0f,  0.0f,  1.0f,  0.0f,
		 0.0f,  0.0f,  0.0f,  0.0f,
		 0.0f, -1.0f,  0.0f,  0.0f,
		 0.0f,  0.0f,  0.0f,  1.0f
	};

	static const float kResult[] = {
		 0.0f,  0.0f,  0.0f,  0.0f,
		 0.0f,  0.0f, -1.0f,  0.0f,
		 0.0f, -1.0f,  0.0f,  0.0f,
		 0.0f,  0.0f,  0.0f,  1.0f
	};

	Common::Matrix4x4 m(kRMatrix);

	m.rotateAxisLocal(Common::Vector3(0.0f, 0.0f, 1.0f), 90.0f);

	compareNear(m, kResult, 0.00001);
}

GTEST_TEST(Matrix4x4, rotateAxisWorld) {
	static const float kRMatrix[] = {
		 0.0f,  0.0f,  1.0f,  0.0f,
		 0.0f,  0.0f,  0.0f,  0.0f,
		 0.0f, -1.0f,  0.0f,  0.0f,
		 0.0f,  0.0f,  0.0f,  1.0f
	};

	static const float kResult[] = {
		 0.0f, -1.0f,  0.0f,  0.0f,
		 0.0f,  0.0f,  0.0f,  0.0f,
		 0.0f,  0.0f, -1.0f,  0.0f,
		 0.0f,  0.0f,  0.0f,  1.0f
	};

	Common::Matrix4x4 m(kRMatrix);

	m.rotateAxisWorld(Common::Vector3(0.0f, 0.0f, 1.0f), 90.0f);

	compareNear(m, kResult, 0.00001);
}

GTEST_TEST(Matrix4x4, rotateXAxisLocal) {
	static const float kRMatrix[] = {
		 0.0f,  0.0f,  1.0f,  0.0f,
		 0.0f,  0.0f,  0.0f,  0.0f,
		 0.0f, -1.0f,  0.0f,  0.0f,
		 0.0f,  0.0f,  0.0f,  1.0f
	};

	static const float kResult[] = {
		 0.0f,  0.0f,  1.0f,  0.0f,
		 0.0f, -1.0f,  0.0f,  0.0f,
		 0.0f,  0.0f,  0.0f,  0.0f,
		 0.0f,  0.0f,  0.0f,  1.0f
	};

	Common::Matrix4x4 m(kRMatrix);

	m.rotateXAxisLocal(90.0f);

	compareNear(m, kResult, 0.00001);
}

GTEST_TEST(Matrix4x4, rotateXAxisWorld) {
	static const float kRMatrix[] = {
		 0.0f,  0.0f,  1.0f,  0.0f,
		 0.0f,  0.0f,  0.0f,  0.0f,
		 0.0f, -1.0f,  0.0f,  0.0f,
		 0.0f,  0.0f,  0.0f,  1.0f
	};

	static const float kResult[] = {
		 0.0f,  0.0f,  0.0f,  0.0f,
		 0.0f,  0.0f, -1.0f,  0.0f,
		 0.0f, -1.0f,  0.0f,  0.0f,
		 0.0f,  0.0f,  0.0f,  1.0f
	};

	Common::Matrix4x4 m(kRMatrix);

	m.rotateXAxisWorld(90.0f);

	compareNear(m, kResult, 0.00001);
}

GTEST_TEST(Matrix4x4, rotateYAxisLocal) {
	static const float kRMatrix[] = {
		 0.0f, -1.0f,  0.0f,  0.0f,
		 0.0f,  0.0f,  1.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,  0.0f,
		 0.0f,  0.0f,  0.0f,  1.0f
	};

	static const float kResult[] = {
		 1.0f,  0.0f,  0.0f,  0.0f,
		 0.0f,  0.0f,  1.0f,  0.0f,
		 0.0f, -1.0f,  0.0f,  0.0f,
		 0.0f,  0.0f,  0.0f,  1.0f
	};

	Common::Matrix4x4 m(kRMatrix);

	m.rotateYAxisLocal(90.0f);

	compareNear(m, kResult, 0.00001);
}

GTEST_TEST(Matrix4x4, rotateYAxisWorld) {
	static const float kRMatrix[] = {
		 0.0f, -1.0f,  0.0f,  0.0f,
		 0.0f,  0.0f,  1.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,  0.0f,
		 0.0f,  0.0f,  0.0f,  1.0f
	};

	static const float kResult[] = {
		 0.0f,  0.0f,  1.0f,  0.0f,
		 0.0f,  1.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,  0.0f,
		 0.0f,  0.0f,  0.0f,  1.0f
	};

	Common::Matrix4x4 m(kRMatrix);

	m.rotateYAxisWorld(90.0f);

	compareNear(m, kResult, 0.00001);
}

GTEST_TEST(Matrix4x4, rotateZAxisLocal) {
	static const float kRMatrix[] = {
		 0.0f,  0.0f,  1.0f,  0.0f,
		 0.0f,  0.0f,  0.0f,  0.0f,
		 0.0f, -1.0f,  0.0f,  0.0f,
		 0.0f,  0.0f,  0.0f,  1.0f
	};

	static const float kResult[] = {
		 0.0f,  0.0f,  0.0f,  0.0f,
		 0.0f,  0.0f, -1.0f,  0.0f,
		 0.0f, -1.0f,  0.0f,  0.0f,
		 0.0f,  0.0f,  0.0f,  1.0f
	};

	Common::Matrix4x4 m(kRMatrix);

	m.rotateZAxisLocal(90.0f);

	compareNear(m, kResult, 0.00001);
}

GTEST_TEST(Matrix4x4, rotateZAxisWorld) {
	static const float kRMatrix[] = {
		 0.0f,  0.0f,  1.0f,  0.0f,
		 0.0f,  0.0f,  0.0f,  0.0f,
		 0.0f, -1.0f,  0.0f,  0.0f,
		 0.0f,  0.0f,  0.0f,  1.0f
	};

	static const float kResult[] = {
		 0.0f, -1.0f,  0.0f,  0.0f,
		 0.0f,  0.0f,  0.0f,  0.0f,
		 0.0f,  0.0f, -1.0f,  0.0f,
		 0.0f,  0.0f,  0.0f,  1.0f
	};

	Common::Matrix4x4 m(kRMatrix);

	m.rotateZAxisWorld(90.0f);

	compareNear(m, kResult, 0.00001);
}

GTEST_TEST(Matrix4x4, setRotation) {
	static const float kResult[] = {
		0.0f, 0.1f, 0.2f, 0.0f,
		1.0f, 1.1f, 1.2f, 0.0f,
		2.0f, 2.1f, 2.2f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	const Common::Matrix4x4 m1(kUniqueValues);
	Common::Matrix4x4 m2;

	m2.setRotation(m1);

	compareULP(m2, kResult);
}

GTEST_TEST(Matrix4x4, resetRotation) {
	static const float kResult[] = {
		1.0f, 0.0f, 0.0f, 0.3f,
		0.0f, 1.0f, 0.0f, 1.3f,
		0.0f, 0.0f, 1.0f, 2.3f,
		3.0f, 3.1f, 3.2f, 3.3f
	};

	Common::Matrix4x4 m(kUniqueValues);

	m.resetRotation();

	compareULP(m, kResult);
}

GTEST_TEST(Matrix4x4, transformA) {
	static const float kRMatrix[] = {
		1.0f,  0.0f,  0.0f, 0.0f,
		0.0f, -1.0f,  0.0f, 0.0f,
		0.0f,  0.0f, -1.0f, 0.0f,
		0.0f,  0.0f,  0.0f, 1.0f
	};

	static const float kTMatrix[] = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 5.0f, 0.0f, 1.0f
	};

	static const float kResult[] = {
		 1.0f,  0.0f,  0.0f, 0.0f,
		 0.0f, -1.0f,  0.0f, 0.0f,
		 0.0f,  0.0f, -1.0f, 0.0f,
		 0.0f, -5.0f,  0.0f, 1.0f
	};

	Common::Matrix4x4 m1(kRMatrix);
	Common::Matrix4x4 m2(kTMatrix);

	m1.transform(m2);

	compareNear(m1, kResult, 0.00001);
}

GTEST_TEST(Matrix4x4, transformAB) {
	static const float kRMatrix[] = {
		1.0f,  0.0f,  0.0f, 0.0f,
		0.0f, -1.0f,  0.0f, 0.0f,
		0.0f,  0.0f, -1.0f, 0.0f,
		0.0f,  0.0f,  0.0f, 1.0f
	};

	static const float kTMatrix[] = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 5.0f, 0.0f, 1.0f
	};

	static const float kResult[] = {
		 1.0f,  0.0f,  0.0f, 0.0f,
		 0.0f, -1.0f,  0.0f, 0.0f,
		 0.0f,  0.0f, -1.0f, 0.0f,
		 0.0f, -5.0f,  0.0f, 1.0f
	};

	Common::Matrix4x4 m1(kRMatrix);
	Common::Matrix4x4 m2(kTMatrix);
	Common::Matrix4x4 m3;

	m3.transform(m1, m2);

	compareNear(m3, kResult, 0.00001);
}

GTEST_TEST(Matrix4x4, lookAt) {
	static const float kResult[] = {
		 0.0f,  0.0f, -1.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,  0.0f,
		 0.0f,  1.0f,  0.0f,  0.0f,
		 0.0f,  0.0f,  0.0f,  1.0f
	};

	Common::Matrix4x4 m;

	m.lookAt(Common::Vector3(0.0f, 1.0f, 0.0f));

	compareULP(m, kResult);
}

GTEST_TEST(Matrix4x4, perspective) {
	static const float kResult[] = {
		1.0f, 0.0f,  0.00000000f,  0.0f,
		0.0f, 1.0f,  0.00000000f,  0.0f,
		0.0f, 0.0f, -1.02020202f, -1.0f,
		0.0f, 0.0f, -2.02020202f,  0.0f
	};

	Common::Matrix4x4 m;

	m.perspective(90, 1.0f, 1.0f, 100.0f);

	compareULP(m, kResult);
}

GTEST_TEST(Matrix4x4, ortho) {
	static const float kResult[] = {
		1.0f,  0.0f,  0.00000000f, 0.0f,
		0.0f, -1.0f,  0.00000000f, 0.0f,
		0.0f,  0.0f, -0.02020202f, 0.0f,
		0.0f,  0.0f, -1.02020202f, 1.0f
	};

	Common::Matrix4x4 m;

	m.ortho(-1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 100.0f);

	compareULP(m, kResult);
}

GTEST_TEST(Matrix4x4, multiplyMatrix) {
	static const float kRMatrix[] = {
		1.0f,  0.0f,  0.0f, 0.0f,
		0.0f, -1.0f,  0.0f, 0.0f,
		0.0f,  0.0f, -1.0f, 0.0f,
		0.0f,  0.0f,  0.0f, 1.0f
	};

	static const float kTMatrix[] = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 5.0f, 0.0f, 1.0f
	};

	static const float kResult[] = {
		 1.0f,  0.0f,  0.0f, 0.0f,
		 0.0f, -1.0f,  0.0f, 0.0f,
		 0.0f,  0.0f, -1.0f, 0.0f,
		 0.0f, -5.0f,  0.0f, 1.0f
	};

	Common::Matrix4x4 m1(kRMatrix);
	Common::Matrix4x4 m2(kTMatrix);

	m1 *= m2;

	compareNear(m1, kResult, 0.00001);
}

GTEST_TEST(Matrix4x4, multiplyMatrixConst) {
	static const float kRMatrix[] = {
		1.0f,  0.0f,  0.0f, 0.0f,
		0.0f, -1.0f,  0.0f, 0.0f,
		0.0f,  0.0f, -1.0f, 0.0f,
		0.0f,  0.0f,  0.0f, 1.0f
	};

	static const float kTMatrix[] = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 5.0f, 0.0f, 1.0f
	};

	static const float kResult[] = {
		 1.0f,  0.0f,  0.0f, 0.0f,
		 0.0f, -1.0f,  0.0f, 0.0f,
		 0.0f,  0.0f, -1.0f, 0.0f,
		 0.0f, -5.0f,  0.0f, 1.0f
	};

	const Common::Matrix4x4 m1(kRMatrix);
	const Common::Matrix4x4 m2(kTMatrix);

	const Common::Matrix4x4 m3 = m1 * m2;

	compareNear(m3, kResult, 0.00001);
}

GTEST_TEST(Matrix4x4, multiplyVector) {
	static const float kRMatrix[] = {
		1.0f,  0.0f,  0.0f, 0.0f,
		0.0f, -1.0f,  0.0f, 0.0f,
		0.0f,  0.0f, -1.0f, 0.0f,
		0.0f,  0.0f,  0.0f, 1.0f
	};

	const Common::Matrix4x4 m(kRMatrix);

	const Common::Vector3 v = m * Common::Vector3(0.0f, 5.0f, 0.0f);

	EXPECT_FLOAT_EQ(v._x,  0.0f);
	EXPECT_FLOAT_EQ(v._y, -5.0f);
	EXPECT_FLOAT_EQ(v._z,  0.0f);
}

GTEST_TEST(Matrix4x4, vectorRotate) {
	static const float kRMatrix[] = {
		1.0f,  0.0f,  0.0f, 0.0f,
		0.0f, -1.0f,  0.0f, 0.0f,
		0.0f,  0.0f, -1.0f, 0.0f,
		0.0f,  0.0f,  0.0f, 1.0f
	};

	const Common::Matrix4x4 m(kRMatrix);

	const Common::Vector3 v = m.vectorRotate(Common::Vector3(0.0f, 5.0f, 0.0f));

	EXPECT_FLOAT_EQ(v._x,  0.0f);
	EXPECT_FLOAT_EQ(v._y, -5.0f);
	EXPECT_FLOAT_EQ(v._z,  0.0f);
}

GTEST_TEST(Matrix4x4, vectorRotateReverse) {
	static const float kRMatrix[] = {
		1.0f,  0.0f,  0.0f, 0.0f,
		0.0f, -1.0f,  0.0f, 0.0f,
		0.0f,  0.0f, -1.0f, 0.0f,
		0.0f,  0.0f,  0.0f, 1.0f
	};

	const Common::Matrix4x4 m(kRMatrix);

	const Common::Vector3 v = m.vectorRotateReverse(Common::Vector3(0.0f, 5.0f, 0.0f));

	EXPECT_FLOAT_EQ(v._x,  0.0f);
	EXPECT_FLOAT_EQ(v._y, -5.0f);
	EXPECT_FLOAT_EQ(v._z,  0.0f);
}
