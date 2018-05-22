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

#ifndef COMMON_INTERSECT_H
#define COMMON_INTERSECT_H

#include "glm/vec3.hpp"

namespace Common {

/** Find the 3D intersection of a ray with a triangle. Code ported
 *  from http://geomalgorithms.com/a06-_intersect-2.html.
 *
 *  @param p0 Ray starting point
 *  @param p1 Ray endpoint
 *  @param v0 First vertex of a triangle
 *  @param v1 Second vertex of a triangle
 *  @param v2 Third vertex of a triangle
 *  @param I Intersection point (when it exists)
 *
 *  @return -1 = triangle is degenerate (a segment or point)
 *           0 = disjoint (no intersect)
 *           1 = intersect in unique point I1
 *           2 = are in the same plane
 */
int intersectRayTriangle(const glm::vec3 &p0,
                         const glm::vec3 &p1,
                         const glm::vec3 &v0,
                         const glm::vec3 &v1,
                         const glm::vec3 &v2,
                         glm::vec3 &I);

} // End of namespace Common

#endif // COMMON_INTERSECT_H
