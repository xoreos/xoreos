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
 *  An axis-aligned bounding box node.
 */

#ifndef COMMON_AABBNODE_H
#define COMMON_AABBNODE_H

#include <vector>

#include "src/common/types.h"
#include "src/common/boundingbox.h"

namespace Common {

class AABBNode : public BoundingBox {
public:
	/** Construct an axis-align bounding box.
	 *
	 *  @param min       Minimal values of the bounding box.
	 *  @param max       Maximal values of the bounding box.
	 *  @param property  An integer value assigned to AABB.
	 */
	AABBNode(float min[3], float max[3], int32_t property = -1);
	~AABBNode();

	/** Has the AABB children? */
	bool hasChildren() const;
	//* Has the AABB a parent? */
	bool hasParent() const;
	/** Set left and right children to the AABB. */
	void setChildren(AABBNode *leftChild, AABBNode *rightChild);
	/** Set a parent to the node. */
	void setParent(AABBNode *parent);
	/** Replace this node by an other branch. */
	void replaceBranch(AABBNode *branch);

	/** Rotate the AABB and its children. Must be a 90° multiple. */
	void rotate(float angle, float x, float y, float z);
	/** Change orientation around the z axis. */
	void setOrientation(uint8_t orientation);
	/** Translate the AABB and its children. */
	void translate(float x, float y, float z);
	/** Scale the AABB and its children. */
	void scale(float x, float y, float z);
	/** Apply the origin transformations directly to the coordinates and its children. */
	void absolutize();

	/** Get the nodes that go through a given segment. */
	void getNodes(float x1, float y1, float z1, float x2, float y2, float z2, std::vector<AABBNode *> &nodes);
	/** Get the nodes at a given point in the XY plane. */
	void getNodes(float x, float y, std::vector<AABBNode *> &nodes);
	/** Get the nodes that intersect a given axis-align box. */
	void getNodesInAABox(glm::vec3 min, glm::vec3 max, std::vector<AABBNode *> &nodes);
	/** Get the nodes that intersect a given axis-align box in the xy plan. */
	void getNodesInAABox(glm::vec2 min, glm::vec2 max, std::vector<AABBNode *> &nodes);
	/** Get the nodes that intersect a given segment in the XY plane. */
	void getNodesInSegment(glm::vec3 start, glm::vec3 end, std::vector<AABBNode *> &nodes);
	/** Get the property of the AABB. */
	int32_t getProperty() const;
	/** Add a given value to the leaves nodes. */
	void adjustChildrenProperty(int32_t adjust);
	/** Ensure the parent surrounds the node. */
	void surroundParent();

private:
	AABBNode *_parent;     ///< The parent node.
	AABBNode *_leftChild;  ///< Left child.
	AABBNode *_rightChild; ///< Right child.
	int32_t _property;       ///< An arbitrary value of the AABB.
};

} // End of namespace Common

#endif // COMMON_AABBNODE_H
