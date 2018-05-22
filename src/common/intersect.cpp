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
 *  Helper functions to test geometric figures for intersection.
 */

#include "glm/detail/func_geometric.hpp"

#include "src/common/intersect.h"

namespace Common {

int intersectRayTriangle(const glm::vec3 &p0,
                         const glm::vec3 &p1,
                         const glm::vec3 &v0,
                         const glm::vec3 &v1,
                         const glm::vec3 &v2,
                         glm::vec3 &I) {
	glm::vec3 u = v1 - v0;
	glm::vec3 v = v2 - v0;
	glm::vec3 n = glm::cross(u, v);

	if (n.x == 0 && n.y == 0 && n.z == 0)
		return -1;

	glm::vec3 dir = p1 - p0;
	glm::vec3 w0 = p0 - v0;
	float a = -glm::dot(n, w0);
	float b = glm::dot(n, dir);

	if (fabs(b) < 0.00000001)
		return a == 0 ? 2 : 0;

	float r = a / b;

	if (r < 0)
		return 0;

	I = p0 + r * dir;

	float uu = glm::dot(u, u);
	float uv = glm::dot(u, v);
	float vv = glm::dot(v, v);
	glm::vec3 w = I - v0;
	float wu = glm::dot(w, u);
	float wv = glm::dot(w, v);
	float D = uv * uv  - uu * vv;
	float s = (uv * wv - vv * wu) / D;

	if (s < 0 || s > 1)
		return 0;

	float t = (uv * wu - uu * wv) / D;

	if (t < 0 || s + t > 1)
		return 0;

	return 1;
}

} // End of namespace Common
