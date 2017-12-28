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
 *  A creature in a Star Wars: Knights of the Old Republic area.
 */

#ifndef ENGINES_KOTOR_CREATURE_H
#define ENGINES_KOTOR_CREATURE_H

#include "src/common/types.h"
#include "src/common/scopedptr.h"
#include "src/common/ustring.h"

#include "src/aurora/types.h"

#include "src/graphics/aurora/types.h"

#include "src/engines/kotor/object.h"

namespace Engines {

namespace KotOR {

class CharacterGenerationInfo;

class Creature : public Object {
public:
	/** Create a dummy creature instance. Not playable as it is.*/
	Creature();
	/** Load from a creature instance. */
	Creature(const Aurora::GFF3Struct &creature);
	~Creature();

	/** Create a fake player character creature for testing purposes. */
	void createFakePC();
	/** Create a player character creature from a character info class. */
	void createPC(CharacterGenerationInfo *info);

	// Basic visuals

	void show(); ///< Show the creature's model.
	void hide(); ///< Hide the creature's model.

	// Basic properties

	bool isPC() const; ///< Is the creature a player character?

	Gender getGender() const; ///< Get the gender of the creature.
	int getLevel(const Class &c) const; ///< Get the level of the creature regarding a specific class.

	Race getRace() const; ///< Get the race of the creature.
	SubRace getSubRace() const; ///< Get the subrace of the creature.

	int getLevelByPosition(int position) const; ///< Get the level by its position in the level vector.
	Class getClassByPosition(int position) const; ///< Get the class by its position in the level vector.

	// Positioning

	/** Set the creature's position. */
	void setPosition(float x, float y, float z);
	/** Set the creature's orientation. */
	void setOrientation(float x, float y, float z, float angle);

	// Object/Cursor interactions

	void enter(); ///< The cursor entered the creature.
	void leave(); ///< The cursor left the creature.

	/** (Un)Highlight the creature. */
	void highlight(bool enabled);

	/** The creature was clicked. */
	bool click(Object *triggerer = 0);

private:
	/** Parts of a creature's body. */
	struct PartModels {
		Common::UString type;

		Common::UString body;
		Common::UString head;

		Common::UString bodyTexture;
	};

	/** A class level. */
	struct ClassLevel {
		Class characterClass;
		int level;
	};

	bool _isPC; ///< Is the creature a PC?

	uint32 _appearance; ///< The creature's general appearance.

	Race _race; ///< The race of the creature.
	SubRace _subRace; ///< The subrace of the creature.

	Gender _gender;
	std::vector<ClassLevel> _levels; ///< The levels of the creature.

	Common::ScopedPtr<Graphics::Aurora::Model> _model; ///< The creature's model.


	void init();

	void load(const Aurora::GFF3Struct &creature);
	void load(const Aurora::GFF3Struct &instance, const Aurora::GFF3Struct *blueprint);

	void loadProperties(const Aurora::GFF3Struct &gff);
	void loadPortrait(const Aurora::GFF3Struct &gff);
	void loadAppearance();

	void getPartModels(PartModels &parts, uint32 state = 'b');
	void loadBody(PartModels &parts);
	void loadHead(PartModels &parts);
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_CREATURE_H
