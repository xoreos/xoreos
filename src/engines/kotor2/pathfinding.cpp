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
 *  Pathfinding class for KotOR2.
 */

#include "src/common/aabbnode.h"

#include "src/engines/kotorbase/room.h"

#include "src/engines/kotor2/pathfinding.h"

namespace Engines {

namespace KotOR2 {

Pathfinding::Pathfinding(const std::vector<bool> &walkableProp)
    : KotOR::Pathfinding(walkableProp) {
}

void Pathfinding::addRoom(KotOR::Room *room) {
	_startFace.push_back(_faces.size() / 3);

	std::map<uint32, uint32> adjRooms;
	_walkmeshLoader.load(Aurora::kFileTypeWOK, room->getResRef(), glm::mat4(),
	                     _vertices, _faces, _faceProperty, _adjFaces, adjRooms,
	                     this);
	_adjRooms.push_back(adjRooms);

	Common::AABBNode *rootNode = _walkmeshLoader.getAABB();
	_AABBTrees.push_back(rootNode);
	_roomsKotOR2.push_back(room);
}

KotOR::Room *Pathfinding::getRoomAt(float x, float y) const {
	for (size_t n = 0; n < _AABBTrees.size(); ++n) {
		if (!_AABBTrees[n])
			continue;

		if (!_AABBTrees[n]->isIn(x, y))
			continue;

		return _roomsKotOR2[n];
	}

	return 0;
}

} // End of namespace KotOR2

} // End of namespace Engines
