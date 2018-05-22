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
 *  Class to evaluate walkmesh elevation at given coordinates.
 */

#include "glm/gtc/type_ptr.hpp"

#include "src/common/maths.h"
#include "src/common/util.h"
#include "src/common/intersect.h"

#include "src/engines/aurora/walkeleveval.h"

namespace Engines {

float WalkmeshElevationEvaluator::getElevationAt(const Walkmesh &w, float x, float y) {
	glm::vec3 p0 = glm::vec3(x, y, 1000);
	glm::vec3 p1 = glm::vec3(x, y, -1000);

	const std::vector<uint32> &indicesWalkable = w.getIndicesWalkable();
	size_t faceCount = indicesWalkable.size() / 3;
	const WalkmeshData &walkmeshData = w.getData();
	const float *vertices = walkmeshData.vertices.data();
	uint32 index = 0;

	for (size_t i = 0; i < faceCount; ++i) {
		glm::vec3 v0 = glm::make_vec3(vertices + 3 * indicesWalkable[index + 0]);
		glm::vec3 v1 = glm::make_vec3(vertices + 3 * indicesWalkable[index + 1]);
		glm::vec3 v2 = glm::make_vec3(vertices + 3 * indicesWalkable[index + 2]);
		index += 3;

		glm::vec3 intersection;
		if (Common::intersectRayTriangle(p0, p1, v0, v1, v2, intersection) == 1)
			return intersection.z;
	}

	return FLT_MIN;
}

} // End of namespace Engines
