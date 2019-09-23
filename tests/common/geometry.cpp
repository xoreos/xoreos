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
 *  Unit tests for our geometrical algorithms helpers.
 */

#include "gtest/gtest.h"

#include "src/common/geometry.h"

static const glm::vec2 pA = glm::vec2(0.f, 0.f);
static const glm::vec2 pB = glm::vec2(1.f, 1.f);
static const glm::vec2 pC = glm::vec2(0.5, 0.5);
static const glm::vec2 pD = glm::vec2(1.5, 1.5);
static const glm::vec2 pE = glm::vec2(0.5, 1.2);
static const glm::vec2 pF = glm::vec2(1.2, 1.2);
static const glm::vec2 pG = glm::vec2(1.2, 0.5);
static const glm::vec2 pH = glm::vec2(-0.5, 0.5);
static const glm::vec2 pI = glm::vec2(1.f, 0.f);
static const glm::vec2 pJ = glm::vec2(0.f, 1.f);

GTEST_TEST(Geometry, intersectBoxes3D) {
	ASSERT_TRUE(Common::intersectBoxes3D(pA, pB, pC, pD));
	ASSERT_TRUE(Common::intersectBoxes3D(pC, pD, pA, pB));
	ASSERT_TRUE(Common::intersectBoxes3D(pA, pB, pB, pD));
	ASSERT_TRUE(Common::intersectBoxes3D(pB, pD, pA, pB));
	ASSERT_FALSE(Common::intersectBoxes3D(pA, pB, pE, pD));
	ASSERT_FALSE(Common::intersectBoxes3D(pE, pD, pA, pB));
	ASSERT_FALSE(Common::intersectBoxes3D(pA, pB, pG, pD));
	ASSERT_FALSE(Common::intersectBoxes3D(pG, pD, pA, pB));
	ASSERT_FALSE(Common::intersectBoxes3D(pA, pB, pF, pD));
	ASSERT_FALSE(Common::intersectBoxes3D(pF, pD, pA, pB));
	ASSERT_TRUE(Common::intersectBoxes3D(pA, pB, pH, pE));
	ASSERT_TRUE(Common::intersectBoxes3D(pH, pE, pA, pB));

	glm::vec2 minA(30.f, 30.f);
	glm::vec2 maxA(40.f, 40.f);
	glm::vec2 minB(30.825901, 31.843029);
	glm::vec2 maxB(37.725903, 38.743031);
	ASSERT_TRUE(Common::intersectBoxes3D(minB, maxB, minA, maxA));
}

GTEST_TEST(Geometry, intersectBoxSegment2D) {
	ASSERT_FALSE(Common::intersectBoxSegment2D(pA, pC, pB, pD));
	ASSERT_TRUE(Common::intersectBoxSegment2D(pC, pB, pA, pD));
	ASSERT_TRUE(Common::intersectBoxSegment2D(pA, pB, pC, pH));
	ASSERT_TRUE(Common::intersectBoxSegment2D(pA, pB, pB, pD));
}

GTEST_TEST(Geometry, intersectBoxTriangle3D) {
	glm::vec3 vA(12.f, 9.f, 9.f);
	glm::vec3 vB(9.f, 12.f, 9.f);
	glm::vec3 vC(19.f, 19.f,20.f);
	glm::vec3 vD(0.f, 0.f, 0.f);
	glm::vec3 min(-10.f, -10.f, -10.f);
	glm::vec3 max(10.f, 10.f, 10.f);
	ASSERT_FALSE(Common::intersectBoxTriangle3D(min, max, vA, vB, vC));
	ASSERT_TRUE(Common::intersectBoxTriangle3D(min, max, vA, vD, vC));
	ASSERT_TRUE(Common::intersectBoxTriangle3D(min, max, max, max, vC));
}

GTEST_TEST(Geometry, intersectSegmentPoint2D) {
	ASSERT_TRUE(Common::intersectSegmentPoint2D(pA, pD, pB));
	ASSERT_FALSE(Common::intersectSegmentPoint2D(pC, pB, pA));
	ASSERT_TRUE(Common::intersectSegmentPoint2D(pA, pB, pA));
	ASSERT_FALSE(Common::intersectSegmentPoint2D(pA, pB, pH));
}

GTEST_TEST(Geometry, intersectSegments2D) {
	glm::vec2 intersectA, intersectB;

	ASSERT_EQ(Common::intersectSegments2D(pA, pF, pC, pD, intersectA, intersectB), 2);
	ASSERT_TRUE(intersectA == pC || intersectA == pF);
	ASSERT_TRUE(intersectB == pC || intersectB == pF);

	ASSERT_EQ(Common::intersectSegments2D(pA, pB, pI, pJ, intersectA, intersectB), 1);
	ASSERT_EQ(intersectA, pC);

	ASSERT_EQ(Common::intersectSegments2D(pA, pC, pB, pD, intersectA, intersectB), 0);

	ASSERT_EQ(Common::intersectSegments2D(pA, pC, pB, pJ, intersectA, intersectB), 0);

	ASSERT_EQ(Common::intersectSegments2D(pA, pA, pA, pA, intersectA, intersectB), 1);
	ASSERT_EQ(pA, intersectA);
}

GTEST_TEST(Geometry, intersectBoxTriangle2D) {
	// Triangle inside the box.
	ASSERT_TRUE(Common::intersectBoxTriangle2D(pA, pB, pA, pC, pI));
	// Triangle crossing the box.
	ASSERT_TRUE(Common::intersectBoxTriangle2D(pA, pB, pC, pD, pG));
	// Box inside the triangle.
	ASSERT_TRUE(Common::intersectBoxTriangle2D(pB, pF, pI, pJ, pD));

	ASSERT_FALSE(Common::intersectBoxTriangle2D(pA, pC, pB, pJ, pD));
}

GTEST_TEST(Geometry, intersectTrianglePoint2D) {
	ASSERT_TRUE(Common::intersectTrianglePoint2D(pF, pI, pJ, pD));
	ASSERT_FALSE(Common::intersectTrianglePoint2D(pI, pA, pC, pJ));
	// Check a point on the border of the triangle.
	ASSERT_TRUE(Common::intersectTrianglePoint2D(pC, pA, pB, pJ));
}

GTEST_TEST(Geometry, intersectTriangleSegment2D) {
	ASSERT_TRUE(Common::intersectTriangleSegment2D(pD, pJ, pI, pA, pF));
	ASSERT_TRUE(Common::intersectTriangleSegment2D(pI, pJ, pD, pF, pF));
	ASSERT_FALSE(Common::intersectTriangleSegment2D(pA, pC, pJ, pI, pD));
}
