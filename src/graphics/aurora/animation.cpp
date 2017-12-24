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

void Animation::update(Model *model, float UNUSED(lastFrame), float nextFrame) {
	// TODO: Also need to fire off associated events
	//       for event in _events event->fire()


	float scale = model->getAnimationScale(_name);
	for (NodeList::iterator n = nodeList.begin(); n != nodeList.end(); ++n) {
		ModelNode *animNode = (*n)->_nodedata;
		ModelNode *target = model->_animationNodeMap[animNode->_nodeNumber];
		if (!target)
			continue;

		// Update position and orientation based on time
		if (!animNode->_positionFrames.empty())
			interpolatePosition(animNode, target, nextFrame, scale, model->_skinned);
		if (!animNode->_orientationFrames.empty())
			interpolateOrientation(animNode, target, nextFrame);
	}

	if (model->_skinned)
		updateSkinnedModel(model);
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

void Animation::interpolatePosition(ModelNode *animNode, ModelNode *target, float time, float scale,
                                    bool relative) const {
	float dx = 0;
	float dy = 0;
	float dz = 0;
	if (relative) {
		const PositionKeyFrame &pos = target->_positionFrames[0];
		dx = pos.x;
		dy = pos.y;
		dz = pos.z;
	}

	// If only one keyframe, don't interpolate, just set the only position
	if (animNode->_positionFrames.size() == 1) {
		const PositionKeyFrame &pos = animNode->_positionFrames[0];
		target->setPosition((dx + pos.x) * scale,
		                    (dy + pos.y) * scale,
		                    (dz + pos.z) * scale);
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
	if (lastFrame + 1 >= animNode->_positionFrames.size() || last.time >= time) {
		target->setPosition((dx + last.x) * scale,
		                    (dy + last.y) * scale,
		                    (dz + last.z) * scale);
		return;
	}

	const PositionKeyFrame &next = animNode->_positionFrames[lastFrame + 1];

	const float f = (time - last.time) / (next.time - last.time);
	const float x = f * next.x + (1.0f - f) * last.x;
	const float y = f * next.y + (1.0f - f) * last.y;
	const float z = f * next.z + (1.0f - f) * last.z;
	target->setPosition((dx + x) * scale, (dy + y) * scale, (dz + z) * scale);
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
	if (lastFrame + 1 >= animNode->_orientationFrames.size() || last.time >= time) {
		target->setOrientation(last.x, last.y, last.z, Common::rad2deg(acos(last.q) * 2.0));
		return;
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

	target->setOrientation(x, y, z, Common::rad2deg(acos(q) * 2.0));
}

void Animation::updateSkinnedModel(Model *model) {
	const std::list<ModelNode *> &nodes = model->getNodes();
	for (std::list<ModelNode *>::const_iterator it = nodes.begin(); it != nodes.end(); ++it) {
		ModelNode *node = *it;
		if (!node->_mesh || !node->_mesh->skin)
			continue;

		Common::Matrix4x4 transform;
		transform.translate(node->_position[0], node->_position[1], node->_position[2]);
		transform.rotate(node->_orientation[3],
		                 node->_orientation[0],
		                 node->_orientation[1],
		                 node->_orientation[2]);
		Common::Matrix4x4 invTransform = transform.getInverse();

		ModelNode::Skin *skin = node->_mesh->skin;

		_invBindPoseMatrices.reserve(16 * skin->boneMappingCount);
		float *invBindPoseArr = &_invBindPoseMatrices[0];

		_boneTransMatrices.reserve(16 * skin->boneMappingCount);
		float *boneTransArr = &_boneTransMatrices[0];

		for (uint16 i = 0; i < skin->boneMappingCount; ++i) {
			int index = static_cast<int>(skin->boneMapping[i]);
			if (index != -1) {
				uint32 off = index * 16;
				computeNodeTransform(skin->boneNodeMap[index], invBindPoseArr + off,
				                     boneTransArr + off);
			}
		}

		// TODO: Use vertex shader

		ModelNode::MeshData *meshData = node->_mesh->data;
		VertexBuffer &vertexBuffer = meshData->vertexBuffer;
		uint32 vertexCount = vertexBuffer.getCount();
		uint32 stride = vertexBuffer.getSize() / sizeof(float);
		float *v = reinterpret_cast<float *>(vertexBuffer.getData());
		float *iv = &meshData->initialVertexCoords[0];
		float *boneWeights = &skin->boneWeights[0];
		float *boneMappingId = &skin->boneMappingId[0];
		Common::Matrix4x4 invBindPose;
		Common::Matrix4x4 boneTransform;

		for (uint32 i = 0; i < vertexCount; ++i) {
			v[0] = 0;
			v[1] = 0;
			v[2] = 0;
			for (uint8 j = 0; j < 4; ++j) {
				int index = static_cast<int>(boneMappingId[j]);
				if (index != -1) {
					uint32 off = index * 16;
					float rv[3];
					float tv[3];

					transform.multiply(iv, rv);

					invBindPose = invBindPoseArr + off;
					invBindPose.multiply(rv, tv);

					boneTransform = boneTransArr + off;
					boneTransform.multiply(tv, rv);

					invTransform.multiply(rv, tv);

					v[0] += tv[0] * boneWeights[j];
					v[1] += tv[1] * boneWeights[j];
					v[2] += tv[2] * boneWeights[j];
				}
			}
			v += stride;
			iv += 3;
			boneWeights += 4;
			boneMappingId += 4;
		}
	}
}

void Animation::computeNodeTransform(ModelNode *node, float *outInvBindPose, float *outTransform) {
	_nodeChain.clear();
	for (ModelNode *node2 = node; node2; node2 = node2->_parent)
		_nodeChain.push_back(node2);

	Common::Matrix4x4 bindPose;
	Common::Matrix4x4 transform;
	for (int i = _nodeChain.size() - 1; i >= 0; --i) {
		const ModelNode *node2 = _nodeChain[i];
		if (node2->_positionFrames.size() > 0) {
			const PositionKeyFrame &pos = node2->_positionFrames[0];
			bindPose.translate(pos.x, pos.y, pos.z);
		}
		if (node2->_orientationFrames.size() > 0) {
			const QuaternionKeyFrame &ori = node2->_orientationFrames[0];
			bindPose.rotate(Common::rad2deg(acos(ori.q) * 2.0), ori.x, ori.y, ori.z);
		}
		transform.translate(node2->_position[0], node2->_position[1], node2->_position[2]);
		transform.rotate(node2->_orientation[3],
		                 node2->_orientation[0],
		                 node2->_orientation[1],
		                 node2->_orientation[2]);
	}

	memcpy(outInvBindPose, bindPose.getInverse().get(), 16 * sizeof(float));
	memcpy(outTransform, transform.get(), 16 * sizeof(float));
}

} // End of namespace Aurora

} // End of namespace Graphics
