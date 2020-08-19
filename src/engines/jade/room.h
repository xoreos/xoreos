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
 *  A room within a Jade Empire area.
 */

#ifndef ENGINES_JADE_ROOM_H
#define ENGINES_JADE_ROOM_H

#include <memory>
#include "src/common/changeid.h"

#include "src/graphics/aurora/types.h"

namespace Engines {

namespace Jade {

class Room {
public:
	Room(const Common::UString &resRef, uint32_t id, float x, float y, float z, bool walkable);
	~Room();

	const Common::UString &getResRef() const;

	bool isWalkable() const;

	void show();
	void hide();

private:
	Common::ChangeID _resources;

	const Common::UString &_resRef;

	std::unique_ptr<Graphics::Aurora::Model> _model;

	bool _walkable;

	void load(const Common::UString &resRef, uint32_t id, float x, float y, float z);
	void unload();
};

} // End of namespace Jade

} // End of namespace Engines

#endif // ENGINES_JADE_ROOM_H
