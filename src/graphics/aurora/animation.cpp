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
 *  An animation to be applied to a model.
 */

#include "src/common/readstream.h"
#include "src/common/debug.h"

#include "src/graphics/graphics.h"
#include "src/graphics/camera.h"

#include "src/graphics/aurora/model.h"
#include "src/graphics/aurora/modelnode.h"
#include "src/graphics/aurora/animation.h"
#include "src/graphics/aurora/animnode.h"

using Common::kDebugGraphics;

namespace Graphics {

namespace Aurora {

Animation::Animation() {

}

Animation::~Animation() {
	for (NodeList::iterator n = nodeList.begin(); n != nodeList.end(); ++n)
		delete *n;
}

const Common::UString &Animation::getName() const {
	return _name;
}

void Animation::setName(Common::UString &name) {
	_name = name;
}

void Animation::setLength(float length) {
	_length = length;
}

float Animation::getLength() const {
	return _length;
}

void Animation::setTransTime(float transtime) {
	_transtime = transtime;
}

void Animation::update(Model *model, float UNUSED(lastFrame), float nextFrame) {
	// TODO: Also need to fire off associated events
	//       for event in _events event->fire()


	float scale = model->getAnimationScale(_name);
	for (NodeList::iterator n = nodeList.begin(); n != nodeList.end(); ++n) {
		ModelNode *animNode = (*n)->_nodedata;
		ModelNode *target = model->getNode(animNode->getName());
		if (!target)
			continue;

		// Update position and orientation based on time
		if (!animNode->_positionFrames.empty())
			interpolatePosition(animNode, target, nextFrame, scale);
		if (!animNode->_orientationFrames.empty())
			interpolateOrientation(animNode, target, nextFrame);
	}
}

void Animation::addAnimNode(AnimNode *node) {
	nodeList.push_back(node);
	nodeMap.insert(std::make_pair(node->getName(), node));
}

void Animation::interpolatePosition(ModelNode *animNode, ModelNode *target, float time, float scale) const {
	// If only one keyframe, don't interpolate, just set the only position
	if (animNode->_positionFrames.size() == 1) {
		const PositionKeyFrame &pos = animNode->_positionFrames[0];
		target->setPosition(pos.x * scale, pos.y * scale, pos.z * scale);
		return;
	}

	size_t lastFrame = 0;
	for (size_t i = 0; i < animNode->_positionFrames.size(); i++) {
		const PositionKeyFrame &pos = animNode->_positionFrames[i];
		if (pos.time >= time)
			break;

		lastFrame = i;
	}

	const PositionKeyFrame &last = animNode->_positionFrames[lastFrame];
	if (lastFrame + 1 >= animNode->_positionFrames.size() || last.time == time) {
		target->setPosition(last.x * scale, last.y * scale, last.z * scale);
		return;
	}

	const PositionKeyFrame &next = animNode->_positionFrames[lastFrame + 1];

	const float f = (time - last.time) / (next.time - last.time);
	const float x = f * next.x + (1.0f - f) * last.x;
	const float y = f * next.y + (1.0f - f) * last.y;
	const float z = f * next.z + (1.0f - f) * last.z;
	target->setPosition(x * scale, y * scale, z * scale);
}

void Animation::interpolateOrientation(ModelNode *animNode, ModelNode *target, float time) const {
	// If only one keyframe, don't interpolate just set the only orientation
	if (animNode->_orientationFrames.size() == 1) {
		const QuaternionKeyFrame &ori = animNode->_orientationFrames[0];
		target->setOrientation(ori.x, ori.y, ori.z, Common::rad2deg(acos(ori.q) * 2.0));
		return;
	}

	size_t lastFrame = 0;
	for (size_t i = 0; i < animNode->_orientationFrames.size(); i++) {
		const QuaternionKeyFrame &ori = animNode->_orientationFrames[i];
		if (ori.time >= time)
			break;

		lastFrame = i;
	}

	const QuaternionKeyFrame &last = animNode->_orientationFrames[lastFrame];
	if (lastFrame + 1 >= animNode->_orientationFrames.size() || last.time == time) {
		target->setOrientation(last.x, last.y, last.z, Common::rad2deg(acos(last.q) * 2.0));
	}

	const QuaternionKeyFrame &next = animNode->_orientationFrames[lastFrame + 1];

	const float f = (time - last.time) / (next.time - last.time);
	const float x = f * next.x + (1.0f - f) * last.x;
	const float y = f * next.y + (1.0f - f) * last.y;
	const float z = f * next.z + (1.0f - f) * last.z;
	const float q = f * next.q + (1.0f - f) * last.q;
	target->setOrientation(x, y, z, Common::rad2deg(acos(q) * 2.0));
}

} // End of namespace Aurora

} // End of namespace Graphics
