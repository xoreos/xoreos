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
 *  Unit tests AABB nodes.
 */

#include "gtest/gtest.h"

#include "src/common/aabbnode.h"

GTEST_TEST(AABBNode, hasChildren) {
	float min[] = {0.f, 0.f, 0.f};
	float max[] = {1.f, 1.f, 1.f};
	Common::AABBNode a(min, max);
	Common::AABBNode *b = new Common::AABBNode(min, max);
	Common::AABBNode *c = new Common::AABBNode(min, max);

	a.setChildren(b, c);
	EXPECT_TRUE(a.hasChildren());
}

GTEST_TEST(AABBNode, rotate) {
	float min[] = {0.f, 0.f, 0.f};
	float max[] = {1.f, 2.f, 0.f};
	Common::AABBNode a(min, max);
	float x, y, z;

	a.rotate(90.f, 0.f, 0.f, 1.f);
	a.getMax(x, y, z);
	ASSERT_FLOAT_EQ(x, 0.f);
	ASSERT_FLOAT_EQ(y, 1.f);
	a.getMin(x, y, z);
	ASSERT_FLOAT_EQ(x, -2.f);
	ASSERT_FLOAT_EQ(y, 0.f);

	a.rotate(270.f, 0.f, 0.f, 1.f);
	a.getMax(x, y, z);
	ASSERT_FLOAT_EQ(x, 1.f);
	ASSERT_FLOAT_EQ(y, 2.f);
}

GTEST_TEST(AABBNode, setOrientation) {
	float min[] = {0.f, 0.f, 0.f};
	float max[] = {1.f, 2.f, 0.f};
	Common::AABBNode a(min, max);

	a.setOrientation(1);
	float x, y, z;
	a.getMax(x, y, z);
	ASSERT_FLOAT_EQ(x, 0.f);
	ASSERT_FLOAT_EQ(y, 1.f);
	a.getMin(x, y, z);
	ASSERT_FLOAT_EQ(x, -2.f);
	ASSERT_FLOAT_EQ(y, 0.f);

	Common::AABBNode *b = new Common::AABBNode(min, max);
	Common::AABBNode *c = new Common::AABBNode(min, max);
	a.setChildren(b, c);
	a.setOrientation(1);
	b->getMax(x, y, z);
	ASSERT_FLOAT_EQ(x, 0.f);
	ASSERT_FLOAT_EQ(y, 1.f);
}

GTEST_TEST(AABBNode, translate) {
	float min[] = {0.f, 0.f, 0.f};
	float max[] = {1.f, 2.f, 0.f};
	Common::AABBNode a(min, max);
	Common::AABBNode *b = new Common::AABBNode(min, max);
	Common::AABBNode *c = new Common::AABBNode(min, max);
	a.setChildren(b, c);

	a.translate(1.f, 1.f, 0.f);
	float x, y, z;
	a.getMax(x, y, z);
	ASSERT_FLOAT_EQ(x, 2.f);
	ASSERT_FLOAT_EQ(y, 3.f);
	ASSERT_FLOAT_EQ(z, 0.f);

	b->getMax(x, y, z);
	ASSERT_FLOAT_EQ(x, 2.f);
	ASSERT_FLOAT_EQ(y, 3.f);
	ASSERT_FLOAT_EQ(z, 0.f);
}

GTEST_TEST(AABBNode, getNodes) {
	float min[3] = {0.f, 0.f, 0.f};
	float max[3] = {1.f, 2.f, 0.f};
	Common::AABBNode a = Common::AABBNode(min, max);

	std::vector<Common::AABBNode *> nodes;
	a.getNodes(0.5, 0.5, 1.f, 0.5, 0.5, -1.f, nodes);
	ASSERT_TRUE(nodes.size() == 1);

	nodes.clear();
	a.getNodes(2.5, 2.5, 1.f, 2.5, 2.5, -1.f, nodes);
	ASSERT_TRUE(nodes.empty());

	nodes.clear();
	a.getNodes(0.5, 0.5, nodes);
	ASSERT_TRUE(nodes.size() == 1);

	nodes.clear();
	a.getNodes(2.5, 2.5, nodes);
	ASSERT_TRUE(nodes.empty());
}

GTEST_TEST(AABBNode, getNodesInAABox2D) {
	float min[3] = {0.f, 0.f, 0.f};
	float max[3] = {1.f, 2.f, 0.f};
	Common::AABBNode a = Common::AABBNode(min, max);

	std::vector<Common::AABBNode *> nodes;
	a.getNodesInAABox2D(glm::vec3(0.5, 0.5, 0), glm::vec3(1.5, 1.5, 0), nodes);
	ASSERT_TRUE(nodes.size() == 1);

	nodes.clear();
	a.getNodesInAABox2D(glm::vec3(2.5, 2.5, 0), glm::vec3(3.5, 3.5, 0), nodes);
	ASSERT_TRUE(nodes.empty());
}

GTEST_TEST(AABBNode, getNodesInSegment) {
	float min[3] = {0.f, 0.f, 0.f};
	float max[3] = {1.f, 2.f, 0.f};
	Common::AABBNode a = Common::AABBNode(min, max);

	std::vector<Common::AABBNode *> nodes;
	a.getNodesInSegment(glm::vec3(-0.5, 0.5, 0.f), glm::vec3(1.5, 0.5, 0.f), nodes);
	ASSERT_TRUE(nodes.size() == 1);

	nodes.clear();
	a.getNodesInSegment(glm::vec3(-0.5, -0.5, 0.f), glm::vec3(1.5, -0.5, 0.f), nodes);
	ASSERT_TRUE(nodes.empty());
}

GTEST_TEST(AABBNode, adjustChildrenProperty) {
	float min[3] = {0.f, 0.f, 0.f};
	float max[3] = {1.f, 2.f, 0.f};
	Common::AABBNode a = Common::AABBNode(min, max);
	Common::AABBNode *d = new Common::AABBNode(min, max, 1);
	Common::AABBNode *e = new Common::AABBNode(min, max, 2);
	a.setChildren(d, e);

	a.adjustChildrenProperty(3);
	ASSERT_EQ(d->getProperty(), 4);
	ASSERT_EQ(e->getProperty(), 5);
}

