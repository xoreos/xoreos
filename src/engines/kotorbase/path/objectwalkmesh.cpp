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
 *  WalkmeshObject implementation for KotOR games.
 */

#include "external/glm/gtc/matrix_transform.hpp"
#include "external/glm/gtc/type_ptr.hpp"
#include "external/glm/gtx/intersect.hpp"

#include "src/common/util.h"
#include "src/common/maths.h"
#include "src/common/geometry.h"

#include "src/engines/kotorbase/situated.h"

#include "src/engines/kotorbase/path/walkmeshloader.h"
#include "src/engines/kotorbase/path/objectwalkmesh.h"

namespace Engines {

namespace KotORBase {

ObjectWalkmesh::ObjectWalkmesh(Situated *situated, Aurora::FileType fileType) :
		_fileType(fileType){

	float ori[4];
	float pos[3];
	situated->getOrientation(ori[0], ori[1], ori[2], ori[3]);
	situated->getPosition(pos[0], pos[1], pos[2]);

	Common::UString fileName = situated->getModelName();
	if (fileType == Aurora::kFileTypeDWK)
		fileName += "0";

	load(fileName, ori, pos);
}

void ObjectWalkmesh::load(const Common::UString &resRef, float orientation[4], float position[3]) {
	WalkmeshLoader loader;

	glm::mat4 transform;
	transform = glm::translate(transform, glm::make_vec3(position));
	transform = glm::rotate(transform, Common::deg2rad(orientation[3]), glm::make_vec3(orientation));

	std::vector<uint32_t> faceProperties, adjFaces;
	std::map<uint32_t, uint32_t> adjRooms;
	loader.load(_fileType, resRef, transform,
	            _vertices, _faces, faceProperties, adjFaces, adjRooms);

	computeMinMax();
}

bool ObjectWalkmesh::in(const glm::vec2 &minBox, const glm::vec2 &maxBox) const {
	return Common::intersectBoxes3D(_min, _max, minBox, maxBox);
}

bool ObjectWalkmesh::in(const glm::vec2 &point) const {
	if (!Common::intersectBoxPoint2D(_min, _max, point))
		return false;

	for (size_t f = 0; f < _faces.size() / 3; ++f) {
		const size_t vertAIndex = _faces[f * 3 + 0];
		const glm::vec2 vertA(_vertices[vertAIndex * 3], _vertices[vertAIndex] * 3 + 1);
		const size_t vertBIndex = _faces[f * 3 + 1];
		const glm::vec2 vertB(_vertices[vertBIndex * 3], _vertices[vertBIndex] * 3 + 1);
		const size_t vertCIndex = _faces[f * 3 + 2];
		const glm::vec2 vertC(_vertices[vertCIndex * 3], _vertices[vertCIndex] * 3 + 1);

		if (Common::intersectTrianglePoint2D(point, vertA, vertB, vertC))
			return true;
	}

	return false;
}

bool ObjectWalkmesh::in(const glm::vec3 &start, const glm::vec3 &end) const {
	glm::vec3 segment = end - start;

	// Avoid division by zero.
	for (uint8_t i = 0; i < 3; ++i) {
		if (fabs(segment[i]) < 0.00001f)
			segment[i] = 0.00001f;
	}

	const float t1 = (_min.x - start.x) / segment.x;
	const float t2 = (_max.x - start.x) / segment.x;
	const float t3 = (_min.y - start.y) / segment.y;
	const float t4 = (_max.y - start.y) / segment.y;
	const float t5 = (_min.z - start.z) / segment.z;
	const float t6 = (_max.z - start.z) / segment.z;

	const float tmin = fmaxf(fmaxf(fminf(t1, t2), fminf(t3, t4)), fminf(t5, t6));
	const float tmax = fminf(fminf(fmaxf(t1, t2), fmaxf(t3, t4)), fmaxf(t5, t6));

	// if tmax < 0, ray is intersecting box
	// but entire box is behing it's origin.
	if (tmax < 0)
		return false;

	// if tmin > tmax, ray doesn't intersect box.
	if (tmin > tmax)
		return false;

	// If tmin > 1 the segment is before the box.
	if (tmin > 1)
		return false;

	return true;
}

bool ObjectWalkmesh::findIntersection(const glm::vec3 &start, const glm::vec3 &end,
                                      glm::vec3 &intersect) const {
	if (!in(start, end))
		return false;

	float distance = FLT_MAX;
	glm::vec2 testPosition;
	float testDistance;
	const glm::vec3 dir = glm::normalize(end - start);
	for (size_t f = 0; f < _faces.size() / 3; ++f) {
		const glm::vec3 vertA(_vertices[_faces[f * 3] * 3],
		        _vertices[_faces[f * 3] * 3 + 1],
		        _vertices[_faces[f * 3] * 3 + 2]);
		const glm::vec3 vertB(_vertices[_faces[f * 3 + 1] * 3],
		        _vertices[_faces[f * 3 + 1] * 3 + 1],
		        _vertices[_faces[f * 3 + 1] * 3 + 2]);
		const glm::vec3 vertC(_vertices[_faces[f * 3 + 2] * 3],
		        _vertices[_faces[f * 3 + 2] * 3 + 1],
		        _vertices[_faces[f * 3 + 2] * 3 + 2]);
		if (!glm::intersectRayTriangle(start, dir, vertA, vertB, vertC, testPosition, testDistance))
			continue;

		glm::vec3 test = start + dir * testDistance;
		if (glm::distance(start, test) >= distance)
			continue;

		distance = glm::distance(start, test);
		intersect = test;
	}

	if (distance == FLT_MAX)
		return false;

	return true;
}

const std::vector<float> &ObjectWalkmesh::getVertices() const {
	return _vertices;
}

const std::vector<uint32_t> &ObjectWalkmesh::getFaces() const {
	return _faces;
}

void ObjectWalkmesh::computeMinMax() {
	_min[0] = FLT_MAX;
	_min[1] = FLT_MAX;
	_min[2] = FLT_MAX;
	_max[0] = FLT_MIN;
	_max[1] = FLT_MIN;
	_max[2] = FLT_MIN;

	for (size_t v = 0; v < _vertices.size() / 3; ++v) {
		// x coordinate.
		_min[0] = MIN(_vertices[v * 3], _min[0]);
		_max[0] = MAX(_vertices[v * 3], _max[0]);
		// y coordinate.
		_min[1] = MIN(_vertices[v * 3 + 1], _min[1]);
		_max[1] = MAX(_vertices[v * 3 + 1], _max[1]);
		// z coordinate.
		_min[2] = MIN(_vertices[v * 3 + 2], _min[2]);
		_max[2] = MAX(_vertices[v * 3 + 2], _max[2]);
	}
}

} // End of namespace KotORBase

} // End of namespace Engines
