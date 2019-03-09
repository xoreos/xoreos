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

#ifndef GRAPHICS_AURORA_SKELETALANIMATION_H
#define GRAPHICS_AURORA_SKELETALANIMATION_H

#include <vector>

#include "external/glm/vec3.hpp"
#include "external/glm/mat4x4.hpp"

#include "src/common/types.h"

#include "src/graphics/aurora/animation.h"

namespace Graphics {

namespace Aurora {

class SkeletalAnimation : public Animation {
public:
	SkeletalAnimation(int bonesPerVertex);

	void update(Model *model,
	            float lastFrame,
	            float nextFrame,
	            const std::vector<ModelNode *> &modelNodeMap);

private:
	int _bonesPerVertex;

	void updateModel(Model *model, float time);
	void fillBoneTransforms(ModelNode *node);

	/** Transform vertex coordinates.
	 *
	 *  @param node        Model node whose vertices are being transformed.
	 *  @param vertsIn     Input array of vertex coordinates.
	 *  @param boneIndices Array of bone indices.
	 *  @param boneWeights Array of bone weights.
	 *  @param vertsOut    Output array of vertex coordinates.
	 */
	void transform(ModelNode *node,
	               const std::vector<float> &vertsIn,
	               const std::vector<float> &boneIndices,
	               const std::vector<float> &boneWeights,
	               std::vector<float> &vertsOut);

	/** Multiply a specified vector by a specified matrix. */
	static void multiply(const float *v, const glm::mat4 &m, float *vOut);
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_SKELETALANIMATION_H