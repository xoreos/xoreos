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

#ifndef ENGINES_KOTOR2_PATHFINDING_H
#define ENGINES_KOTOR2_PATHFINDING_H

#include "src/engines/kotor/pathfinding.h"

namespace Engines {

namespace KotOR {
	class Room;
}

namespace KotOR2 {

class Pathfinding : public KotOR::Pathfinding {
public:
	Pathfinding(const std::vector<bool> &walkableProp);

	void addRoom(KotOR::Room *room);
	KotOR::Room *getRoomAt(float x, float y) const;

private:
	std::vector<KotOR::Room *> _roomsKotOR2;
};

} // End of namespace KotOR2

} // End of namespace Engines

#endif // ENGINES_KOTOR2_PATHFINDING_H
