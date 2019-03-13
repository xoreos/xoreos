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
 *  Skeletal animation helper class.
 */

#include "external/glm/gtc/type_ptr.hpp"
#include "external/glm/gtc/matrix_transform.hpp"

#include "src/graphics/aurora/skeletalanimation.h"
#include "src/graphics/aurora/model.h"
#include "src/graphics/aurora/animnode.h"

namespace Graphics {

namespace Aurora {

SkeletalAnimation::SkeletalAnimation(int bonesPerVertex) :
		Animation(),
		_bonesPerVertex(bonesPerVertex) {
}

void SkeletalAnimation::update(Model *model,
                               float lastFrame,
                               float nextFrame,
                               const std::vector<ModelNode *> &modelNodeMap) {

	Animation::update(model, lastFrame, nextFrame, modelNodeMap);
	updateModel(model, lastFrame);
}

void SkeletalAnimation::updateModel(Model *model, float time) {
	if (!model->hasSkinNodes())
		return;

	for (const auto &n : model->getNodes()) {
		if (!n->hasSkinNode())
			continue;

		model->computeNodeTransforms();

		if (GfxMan.isRendererExperimental()) {
			fillBoneTransforms(n);
			continue;
		}

		const std::vector<float> &vertsIn = n->getInitialVertexCoords();
		const std::vector<float> &boneIndices = n->getBoneIndices();
		const std::vector<float> &boneWeights = n->getBoneWeights();
		VertexBuffer *vertexBuffer = n->getMesh()->data->rawMesh->getVertexBuffer();

		transform(n, vertsIn, boneIndices, boneWeights, vertexBuffer);

		n->notifyVertexCoordsBuffered();
	}

	for (const auto &m : model->getAttachedModels()) {
		updateModel(m.second, time);
	}
}

void SkeletalAnimation::fillBoneTransforms(ModelNode *node) {
	ModelNode::Mesh *mesh = node->getMesh();
	std::vector<float> &boneTransforms = mesh->data->rawMesh->getBoneTransforms();
	ModelNode::Skin *skin = mesh->skin;

	for (int i = 0; i < static_cast<int>(skin->boneMappingCount); ++i) {
		int boneIndex = static_cast<int>(mesh->skin->boneMapping[i]);
		if (boneIndex == -1)
			continue;

		float *boneTransformsData = boneTransforms.data() + 16 * boneIndex;
		ModelNode *boneNode = skin->boneNodeMap[boneIndex];
		std::memcpy(boneTransformsData, glm::value_ptr(boneNode->getBoneTransform()), 16 * sizeof(float));
	}
}

void SkeletalAnimation::transform(ModelNode *node,
                                  const std::vector<float> &vertsIn,
                                  const std::vector<float> &boneIndices,
                                  const std::vector<float> &boneWeights,
                                  VertexBuffer *vertexBuffer) {

	const int vertexCount = static_cast<int>(vertsIn.size()) / 3;
	const float *vertsInData = vertsIn.data();

	const float *boneIndicesData = boneIndices.data();
	const float *boneWeightsData = boneWeights.data();
	const int boneStride = _bonesPerVertex;

	float *bufferData = static_cast<float *>(vertexBuffer->getData());
	const int bufferStride = static_cast<int>(vertexBuffer->getVertexDecl()[0].stride) / sizeof(float);

	for (int i = 0; i < vertexCount; ++i) {
		bufferData[0] = 0.0f;
		bufferData[1] = 0.0f;
		bufferData[2] = 0.0f;

		for (int j = 0; j < _bonesPerVertex; ++j) {
			const int boneIndex = static_cast<int>(boneIndicesData[j]);
			if (boneIndex == -1)
				continue;

			const glm::mat4 &boneTransform = node->getBoneNode(boneIndex)->getBoneTransform();
			const float boneWeight = boneWeightsData[j];
			float v0[3], v1[3];

			multiply(vertsInData, node->getAbsoluteBaseTransform(), v0);
			multiply(v0, boneTransform, v1);
			multiply(v1, node->getAbsoluteBaseTransformInverse(), v0);

			bufferData[0] += v0[0] * boneWeight;
			bufferData[1] += v0[1] * boneWeight;
			bufferData[2] += v0[2] * boneWeight;
		}

		vertsInData += 3;
		boneIndicesData += boneStride;
		boneWeightsData += boneStride;
		bufferData += bufferStride;
	}
}

void SkeletalAnimation::multiply(const float *v, const glm::mat4 &m, float *vOut) {
	float x = v[0] * m[0][0] + v[1] * m[1][0] + v[2] * m[2][0] + m[3][0];
	float y = v[0] * m[0][1] + v[1] * m[1][1] + v[2] * m[2][1] + m[3][1];
	float z = v[0] * m[0][2] + v[1] * m[1][2] + v[2] * m[2][2] + m[3][2];
	float w = v[0] * m[0][3] + v[1] * m[1][3] + v[2] * m[2][3] + m[3][3];

	vOut[0] = x / w;
	vOut[1] = y / w;
	vOut[2] = z / w;
}

} // End of namespace Aurora

} // End of namespace Graphics
