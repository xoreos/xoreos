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
 *  A node within a 3D model.
 */

#include "src/common/util.h"
#include "src/common/maths.h"

#include "src/graphics/aurora/modelnode.h"
#include "src/graphics/aurora/animnode.h"
#include "src/graphics/aurora/animation.h"
#include "src/graphics/aurora/model.h"

namespace Graphics {

namespace Aurora {


AnimNode::AnimNode(ModelNode *modelnode) :
	_parent(0) {
	// Actual data is loaded as a generic modelnode
	_nodedata = modelnode;
	if (modelnode)
		_name = modelnode->getName();
}

AnimNode::~AnimNode() {
}

AnimNode *AnimNode::getParent() {
	return _parent;
}

const AnimNode *AnimNode::getParent() const {
	return _parent;
}

void AnimNode::setParent(AnimNode *parent) {
	_parent = parent;
}

const Common::UString &AnimNode::getName() const {
	return _name;
}

ModelNode *AnimNode::getNodeData() {
	return _nodedata;
}

} // End of namespace Aurora

} // End of namespace Graphics
