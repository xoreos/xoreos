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
 *  WalkmeshObject implementation for doors in KotOR games.
 */

#include "src/engines/kotorbase/door.h"

#include "src/engines/kotorbase/path/doorwalkmesh.h"

namespace Engines {

namespace KotORBase {

DoorWalkmesh::DoorWalkmesh(Door *door) :
		ObjectWalkmesh(door, Aurora::kFileTypeDWK),
		_door(door) {
}

bool DoorWalkmesh::in(const glm::vec2 &minBox, const glm::vec2 &maxBox) const {
	if (_door->isOpen())
		return false;

	return ObjectWalkmesh::in(minBox, maxBox);
}

bool DoorWalkmesh::in(const glm::vec2 &point) const {
	if (_door->isOpen())
		return false;

	return ObjectWalkmesh::in(point);
}

bool DoorWalkmesh::findIntersection(const glm::vec3 &start, const glm::vec3 &end,
                      glm::vec3 &intersect) const {
	if (_door->isOpen())
		return false;

	return ObjectWalkmesh::findIntersection(start, end, intersect);
}

const std::vector<float> &DoorWalkmesh::getVertices() const {
	if (_door->isOpen())
		return _noVertices;

	return ObjectWalkmesh::getVertices();
}

const std::vector<uint32_t> &DoorWalkmesh::getFaces() const {
	if (_door->isOpen())
		return _noFaces;

	return ObjectWalkmesh::getFaces();
}

} // End of namespace KotORBase

} // End of namespace Engines
