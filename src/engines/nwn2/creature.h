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
 *  A creature in a Neverwinter Nights 2 area.
 */

#ifndef ENGINES_NWN2_CREATURE_H
#define ENGINES_NWN2_CREATURE_H

#include <vector>
#include <list>

#include "src/common/types.h"
#include "src/common/ptrlist.h"
#include "src/common/ustring.h"

#include "src/aurora/types.h"

#include "src/graphics/aurora/types.h"

#include "src/engines/nwn2/types.h"
#include "src/engines/nwn2/object.h"

namespace Engines {

namespace NWN2 {

class Creature : public Object {
public:
	/** Create a dummy creature instance. Not playable as it is.*/
	Creature();
	/** Load from a creature instance. */
	Creature(const Aurora::GFF3Struct &creature);
	/** Load from a character file. */
	Creature(const Common::UString &bic, bool local);
	~Creature();

	// Basic visuals

	void loadModel();   ///< Load the creature's model.
	void unloadModel(); ///< Unload the creature's model.

	void show(); ///< Show the creature's model.
	void hide(); ///< Hide the creature's model.

	// Basic properties

	/** Return the creature's first name. */
	const Common::UString &getFirstName() const;
	/** Return the creature's last name. */
	const Common::UString &getLastName() const;

	/** Get the creature's gender. */
	uint32 getGender() const;
	/** Set the creature's gender. */
	void setGender(uint32 gender);
	/** Is the creature female, do we need female dialogs tokens? */
	bool isFemale() const;

	/** Return the creature's race value. */
	uint32 getRace() const;
	/** Set the creature's race. */
	void setRace(uint32 race);

	/** Return the creature's subrace value. */
	uint32 getSubRace() const;
	/** Set the creature's subrace. */
	void setSubRace(uint32 subRace);

	/** Get the creature's class and level at that class slot position. */
	void getClass(uint32 position, uint32 &classID, uint16 &level) const;
	/** Get the creature's level for this class. */
	uint16 getClassLevel(uint32 classID) const;

	/** Returns the number of hit dice, which is effectively the total number of levels. */
	uint8 getHitDice() const;

	/** Return a creature's ability score. */
	uint8 getAbility(Ability ability) const;
	/** Return the creature's rank in this skill. */
	 int8 getSkillRank(uint32 skill) const;
	/** Does the creature have this feat? */
	bool  hasFeat(uint32 feat) const;

	/** Get the creature's deity. */
	const Common::UString &getDeity() const;

	uint8 getGoodEvil() const;
	uint8 getLawChaos() const;

	bool isPC() const; ///< Is the creature a player character?
	bool isDM() const; ///< Is the creature a dungeon master?

	/** Return the creature's age. */
	uint32 getAge() const;

	/** Return the creature's XP. */
	uint32 getXP() const;

	/** Return the current HP this creature has. */
	int32 getCurrentHP() const;
	/** Return the max HP this creature can have. */
	int32 getMaxHP() const;

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
	typedef Common::PtrList<Graphics::Aurora::Model> ModelParts;

	/** A class. */
	struct Class {
		uint32 classID; ///< Index into classes.2da.
		uint16 level;   ///< Levels of that class.
	};

	Common::UString _firstName; ///< The creature's first name.
	Common::UString _lastName;  ///< The creature's last name.

	uint32 _gender;  ///< The creature's gender.
	uint32 _race;    ///< The creature's race.
	uint32 _subRace; ///< The creature's subrace.

	bool _isPC; ///< Is the creature a PC?
	bool _isDM; ///< Is the creature a DM?

	uint32 _age; ///< The creature's age.

	uint32 _xp; ///< The creature's experience.

	int32 _baseHP;    ///< The creature's base maximum health points.
	int32 _bonusHP;   ///< The creature's bonus health points.
	int32 _currentHP; ///< The creature's current health points.

	uint8 _abilities[kAbilityMAX]; ///< The creature's abilities.

	std::vector<Class>  _classes; ///< The creature's classes.
	std::vector<int8>   _skills;  ///< The creature's skills.
	std::vector<uint32> _feats;   ///< The creature's feats.

	uint8 _hitDice; ///< The creature's hit dice.

	Common::UString _deity; ///< The creature's deity.

	uint8 _goodEvil; ///< The creature's good/evil value (0-100).
	uint8 _lawChaos; ///< The creature's law/chaos value (0-100).

	uint32 _appearanceID; ///< The creature's general appearance.

	uint8 _armorVisualType;
	uint8 _armorVariation;

	uint8 _bootsVisualType;
	uint8 _bootsVariation;

	uint8 _appearanceHead;  ///< The model variant used for the head.
	uint8 _appearanceMHair; ///< The model variant used for male hair.
	uint8 _appearanceFHair; ///< The model variant used for female hair.

	ModelParts _modelParts;


	/** Init the creature. */
	void init();
	/** Load from a character file. */
	void loadCharacter(const Common::UString &bic, bool local);
	/** Load from a creature instance. */
	void load(const Aurora::GFF3Struct &creature);

	/** Load the creature from an instance and its blueprint. */
	void load(const Aurora::GFF3Struct &instance, const Aurora::GFF3Struct *blueprint);

	/** Load general creature properties. */
	void loadProperties(const Aurora::GFF3Struct &gff);

	/** Load the creature's classes. */
	static void loadClasses (const Aurora::GFF3Struct &gff,
	                         std::vector<Class> &classes, uint8 &hitDice);

	// Model loaders

	Common::UString getBaseModel(const Common::UString &base);
	bool loadArmorModel(const Common::UString &body, const Common::UString &armor,
	                    uint8 visualType, uint8 variation);
	bool loadHeadModel(uint8 appearance);
	bool loadHairModel(uint8 appearance);

	static Aurora::GFF3File *openPC(const Common::UString &bic, bool local);
};

} // End of namespace NWN2

} // End of namespace Engines

#endif // ENGINES_NWN2_CREATURE_H
