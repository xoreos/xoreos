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

#ifndef ENGINES_KOTORBASE_PATH_PATHFINDING_H
#define ENGINES_KOTORBASE_PATH_PATHFINDING_H

#include "external/glm/mat4x4.hpp"

#include "src/aurora/types.h"

#include "src/engines/aurora/pathfinding.h"

#include "src/engines/kotorbase/path/walkmeshloader.h"

namespace Engines {

namespace KotORBase {

class WalkmeshLoader;
class Room;

class Pathfinding : public Engines::Pathfinding {
public:
	Pathfinding(const std::vector<bool> &walkableProp);

	void addRoom(Room *room);
	void connectRooms();

	Room *getRoomAt(float x, float y) const;

protected:
	WalkmeshLoader _walkmeshLoader;
	std::vector<std::map<uint32_t, uint32_t> > _adjRooms;
	std::vector<uint32_t> _startFace;

private:
	uint32_t getFaceFromEdge(uint32_t edge, uint32_t room) const;
	std::vector<Room *> _rooms;
};

} // End of namespace KotORBase

} // End of namespace Engines

#endif // ENGINES_KOTOR_PATHFINDING_H
