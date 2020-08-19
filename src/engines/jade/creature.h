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
 *  A creature in a Jade Empire area.
 */

#ifndef ENGINES_JADE_CREATURE_H
#define ENGINES_JADE_CREATURE_H

#include "src/common/types.h"
#include <memory>
#include "src/common/ustring.h"

#include "src/aurora/types.h"

#include "src/graphics/aurora/types.h"

#include "src/engines/jade/object.h"
#include "src/engines/jade/gui/chargen/characterinfo.h"

namespace Engines {

namespace Jade {

class Creature : public Object {
public:
	/** Create a dummy creature instance. Not playable as it is.*/
	Creature();
	/** Load from a creature instance. */
	Creature(const Aurora::GFF3Struct &creature);
	~Creature();

	/** Create a fake player character creature for testing purposes. */
	void createFakePC();
	/** Create a player character from the given info from the character generation. */
	void createPC(const CharacterInfo &info);

	// Basic visuals

	void show(); ///< Show the creature's model.
	void hide(); ///< Hide the creature's model.

	// Basic properties

	bool isPC() const; ///< Is the creature a player character?

	int32_t getAutoBalance() const;

	// Positioning

	/** Set the creature's position. */
	void setPosition(float x, float y, float z);
	/** Set the creature's orientation. */
	void setOrientation(float x, float y, float z, float angle);

	// Object/Cursor interactions

	void enter(); ///< The cursor entered the creature.
	void leave(); ///< The cursor left the creature.

	/** (Un)Highlight the creature. */
	virtual void highlight(bool enabled);

private:
	bool _isPC; ///< Is the creature a PC?

	int32_t _autoBalance;

	uint32_t _appearance; ///< The creature's general appearance.
	uint32_t _headType;   ///< The creature's type of head if separate from the model.

	std::unique_ptr<Graphics::Aurora::Model> _model; ///< The creature's model.


	void load(const Aurora::GFF3Struct &creature);

	void loadBlueprint(const Aurora::GFF3Struct &gff);
	void loadInstance(const Aurora::GFF3Struct &gff);
	void loadAppearance();

	void loadBody();
	void loadHead();
};

} // End of namespace Jade

} // End of namespace Engines

#endif // ENGINES_JADE_CREATURE_H
