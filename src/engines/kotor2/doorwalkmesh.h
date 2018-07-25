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
 *  WalkmeshObject implementation for KotOR2 doors.
 */

#ifndef ENGINES_KOTOR2_DOORWALKMESH_H
#define ENGINES_KOTOR2_DOORWALKMESH_H

#include "src/engines/kotor2/objectwalkmesh.h"

namespace Engines {

namespace KotOR2 {

class Door;

class DoorWalkmesh : public Engines::KotOR2::ObjectWalkmesh {
public:
	DoorWalkmesh(Door *door);
	~DoorWalkmesh();

	bool in(glm::vec2 &minBox, glm::vec2 &maxBox) const;
	bool in(glm::vec2 &point) const;

	const std::vector<float> &getVertices() const;
	const std::vector<uint32> &getFaces() const;

private:
	Door *_door;

	std::vector<float> _noVertices;
	std::vector<uint32> _noFaces;
};

} // End of namespace KotOR2

} // End of namespace Engines

#endif // ENGINES_KOTOR2_DOORWALKMESH_H
