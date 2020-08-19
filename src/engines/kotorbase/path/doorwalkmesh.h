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

#ifndef ENGINES_KOTORBASE_PATH_DOORWALKMESH_H
#define ENGINES_KOTORBASE_PATH_DOORWALKMESH_H

#include "src/engines/kotorbase/path/objectwalkmesh.h"

namespace Engines {

namespace KotORBase {

class Door;

class DoorWalkmesh : public ObjectWalkmesh {
public:
	DoorWalkmesh(Door *door);

	const std::vector<float> &getVertices() const;
	const std::vector<uint32_t> &getFaces() const;

	bool in(const glm::vec2 &minBox, const glm::vec2 &maxBox) const;
	bool in(const glm::vec2 &point) const;
	bool findIntersection(const glm::vec3 &start, const glm::vec3 &end,
	                      glm::vec3 &intersect) const;

private:
	Door *_door;

	std::vector<float> _noVertices;
	std::vector<uint32_t> _noFaces;
};

} // End of namespace KotORBase

} // End of namespace Engines

#endif // ENGINES_KOTORBASE_PATH_DOORWALKMESH_H
