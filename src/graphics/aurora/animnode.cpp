/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file graphics/aurora/modelnode.cpp
 *  A node within a 3D model.
 */

#include "common/util.h"
#include "common/maths.h"

#include "graphics/aurora/modelnode.h"
#include "graphics/aurora/animnode.h"
#include "graphics/aurora/animation.h"
#include "graphics/aurora/model.h"

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

void AnimNode::update(Model *model, float lastFrame, float nextFrame, float scale) {
	if (!_nodedata)
		return;

	ModelNode *target = model->getNode(_name);
	if (!target)
		return;

	// Determine the corresponding keyframes
	float posX, posY, posZ;
	_nodedata->interpolatePosition(nextFrame, posX, posY, posZ);

	float oX, oY, oZ, oA;
	_nodedata->interpolateOrientation(nextFrame, oX, oY, oZ, oA);

	// Update the position/orientation of corresponding modelnode
	target->setPosition(posX * scale, posY * scale, posZ * scale);
	target->setOrientation(oX, oY, oZ, oA);
}

} // End of namespace Aurora

} // End of namespace Graphics
