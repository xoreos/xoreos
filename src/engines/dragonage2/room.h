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
 *  A room in a Dragon Age II area.
 */

#ifndef ENGINES_DRAGONAGE2_ROOM_H
#define ENGINES_DRAGONAGE2_ROOM_H

#include <vector>

#include "src/common/ptrvector.h"
#include "src/common/ustring.h"

#include "src/aurora/types.h"

#include "src/graphics/aurora/types.h"

#include "src/engines/aurora/resources.h"

namespace Engines {

namespace DragonAge2 {

class Room {
public:
	Room(const Aurora::GFF4Struct &room);
	~Room();

	int32_t getID() const;

	void show();
	void hide();

private:
	typedef Common::PtrVector<Graphics::Aurora::Model> Models;

	int32_t _id;

	Models _models;

	ChangeList _resources;

	void load(const Aurora::GFF4Struct &room);
	void loadLayout(const Common::UString &roomFile);

	void clean();
};

} // End of namespace DragonAge2

} // End of namespace Engines

#endif // ENGINES_DRAGONAGE2_ROOM_H
