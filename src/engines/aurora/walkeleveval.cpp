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
 *  Class to evaluate walkmesh elevation at given coordinates. Code ported from
 *  http://geomalgorithms.com/a06-_intersect-2.html
 */

#include "src/common/maths.h"
#include "src/common/util.h"

#include "src/engines/aurora/walkeleveval.h"

namespace Engines {

struct Point {
	float x, y, z;
};

struct Ray {
	Point p0, p1;
};

struct Triangle {
	Point v0, v1, v2;
};

static Point add(const Point &u, const Point &v) {
	Point p;
	p.x = u.x + v.x;
	p.y = u.y + v.y;
	p.z = u.z + v.z;
	return p;
}

static Point subtract(const Point &u, const Point &v) {
	Point p;
	p.x = u.x - v.x;
	p.y = u.y - v.y;
	p.z = u.z - v.z;
	return p;
}

static Point multiply(float scalar, const Point &v) {
	Point p;
	p.x = scalar * v.x;
	p.y = scalar * v.y;
	p.z = scalar * v.z;
	return p;
}

static inline float dot(const Point &u, const Point &v) {
	return u.x * v.x + u.y * v.y + u.z * v.z;
}

static Point cross(const Point &u, const Point &v) {
	Point p;
	p.x = u.y * v.z - u.z * v.y;
	p.y = u.z * v.x - u.x * v.z;
	p.z = u.x * v.y - u.y * v.x;
	return p;
}

static int intersect(const Ray &R, const Triangle &T, Point &I) {
	Point u = subtract(T.v1, T.v0);
	Point v = subtract(T.v2, T.v0);
	Point n = cross(u, v);

	if (n.x == 0 && n.y == 0 && n.z == 0) {
		return -1;
	}

	Point dir = subtract(R.p1, R.p0);
	Point w0 = subtract(R.p0, T.v0);
	float a = -dot(n, w0);
	float b = dot(n, dir);

	if (fabs(b) < 0.00000001) {
		return a == 0 ? 2 : 0;
	}

	float r = a / b;

	if (r < 0)
		return 0;

	I = add(R.p0, multiply(r, dir));

	float uu = dot(u, u);
	float uv = dot(u, v);
	float vv = dot(v, v);
	Point w = subtract(I, T.v0);
	float wu = dot(w, u);
	float wv = dot(w, v);
	float D = uv * uv  - uu * vv;
	float s = (uv * wv - vv * wu) / D;

	if (s < 0 || s > 1)
		return 0;

	float t = (uv * wu - uu * wv) / D;

	if (t < 0 || s + t > 1)
		return 0;

	return 1;
}

float WalkmeshElevationEvaluator::getElevationAt(const Walkmesh &w, float x, float y) {
	Ray ray;

	ray.p0.x = x;
	ray.p0.y = y;
	ray.p0.z = 1000;

	ray.p1.x = x;
	ray.p1.y = y;
	ray.p1.z = -1000;

	const std::vector<uint32> &indicesWalkable = w.getIndicesWalkable();
	size_t faceCount = indicesWalkable.size() / 3;
	const WalkmeshData &walkmeshData = w.getData();
	const float *vertices = walkmeshData.vertices.data();
	uint32 index = 0;

	for (size_t i = 0; i < faceCount; ++i) {
		const float *v0 = vertices + 3 * indicesWalkable[index + 0];
		const float *v1 = vertices + 3 * indicesWalkable[index + 1];
		const float *v2 = vertices + 3 * indicesWalkable[index + 2];
		index += 3;

		Triangle triangle;

		triangle.v0.x = v0[0];
		triangle.v0.y = v0[1];
		triangle.v0.z = v0[2];

		triangle.v1.x = v1[0];
		triangle.v1.y = v1[1];
		triangle.v1.z = v1[2];

		triangle.v2.x = v2[0];
		triangle.v2.y = v2[1];
		triangle.v2.z = v2[2];

		Point intersection;
		if (intersect(ray, triangle, intersection) == 1)
			return intersection.z;
	}

	return FLT_MIN;
}

} // End of namespace Engines
