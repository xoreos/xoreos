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


#include "src/common/geometry.h"
#include "src/common/util.h"
#include "src/common/aabbnode.h"

namespace Common {

AABBNode::AABBNode(float min[3], float max[3], int32_t property) : _property(property) {
	_min[0] = min[0];
	_min[1] = min[1];
	_min[2] = min[2];

	_max[0] = max[0];
	_max[1] = max[1];
	_max[2] = max[2];

	_leftChild = 0;
	_rightChild = 0;
	_parent = 0;
	_empty = false;
}

AABBNode::~AABBNode() {
	delete _leftChild;
	delete _rightChild;
}

bool AABBNode::hasChildren() const {
	// Assume that there are always two children.
	return _leftChild != 0;
}

bool AABBNode::hasParent() const {
	return _parent != 0;
}

void AABBNode::setChildren(AABBNode *leftChild, AABBNode *rightChild) {
	if (leftChild == 0 || rightChild == 0)
		error("AABB must have two or no child");

	_leftChild = leftChild;
	_leftChild->setParent(this);
	_rightChild = rightChild;
	_rightChild->setParent(this);
}

void AABBNode::setParent(AABBNode *parent) {
	_parent = parent;
}

void AABBNode::replaceBranch(AABBNode *branch) {
	// If there is no parent, there is no branch to replace with.
	if (!_parent || !branch)
		return;

	if (_parent->_leftChild == this) {
		_parent->_leftChild = branch;
		branch->setParent(_parent);
		return;
	}

	if (_parent->_rightChild == this) {
		_parent->_rightChild = branch;
		branch->setParent(_parent);
		return;
	}
}

void AABBNode::rotate(float angle, float x, float y, float z) {
	if (fmod(angle, 90.f) != 0.f)
		error("AABB can only rotate from one axis to the other");

	BoundingBox::rotate(angle, x, y, z);

	if (!hasChildren())
		return;

	_leftChild->rotate(angle, x, y, z);
	_rightChild->rotate(angle, x, y, z);
}

void AABBNode::setOrientation(uint8_t orientation) {
	rotate((float) orientation * 90.f, 0.f, 0.f, 1.f);
}

void AABBNode::translate(float x, float y, float z) {
	BoundingBox::translate(x, y, z);

	if (!hasChildren())
		return;

	_leftChild->translate(x, y, z);
	_rightChild->translate(x, y, z);
}

void AABBNode::scale(float x, float y, float z) {
	BoundingBox::scale(x, y, z);

	if (!hasChildren())
		return;

	_leftChild->scale(x, y, z);
	_rightChild->scale(x, y, z);
}

void AABBNode::absolutize() {
	BoundingBox::absolutize();

	if (!hasChildren())
		return;

	_leftChild->absolutize();
	_rightChild->absolutize();
}

void AABBNode::getNodes(float x1, float y1, float z1, float x2, float y2, float z2, std::vector<AABBNode *> &nodes) {
	if (!isIn(x1, y1, z1, x2, y2, z2))
		return;

	if (!hasChildren()) {
		nodes.push_back(this);
		return;
	}

	_leftChild->getNodes(x1, y1, z1, x2, y2, z2, nodes);
	_rightChild->getNodes(x1, y1, z1, x2, y2, z2, nodes);
}

int32_t AABBNode::getProperty() const {
	return _property;
}

void AABBNode::adjustChildrenProperty(int32_t adjust) {
	if (!hasChildren()) {
		_property += adjust;
		return;
	}

	_leftChild->adjustChildrenProperty(adjust);
	_rightChild->adjustChildrenProperty(adjust);
}

void AABBNode::surroundParent() {
	if (!_parent)
		return;

	bool changed = false;

	float leftMin[3], leftMax[3], rightMin[3], rightMax[3], min[3], max[3];
	_parent->_leftChild->getMin(leftMin[0], leftMin[1], leftMin[2]);
	_parent->_leftChild->getMax(leftMax[0], leftMax[1], leftMax[2]);
	_parent->_rightChild->getMin(rightMin[0], rightMin[1], rightMin[2]);
	_parent->_rightChild->getMax(rightMax[0], rightMax[1], rightMax[2]);
	_parent->getMin(min[0], min[1], min[2]);
	_parent->getMax(max[0], max[1], max[2]);

	for (uint32_t i = 0; i < 3; ++i) {
		if (min[i] > leftMin[i] || min[i] > rightMin[i]) {
			_parent->_min[i] = MIN(leftMin[i], rightMin[i]);
			changed = true;
		}

		if (max[i] < leftMax[i] || max[i] < rightMax[i]) {
			_parent->_max[i] = MAX(leftMax[i], rightMax[i]);
			changed = true;
		}
	}

	if (changed) {
		_parent->surroundParent();
	}
}

void AABBNode::getNodes(float x, float y, std::vector<AABBNode *> &nodes) {
	if (!isIn(x, y))
		return;

	if (!hasChildren()) {
		nodes.push_back(this);
		return;
	}

	_leftChild->getNodes(x, y, nodes);
	_rightChild->getNodes(x, y, nodes);
}

void AABBNode::getNodesInAABox(glm::vec3 min, glm::vec3 max, std::vector<AABBNode *> &nodes) {
	const glm::vec3 cMin(_min[0], _min[1], _min[2]);
	const glm::vec3 cMax(_max[0], _max[1], _max[2]);

	if (!intersectBoxes3D(min, max, cMin, cMax))
		return;

	if (!hasChildren()) {
		nodes.push_back(this);
		return;
	}

	_leftChild->getNodesInAABox(min, max, nodes);
	_rightChild->getNodesInAABox(min, max, nodes);
}

void AABBNode::getNodesInAABox(glm::vec2 min, glm::vec2 max, std::vector<AABBNode *> &nodes) {
	const glm::vec2 cMin(_min[0], _min[1]);
	const glm::vec2 cMax(_max[0], _max[1]);

	if (!intersectBoxes3D(min, max, cMin, cMax))
		return;

	if (!hasChildren()) {
		nodes.push_back(this);
		return;
	}

	_leftChild->getNodesInAABox(min, max, nodes);
	_rightChild->getNodesInAABox(min, max, nodes);
}

void AABBNode::getNodesInSegment(glm::vec3 start, glm::vec3 end, std::vector<AABBNode *> &nodes) {
	const glm::vec2 cMin(_min[0], _min[1]);
	const glm::vec2 cMax(_max[0], _max[1]);
	if (!intersectBoxSegment2D(cMin, cMax, start, end))
		return;

	if (!hasChildren()) {
		nodes.push_back(this);
		return;
	}

	_leftChild->getNodesInSegment(start, end, nodes);
	_rightChild->getNodesInSegment(start, end, nodes);
}

} // End of namespace Common
