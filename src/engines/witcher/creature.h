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
 *  A creature in a The Witcher area.
 */

#ifndef ENGINES_WITCHER_CREATURE_H
#define ENGINES_WITCHER_CREATURE_H

#include "src/common/types.h"

#include "src/graphics/aurora/types.h"

#include "src/engines/witcher/object.h"

namespace Engines {

namespace Witcher {

class Creature : public Object {
public:
	/** Load from a creature template. */
	Creature(const Common::UString &utc);
	~Creature();

	// Basic properties

	bool isPC() const; ///< Is the creature a player character?

private:
	bool _isPC; ///< Is the creature a PC?


	void load(const Common::UString &utc);
	void load(const Aurora::GFF3Struct &utc);
};

} // End of namespace Witcher

} // End of namespace Engines

#endif // ENGINES_WITCHER_CREATURE_H
