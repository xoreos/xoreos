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
 *  Geometric algorithms helpers.
 */

#ifndef COMMON_GEOMETRY_H
#define COMMON_GEOMETRY_H

#include <vector>

#include "external/glm/vec2.hpp"
#include "external/glm/vec3.hpp"
#include "external/glm/geometric.hpp"
#include "external/glm/gtx/normal.hpp"

#include "src/common/types.h"
#include "src/common/maths.h"
#include "src/common/util.h"

namespace Common {

static inline bool intersectBoxes3D(const glm::vec3 &minBoxA, const glm::vec3 &maxBoxA,
                                    const glm::vec3 &minBoxB, const glm::vec3 &maxBoxB) {
	return (minBoxA[0] <= maxBoxB[0] && maxBoxA[0] >= minBoxB[0])
	        && (minBoxA[1] <= maxBoxB[1] && maxBoxA[1] >= minBoxB[1])
	        && (minBoxA[2] <= maxBoxB[2] && maxBoxA[2] >= minBoxB[2]);
}

static inline bool intersectBoxes3D(const glm::vec2 &minBoxA, const glm::vec2 &maxBoxA,
                                    const glm::vec2 &minBoxB, const glm::vec2 &maxBoxB) {
	const glm::vec3 minA(minBoxA[0], minBoxA[1], FLT_MIN);
	const glm::vec3 maxA(maxBoxA[0], maxBoxA[1], FLT_MAX);
	const glm::vec3 minB(minBoxB[0], minBoxB[1], FLT_MIN);
	const glm::vec3 maxB(maxBoxB[0], maxBoxB[1], FLT_MAX);
	return intersectBoxes3D(minA, maxA, minB, maxB);
}

static inline bool intersectBoxPoint2D(const glm::vec2 &minBox, const glm::vec2 &maxBox, const glm::vec2 &point) {
	return point[0] >= minBox[0] && point[0] <= maxBox[0] && point[1] >= minBox[1] && point[1] <= maxBox[1];
}

static inline bool intersectSegmentPoint2D(glm::vec2 start, glm::vec2 end, glm::vec2 point) {
	// Check first if they are collinear.
	if (glm::cross(glm::vec3(end - start, 0.f), glm::vec3(point - start, 0.f))[2] < 0.0001) {
		// Check component by component.
		if (start[0] != end[0]) {
			if (start[0] <= point[0] && point[0] <= end[0])
				return true;
			if (start[0] >= point[0] && point[0] >= end[0])
				return true;
		} else {
			if (start[1] <= point[1] && point[1] <= end[1])
				return true;
			if (start[1] >= point[1] && point[1] >= end[1])
				return true;
		}
	}
	return false;
}

/** Find the intersection between two segments. */
static inline uint32_t intersectSegments2D(const glm::vec2 &startA, const glm::vec2 &endA,
                                         const glm::vec2 &startB, const glm::vec2 &endB,
                                         glm::vec2 &intersectA, glm::vec2 &intersectB) {
	// From http://geomalgorithms.com/a05-_intersect-1.html.
	const glm::vec3 u = glm::vec3(endA - startA, 0.f);
	const glm::vec3 v = glm::vec3(endB - startB, 0.f);
	const glm::vec3 w = glm::vec3(startA - startB, 0.f);
	const float     D = glm::cross(u, v)[2];

	// Test parallelism (includes either being a point).
	if (fabs(D) < 0.00001) {
		// Test collinearity.
		if (glm::cross(u,w)[2] != 0 || glm::cross(v,w)[2] != 0)  {
			return 0;
		}
		// They are collinear or degenerate.
		// Check if they are degenerate points.
		const float du = glm::dot(u,u);
		const float dv = glm::dot(v,v);
		// Are segments points?
		if (du == 0.f && dv == 0.f) {
			// Are they distinct?
			if (startA !=  startB)
				return 0;
			intersectA = startA;
			return 1;
		}
		// Check if segment A is a single point.
		if (du == 0) {
			// Is it in segment B?
			if (intersectSegmentPoint2D(startB, endB, startA) == 0)
				return 0;
			intersectA = startA;
			return 1;
		}
		// Check if segment B is a single point.
		if (dv == 0) {
			// Is it in segment A?
			if (intersectSegmentPoint2D(startA, endA, startB) == 0)
				return 0;
			intersectA = startB;
			return 1;
		}
		// Are they overlaping?
		float t0, t1;
		glm::vec2 w2 = endA - startB;
		if (v[0] != 0) {
			t0 = w[0] / v[0];
			t1 = w2[0] / v[0];
		} else {
			t0 = w[1] / v[1];
			t1 = w2[1] / v[1];
		}
		// We need to have t1 > than t0.
		if (t0 > t1) {
			float t = t0; t0 = t1; t1 = t;
		}
		if (t0 > 1 || t1 < 0) {
			// They don't overlap.
			return 0;
		}
		t0 = t0 < 0 ? 0 : t0;
		t1 = t1 > 1 ? 1 : t1;

		if (t0 == t1) {
			// Intersection is a single point.
			intersectA = startB + glm::vec2(v * t0);
			return 1;
		}

		// They overlap in a valid subsegment.
		intersectA = startB + glm::vec2(v * t0);
		intersectB = startB + glm::vec2(v * t1);
		return 2;
	}

	// The segments are skew and may intersect in a point.
	// Get the intersect parameter for segment A.
	const float sI = glm::cross(v,w)[2] / D;
	if (sI < 0 || sI > 1)
		return 0;

	// Get the intersect parameter for segment B.
	const float tI = glm::cross(u,w)[2] / D;
	if (tI < 0 || tI > 1)
		return 0;

	intersectA = startA + glm::vec2(u * sI);
	return 1;
}

static inline bool intersectBoxSegment2D(const glm::vec2 &minBox, const glm::vec2 &maxBox,
                                         const glm::vec2 &startSeg, const glm::vec2 &endSeg) {
	// Check if an end point is inside the box.
	if (intersectBoxPoint2D(minBox, maxBox, startSeg)
	    || intersectBoxPoint2D(minBox, maxBox, endSeg)) {
		return true;
	}

	// Check each side of the box.
	glm::vec2 iA, iB;
	if (intersectSegments2D(minBox, glm::vec2(maxBox[0], minBox[1]),
	                        startSeg, endSeg, iA, iB))
		return true;

	if (intersectSegments2D(minBox, glm::vec2(minBox[0], maxBox[1]),
	                        startSeg, endSeg, iA, iB))
		return true;

	if (intersectSegments2D(maxBox, glm::vec2(maxBox[0], minBox[1]),
	                        startSeg, endSeg, iA, iB))
		return true;

	if (intersectSegments2D(maxBox, glm::vec2(minBox[0], maxBox[1]),
	                        startSeg, endSeg, iA, iB))
		return true;

	return false;
}

static inline bool intersectBoxTriangle3D(const glm::vec3 &min, const glm::vec3 &max,
                                          const glm::vec3 &vertA, const glm::vec3 &vertB,
                                          const glm::vec3 &vertC) {
	// Use the separating axis theorem.
	float triangleMin;
	float triangleMax;
	float boxMin, boxMax;

	auto projectOnAxis = [] (std::vector<glm::vec3> &points, const glm::vec3 &axis,
	                         float &minV, float &maxV) -> void {
		minV = FLT_MAX;
		maxV = -FLT_MAX;
		for (auto p = points.begin(); p != points.end(); ++p) {
			const float val = glm::dot(*p, axis);
			if (val < minV)
				minV = val;
			if (val > maxV)
				maxV = val;
		}
	};

	// Test the box normals (x, y and z-axis).
	std::vector<glm::vec3> boxNormals = {glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f),
	                                     glm::vec3(0.f, 0.f, 1.f)};
	std::vector<glm::vec3> triangle = {vertA, vertB, vertC};
	std::vector<glm::vec3> box = {min, max};

	for (uint8_t i = 0; i < 3; ++i) {
		projectOnAxis(triangle, boxNormals[i], triangleMin, triangleMax);
		if (triangleMax < min[i] || triangleMin > max[i])
			return false;
	}

	// Test the triangle normal
	const glm::vec3 triangleNormal = glm::triangleNormal(vertA, vertB, vertC);
	const float triangleOffset = glm::dot(triangleNormal, vertA);

	projectOnAxis(box, triangleNormal, boxMin, boxMax);
	if (boxMax < triangleOffset || boxMin > triangleOffset)
		return false;

	// Test the nine edge cross-products.
	std::vector<glm::vec3> triangleEdges = {vertA - vertB, vertB - vertC, vertC - vertA};

	for (uint8_t i = 0; i < 3; ++i) {
		for (uint8_t j = 0; j < 3; ++j)	{
			// The box normals are the same as it's edge tangents.
			const glm::vec3 axis = glm::cross(triangleEdges[i], boxNormals[j]);
			projectOnAxis(box, axis, boxMin, boxMax);
			projectOnAxis(triangle, axis, triangleMin, triangleMax);
			if (boxMax < triangleMin || boxMin > triangleMax)
				return false;
		}
	}

	// No separating axis found.
	return true;
}

static inline bool intersectTrianglePoint2D(const glm::vec2 &point,
                                            const glm::vec2 &vA, const glm::vec2 &vB, const glm::vec2 &vC) {
	// Use Barycentric Technique.
	const float epsilon = 0.00001;
	const glm::vec2 v0 = vC - vA;
	const glm::vec2 v1 = vB - vA;
	const glm::vec2 v2 = point - vA;

	const float dot00 = glm::dot(v0, v0);
	const float dot01 = glm::dot(v0, v1);
	const float dot02 = glm::dot(v0, v2);
	const float dot11 = glm::dot(v1, v1);
	const float dot12 = glm::dot(v1, v2);

	// Compute barycentric coordinates
	const float denom = 1.f / (dot00 * dot11 - dot01 * dot01);
	const float u = (dot11 * dot02 - dot01 * dot12) * denom;
	const float v = (dot00 * dot12 - dot01 * dot02) * denom;

	// Check if point is in triangle
	return (u + epsilon >= 0) && (v + epsilon >= 0) && (u + v - epsilon < 1);
}

static inline bool intersectBoxTriangle2D(const glm::vec2 &min, const glm::vec2 &max,
                                          const glm::vec2 &vA, const glm::vec2 &vB, const glm::vec2 &vC) {
	// First check if one vertex or the whole triangle is inside the box.
	if (intersectBoxPoint2D(min, max, vA))
		return true;
	if (intersectBoxPoint2D(min, max, vB))
		return true;
	if (intersectBoxPoint2D(min, max, vC))
		return true;

	// Second check if one edge of the vertex intersect the box.
	if (intersectBoxSegment2D(min, max, vA, vB))
		return true;
	if (intersectBoxSegment2D(min, max, vB, vC))
		return true;
	if (intersectBoxSegment2D(min, max, vC, vA))
		return true;

	// Finally check if the box is inside the triangle.
	if (intersectTrianglePoint2D(min, vA, vB, vC))
		return true;
	if (intersectTrianglePoint2D(max, vA, vB, vC))
		return true;
	const glm::vec2 topLeft(min[0], max[1]);
	if (intersectTrianglePoint2D(topLeft, vA, vB, vC))
		return true;
	const glm::vec2 bottomRight(max[0], min[1]);
	if (intersectTrianglePoint2D(bottomRight, vA, vB, vC))
		return true;

	return false;
}

static inline bool intersectBoxTriangle2D(const glm::vec3 &min, const glm::vec3 &max,
                                          const glm::vec3 &vA, const glm::vec3 &vB, const glm::vec3 &vC) {
	return intersectBoxTriangle2D(glm::vec2(min), glm::vec2(max),
	                              glm::vec2(vA), glm::vec2(vB), glm::vec2(vC));
}

static inline bool intersectTriangleSegment2D(const glm::vec2 &vA, const glm::vec2 &vB, const glm::vec2 &vC,
                                              const glm::vec2 &startSeg, const glm::vec2 &endSeg) {
	// First check if an edge crosses the segment.
	glm::vec2 iA, iB;
	if (intersectSegments2D(vA, vB, startSeg, endSeg, iA, iB))
		return true;
	if (intersectSegments2D(vA, vC, startSeg, endSeg, iA, iB))
		return true;
	if (intersectSegments2D(vC, vB, startSeg, endSeg, iA, iB))
		return true;

	// Second check if the segment is within the triangle.
	if (intersectTrianglePoint2D(startSeg, vA, vB, vC))
		return true;

	return false;
}

}

#endif // COMMON_GEOMETRY_H
