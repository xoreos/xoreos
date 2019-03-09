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

#include "external/glm/gtc/type_ptr.hpp"
#include "external/glm/gtc/matrix_transform.hpp"

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

Animation::Animation() : _length(0.0f), _transtime(0.0f) {

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

void Animation::update(Model *model,
                       float UNUSED(lastFrame),
                       float nextFrame,
                       const std::vector<ModelNode *> &modelNodeMap) {
	// TODO: Also need to fire off associated events
	//       for event in _events event->fire()


	float scale = model->getAnimationScale(_name);
	for (NodeList::iterator n = nodeList.begin(); n != nodeList.end(); ++n) {
		ModelNode *animNode = (*n)->_nodedata;
		ModelNode *target = modelNodeMap[animNode->_nodeNumber];
		if (!target)
			continue;

		// Update position and orientation based on time
		if (!animNode->_positionFrames.empty()) {
			glm::vec3 pos(interpolatePosition(animNode, nextFrame));

			if (model->arePositionFramesRelative())
				pos += target->getBasePosition();

			pos *= scale;

			target->setBufferedPosition(pos.x, pos.y, pos.z);
		}

		if (!animNode->_orientationFrames.empty()) {
			glm::quat ori(interpolateOrientation(animNode, nextFrame));
			target->setBufferedOrientation(ori.x, ori.y, ori.z, Common::rad2deg(acosf(ori.w) * 2.0f));
		}
	}
}

void Animation::addAnimNode(AnimNode *node) {
	nodeList.push_back(node);
	nodeMap.insert(std::make_pair(node->getName(), node));
}

bool Animation::hasNode(const Common::UString &node) const {
	return (nodeMap.find(node) != nodeMap.end());
}

ModelNode *Animation::getNode(const Common::UString &node) {
	NodeMap::iterator n = nodeMap.find(node);
	if (n == nodeMap.end())
		return 0;

	return n->second->_nodedata;
}

const ModelNode *Animation::getNode(const Common::UString &node) const {
	NodeMap::const_iterator n = nodeMap.find(node);
	if (n == nodeMap.end())
		return 0;

	return n->second->_nodedata;
}

const std::list<AnimNode *> &Animation::getNodes() const {
	return nodeList;
}

/** Return the dot product of two quaternions. */
static float dotQuaternion(float x1, float y1, float z1, float q1,
                           float x2, float y2, float z2, float q2) {

	return x1 * x2 + y1 * y2 + z1 * z2 + q1 * q2;
}

/** Normalize a quaternion. */
static void normQuaternion(float  xIn , float  yIn , float  zIn , float  qIn,
                           float &xOut, float &yOut, float &zOut, float &qOut) {

	const float magnitude = sqrt(dotQuaternion(xIn, yIn, zIn, qIn, xIn, yIn, zIn, qIn));

	xOut = xIn / magnitude;
	yOut = yIn / magnitude;
	zOut = zIn / magnitude;
	qOut = qIn / magnitude;
}

glm::vec3 Animation::interpolatePosition(ModelNode *animNode, float time) const {
	// If only one keyframe, don't interpolate, just set the only position
	if (animNode->_positionFrames.size() == 1) {
		const PositionKeyFrame &pos = animNode->_positionFrames[0];
		return glm::vec3(pos.x, pos.y, pos.z);
	}

	size_t lastFrame = 0;
	for (size_t i = 0; i < animNode->_positionFrames.size(); i++) {
		const PositionKeyFrame &pos = animNode->_positionFrames[i];
		if (pos.time >= time)
			break;

		lastFrame = i;
	}

	const PositionKeyFrame &last = animNode->_positionFrames[lastFrame];
	if (lastFrame + 1 >= animNode->_positionFrames.size() || last.time >= time)
		return glm::vec3(last.x, last.y, last.z);

	const PositionKeyFrame &next = animNode->_positionFrames[lastFrame + 1];

	const float f = (time - last.time) / (next.time - last.time);
	const float x = f * next.x + (1.0f - f) * last.x;
	const float y = f * next.y + (1.0f - f) * last.y;
	const float z = f * next.z + (1.0f - f) * last.z;

	return glm::vec3(x, y, z);
}

glm::quat Animation::interpolateOrientation(ModelNode *animNode, float time) const {
	// If only one keyframe, don't interpolate just set the only orientation
	if (animNode->_orientationFrames.size() == 1) {
		const QuaternionKeyFrame &ori = animNode->_orientationFrames[0];
		return glm::quat(ori.q, ori.x, ori.y, ori.z);
	}

	size_t lastFrame = 0;
	for (size_t i = 0; i < animNode->_orientationFrames.size(); i++) {
		const QuaternionKeyFrame &ori = animNode->_orientationFrames[i];
		if (ori.time >= time)
			break;

		lastFrame = i;
	}

	const QuaternionKeyFrame &last = animNode->_orientationFrames[lastFrame];
	if (lastFrame + 1 >= animNode->_orientationFrames.size() || last.time >= time) {
		return glm::quat(last.q, last.x, last.y, last.z);
	}

	const QuaternionKeyFrame &next = animNode->_orientationFrames[lastFrame + 1];

	const float f = (time - last.time) / (next.time - last.time);

	/* If the angle is > 90°, we need to flip the direction of one quaternion to
	   get a smooth transition instead of wild jumps. */
	const float angle = acos(dotQuaternion(last.x, last.y, last.z, last.q, next.x, next.y, next.z, next.q));
	const float dir   = (angle >= (M_PI / 2)) ? -1.0f : 1.0f;

	float x = f * dir * next.x + (1.0f - f) * last.x;
	float y = f * dir * next.y + (1.0f - f) * last.y;
	float z = f * dir * next.z + (1.0f - f) * last.z;
	float q = f * dir * next.q + (1.0f - f) * last.q;

	// Normalize the result for slightly better results
	normQuaternion(x, y, z, q, x, y, z, q);

	return glm::quat(q, x, y, z);
}

} // End of namespace Aurora

} // End of namespace Graphics
