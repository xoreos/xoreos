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
		if (!animNode->_positionFrames.empty())
			interpolatePosition(animNode, target, nextFrame, scale, model->_positionRelative);
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
		target->setBufferedPosition((dx + pos.x) * scale,
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
		target->setBufferedPosition((dx + last.x) * scale,
		                            (dy + last.y) * scale,
		                            (dz + last.z) * scale);
		return;
	}

	const PositionKeyFrame &next = animNode->_positionFrames[lastFrame + 1];

	const float f = (time - last.time) / (next.time - last.time);
	const float x = f * next.x + (1.0f - f) * last.x;
	const float y = f * next.y + (1.0f - f) * last.y;
	const float z = f * next.z + (1.0f - f) * last.z;

	target->setBufferedPosition((dx + x) * scale,
	                            (dy + y) * scale,
	                            (dz + z) * scale);
}

void Animation::interpolateOrientation(ModelNode *animNode, ModelNode *target, float time) const {
	// If only one keyframe, don't interpolate just set the only orientation
	if (animNode->_orientationFrames.size() == 1) {
		const QuaternionKeyFrame &ori = animNode->_orientationFrames[0];
		target->setBufferedOrientation(ori.x, ori.y, ori.z, Common::rad2deg(acos(ori.q) * 2.0));
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
		target->setBufferedOrientation(last.x, last.y, last.z, Common::rad2deg(acos(last.q) * 2.0));
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

	target->setBufferedOrientation(x, y, z, Common::rad2deg(acos(q) * 2.0));
}

static void multiply(const float *v, const glm::mat4 &m, float *rv) {
	rv[0]   = v[0] * m[0][0] + v[1] * m[1][0] + v[2] * m[2][0] + m[3][0];
	rv[1]   = v[0] * m[0][1] + v[1] * m[1][1] + v[2] * m[2][1] + m[3][1];
	rv[2]   = v[0] * m[0][2] + v[1] * m[1][2] + v[2] * m[2][2] + m[3][2];
	float w = v[0] * m[0][3] + v[1] * m[1][3] + v[2] * m[2][3] + m[3][3];
	rv[0] /= w;
	rv[1] /= w;
	rv[2] /= w;
}

void Animation::updateSkinnedModel(Model *model) {
	const std::list<ModelNode *> &nodes = model->getNodes();
	for (std::list<ModelNode *>::const_iterator it = nodes.begin(); it != nodes.end(); ++it) {
		ModelNode *node = *it;
		if (!node->_mesh || !node->_mesh->skin)
			continue;

		fillBoneTransformsOfModelNode(node);

		if (!GfxMan.isRendererExperimental()) {
			ModelNode::MeshData *meshData = node->_mesh->data;
			VertexBuffer &vertexBuffer = *(meshData->rawMesh->getVertexBuffer());
			uint32 vertexCount = vertexBuffer.getCount();
			float *initialVertexData = meshData->initialVertexCoords.data();

			std::vector<float> &vertexCoordsBuffer = node->_vertexCoordsBuffer;
			vertexCoordsBuffer.resize(3 * vertexCount);
			float *vertexData = vertexCoordsBuffer.data();

			for (uint32 i = 0; i < vertexCount; ++i) {
				applyBoneTransformsToVertex(node, initialVertexData, i, vertexData);
				vertexData += 3;
				initialVertexData += 3;
			}

			node->_vertexCoordsBuffered = true;
		}
	}

	for (std::map<Common::UString, Model *>::iterator m = model->_attachedModels.begin();
			m != model->_attachedModels.end(); ++m) {
		Model *attachedModel = m->second;
		if (attachedModel->_skinned)
			updateSkinnedModel(attachedModel);
	}
}

void Animation::fillBoneTransformsOfModelNode(const ModelNode *node) {
	const Graphics::Aurora::ModelNode::Skin *skin = node->_mesh->skin;
	std::vector<float> &boneTransforms = node->_mesh->data->rawMesh->getBoneTransforms();
	float *boneTransformsData = boneTransforms.data();

	for (uint16 i = 0; i < skin->boneMappingCount; ++i) {
		int index = static_cast<int>(skin->boneMapping[i]);
		if (index != -1) {
			ModelNode *boneNode = skin->boneNodeMap[index];
			boneNode->computeAbsoluteTransform();
			boneNode->computeBoneTransform();

			memcpy(boneTransformsData + 16 * index,
			       glm::value_ptr(boneNode->getBoneTransform()),
			       16 * sizeof(float));
		}
	}
}

inline void Animation::applyBoneTransformsToVertex(
		const ModelNode *node,
		const float *initialVertexData,
		int vertexIndex,
		float *vertexData) {

	vertexData[0] = 0;
	vertexData[1] = 0;
	vertexData[2] = 0;

	const ModelNode::Skin *skin = node->_mesh->skin;
	int offset = 4 * vertexIndex;
	const float *boneWeights = skin->boneWeights.data() + offset;
	const float *boneMappingId = skin->boneMappingId.data() + offset;

	for (uint8 j = 0; j < 4; ++j) {
		int index = static_cast<int>(boneMappingId[j]);
		if (index != -1) {
			float r[3];
			float t[3];
			const glm::mat4 &boneTransform = skin->boneNodeMap[index]->_boneTransform;

			multiply(initialVertexData, node->_bindPose, r);
			multiply(r, boneTransform, t);
			multiply(t, node->_invBindPose, r);

			vertexData[0] += r[0] * boneWeights[j];
			vertexData[1] += r[1] * boneWeights[j];
			vertexData[2] += r[2] * boneWeights[j];
		}
	}
}

} // End of namespace Aurora

} // End of namespace Graphics
