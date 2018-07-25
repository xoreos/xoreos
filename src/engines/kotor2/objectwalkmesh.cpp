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
 *  WalkmeshObject implementation for KotOR2.
 */

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "src/common/util.h"
#include "src/common/maths.h"
#include "src/common/geometry.h"

#include "src/engines/kotor2/situated.h"
#include "src/engines/kotor/walkmeshloader.h"
#include "src/engines/kotor2/objectwalkmesh.h"

namespace Engines {

namespace KotOR2 {

ObjectWalkmesh::ObjectWalkmesh(Situated *situated, Aurora::FileType fileType)
    : _fileType(fileType){

	float ori[4];
	float pos[3];
	situated->getOrientation(ori[0], ori[1], ori[2], ori[3]);
	situated->getPosition(pos[0], pos[1], pos[2]);

	Common::UString fileName = situated->getModelName();
	if (fileType == Aurora::kFileTypeDWK)
		fileName += "0";

	load(fileName, ori, pos);
}

ObjectWalkmesh::~ObjectWalkmesh() {
}

void ObjectWalkmesh::load(const Common::UString &resRef, float orientation[4], float position[3]) {
	KotOR::WalkmeshLoader loader;

	glm::mat4 transform;
	transform = glm::translate(transform, glm::make_vec3(position));
	transform = glm::rotate(transform, Common::deg2rad(orientation[3]), glm::make_vec3(orientation));

	std::vector<uint32> faceProperties, adjFaces;
	std::map<uint32, uint32> adjRooms;
	loader.load(_fileType, resRef, transform,
	            _vertices, _faces, faceProperties, adjFaces, adjRooms);

	computeMinMax();
}

bool ObjectWalkmesh::in(glm::vec2 &minBox, glm::vec2 &maxBox) const {
	return Common::intersectBoxes3D(_min, _max, minBox, maxBox);
}

bool ObjectWalkmesh::in(glm::vec2 &point) const {
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

const std::vector<float> &ObjectWalkmesh::getVertices() const {
	return _vertices;
}

const std::vector<uint32> &ObjectWalkmesh::getFaces() const {
	return _faces;
}

void ObjectWalkmesh::computeMinMax() {
	_min[0] = FLT_MAX;
	_min[1] = FLT_MAX;
	_max[0] = FLT_MIN;
	_max[1] = FLT_MIN;

	for (size_t v = 0; v < _vertices.size() / 3; ++v) {
		// x coordinate.
		_min[0] = MIN(_vertices[v * 3], _min[0]);
		_max[0] = MAX(_vertices[v * 3], _max[0]);
		// y coordinate.
		_min[1] = MIN(_vertices[v * 3 + 1], _min[1]);
		_max[1] = MAX(_vertices[v * 3 + 1], _max[1]);
	}
}

} // End of namespace KotOR2

} // End of namespace Engines
