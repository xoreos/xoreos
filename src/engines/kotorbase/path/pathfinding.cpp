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
 *  Pathfinding class for KotOR games.
 */

#include "src/common/util.h"
#include "src/common/aabbnode.h"

#include "src/engines/aurora/astar.h"

#include "src/engines/kotorbase/room.h"

#include "src/engines/kotorbase/path/walkmeshloader.h"
#include "src/engines/kotorbase/path/pathfinding.h"

namespace Engines {

namespace KotORBase {

Pathfinding::Pathfinding(const std::vector<bool> &walkableProp) :
		Engines::Pathfinding(walkableProp) {

	AStar * aStarAlgorithm = new AStar(this);
	setAStarAlgorithm(aStarAlgorithm);
}

void Pathfinding::addRoom(Room *room) {
	_startFace.push_back(_faces.size() / 3);

	std::map<uint32_t, uint32_t> adjRooms;
	_walkmeshLoader.load(Aurora::kFileTypeWOK, room->getResRef(), glm::mat4(),
	                     _vertices, _faces, _faceProperty, _adjFaces, adjRooms,
	                     this);
	_adjRooms.push_back(adjRooms);

	Common::AABBNode *rootNode = _walkmeshLoader.getAABB();
	_aabbTrees.push_back(rootNode);
	_rooms.push_back(room);
}

void Pathfinding::connectRooms() {
	_verticesCount = _vertices.size() / 3;
	_facesCount = _faces.size() / 3;

	for (size_t r = 0; r < _adjRooms.size(); ++r) {
		for (std::map<uint32_t, uint32_t>::iterator ar = _adjRooms[r].begin();
		     ar != _adjRooms[r].end(); ++ar) {
			if (ar->second == UINT32_MAX)
				continue;

			uint32_t currFace = getFaceFromEdge(ar->first, r);
			// Get adjacent face from the other room.
			uint32_t otherRoom = ar->second;
			for (std::map<uint32_t, uint32_t>::iterator oF = _adjRooms[otherRoom].begin();
			     oF != _adjRooms[otherRoom].end(); ++oF) {
				// Get only faces adjacent to the current room (r).
				if (oF->second != r)
					continue;

				uint32_t otherFace = getFaceFromEdge(oF->first, otherRoom);
				// Check if at least two vertices are the same.
				std::vector<glm::vec3> currVerts;
				std::vector<glm::vec3> othVerts;
				getVertices(currFace, currVerts);
				getVertices(otherFace, othVerts);

				const uint32_t cEdge = ar->first % 3;
				const uint32_t oEdge = oF->first % 3;
				const glm::vec3 &cVert1 = currVerts[cEdge];
				const glm::vec3 &cVert2 = currVerts[(cEdge + 1) % 3];
				const glm::vec3 &oVert1 = othVerts[oEdge];
				const glm::vec3 &oVert2 = othVerts[(oEdge + 1) % 3];

				if ((cVert1[0] == oVert1[0] && cVert2[0] == oVert2[0]
				     && cVert1[1] == oVert1[1] && cVert2[1] == oVert2[1])
				    || (cVert1[0] == oVert2[0] && cVert2[0] == oVert1[0]
				        && cVert1[1] == oVert2[1] && cVert2[1] == oVert1[1])) {

					_adjFaces[currFace * 3 + cEdge % 3] = otherFace;
					_adjFaces[otherFace * 3 + oEdge % 3] = currFace;

					break;
				}
			}
		}
	}
}

uint32_t Pathfinding::getFaceFromEdge(uint32_t edge, uint32_t room) const {
	if (edge == UINT32_MAX)
		error("Edge is not valid");

	return (edge + (2 - edge % 3)) / 3 +  _startFace[room];
}

Room *Pathfinding::getRoomAt(float x, float y) const {
	for (size_t n = 0; n < _aabbTrees.size(); ++n) {
		if (!_aabbTrees[n])
			continue;

		if (!_aabbTrees[n]->isIn(x, y))
			continue;

		return _rooms[n];
	}

	return 0;
}

} // End of namespace KotORBase

} // End of namespace Engines
