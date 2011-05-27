/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file engines/nwn/creature.h
 *  NWN creature.
 */

#ifndef ENGINES_NWN_CREATURE_H
#define ENGINES_NWN_CREATURE_H

#include "common/types.h"
#include "common/ustring.h"

#include "aurora/types.h"
#include "aurora/locstring.h"

#include "graphics/aurora/types.h"
#include "graphics/aurora/textureman.h"

#include "engines/nwn/types.h"
#include "engines/nwn/object.h"

namespace Aurora {
	class GFFStruct;
}

namespace Engines {

namespace NWN {

class Tooltip;

class Creature : public Object {
public:
	Creature();
	~Creature();

	/** Clear the creature information. */
	void clear();

	/** Last time info was changed that's displayed in the GUI. */
	uint32 lastChangedGUIDisplay() const;

	void show();
	void hide();

	void setPosition(float x, float y, float z);
	void setOrientation(float x, float y, float z);

	void enter();
	void leave();

	void highlight(bool enabled);

	/** Load from a character file. */
	void loadCharacter(Common::SeekableReadStream &stream);
	/** Load from a creature instance. */
	void load(const Aurora::GFFStruct &creature);

	/** Load the creature's model. */
	void loadModel();
	/** Unload the creature's model. */
	void unloadModel();

	/** Return the creature's first name. */
	const Common::UString &getFirstName() const;
	/** Return the creature's last name. */
	const Common::UString &getLastName() const;

	/** Get the creature's gender. */
	uint32 getGender() const;
	/** Is the creature female, do we need female dialogs tokens? */
	bool isFemale() const;

	/** Return the creature's race value. */
	uint32 getRace() const;

	/** Return the creature's race as needed in conversations, e.g. "Dwarven". */
	const Common::UString &getConvRace() const;
	/** Return the creature's lowercase race as needed in conversations, e.g. "dwarven". */
	const Common::UString &getConvrace() const;
	/** Return the creature's race plural as needed in conversations, e.g. "Dwarves". */
	const Common::UString &getConvRaces() const;

	/** Get the creature's subrace. */
	const Common::UString &getSubRace() const;

	/** Return the creature's class as needed in conversations, e.g. "Barbarian". */
	const Common::UString &getConvClass() const;
	/** Return the creature's class as needed in conversations, e.g. "barbarian". */
	const Common::UString &getConvclass() const;
	/** Return the pcreature's class plural as needed in conversations, e.g. "Barbarians". */
	const Common::UString &getConvClasses() const;

	/** Return the creature's class description. */
	Common::UString getClassString() const;

	/** Returns the number of hit dice, which is effectively the total number of levels. */
	uint8 getHitDice() const;

	/** Get the creature's deity. */
	const Common::UString &getDeity() const;

	bool isPC() const; ///< Is the creature a player character?
	bool isDM() const; ///< Is the creature a dungeon master?

	/** Return the creature's age. */
	uint32 getAge() const;

	/** Return the creature's XP. */
	uint32 getXP() const;

	/** Return a creature's ability score. */
	uint8 getAbility(Ability ability) const;

	/** Return the current HP this creature has. */
	int32 getCurrentHP() const;
	/** Return the max HP this creature can have. */
	int32 getMaxHP() const;

private:
	enum BodyPartType {
		kBodyPartHead       = 0,
		kBodyPartNeck          ,
		kBodyPartTorso         ,
		kBodyPartPelvis        ,
		kBodyPartBelt          ,
		kBodyPartRightFoot     ,
		kBodyPartLeftFoot      ,
		kBodyPartRightShin     ,
		kBodyPartLeftShin      ,
		kBodyPartLeftThigh     ,
		kBodyPartRightThigh    ,
		kBodyPartRightFArm     ,
		kBodyPartLeftFArm      ,
		kBodyPartRightBicep    ,
		kBodyPartLeftBicep     ,
		kBodyPartRightShoul    ,
		kBodyPartLeftShoul     ,
		kBodyPartRightHand     ,
		kBodyPartLeftHand      ,
		kBodyPartMAX
	};

	struct Class {
		uint32 classID;
		uint16 level;
	};

	struct BodyPart {
		uint32 id;
		Common::UString modelName;
		std::list<Graphics::Aurora::PLTHandle> plts;

		BodyPart();
	};

	uint32 _lastChangedGUIDisplay;

	Common::UString _firstName;
	Common::UString _lastName;

	uint32 _gender;
	uint32 _race;

	Common::UString _subRace;

	bool _isPC;
	bool _isDM;

	uint32 _age;

	uint32 _xp;

	int32 _baseHP;
	int32 _bonusHP;
	int32 _currentHP;

	uint8 _abilities[kAbilityMAX];

	std::vector<Class> _classes;

	uint8 _hitDice;

	Common::UString _deity;

	uint32 _appearanceID;
	uint32 _phenotype;

	std::vector<BodyPart> _bodyParts;

	uint32 _colorSkin;
	uint32 _colorHair;
	uint32 _colorTattoo1;
	uint32 _colorTattoo2;

	Graphics::Aurora::Model *_model;

	Tooltip *_tooltip;


	void load(const Aurora::GFFStruct &instance, const Aurora::GFFStruct *blueprint);

	void loadProperties(const Aurora::GFFStruct &gff);
	void loadPortrait(const Aurora::GFFStruct &gff);

	void constructModelName(const Common::UString &type, uint32 id,
	                        const Common::UString &gender,
	                        const Common::UString &race,
	                        const Common::UString &phenoType,
	                        const Common::UString &phenoTypeAlt,
	                        Common::UString &model);
	void getPartModels();

	void finishPLTs(std::list<Graphics::Aurora::PLTHandle> &plts);

	void createTooltip();
	void showTooltip();
	void hideTooltip();
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_CREATURE_H
