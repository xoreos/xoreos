/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
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

namespace Engines {

namespace NWN {

class Tooltip;

class Creature : public Object {
public:
	/** Load from a creature instance. */
	Creature(const Aurora::GFFStruct &creature);
	/** Load from a character file. */
	Creature(const Common::UString &bic, bool local);
	~Creature();

	/** Last time info was changed that's displayed in the GUI. */
	uint32 lastChangedGUIDisplay() const;

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

	/** Get the creature's class and level at that class slot position. */
	void getClass(uint32 position, uint32 &classID, uint16 &level) const;
	/** Get the creature's level for this class. */
	uint16 getClassLevel(uint32 classID) const;

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

	// Object/Object interactions

	/** Add an associate (henchman, familiar, ...). */
	void addAssociate(Creature &associate, AssociateType type);
	/** Remove an associate (henchman, familiar, ...). */
	void removeAssociate(Creature &ssociate);

	/** Get this creature's nth's associate of a specific type. */
	Creature *getAssociate(AssociateType type, int nth = 1) const;

	/** Return the creature's master. */
	Creature *getMaster() const;
	/** Set the creature's master. */
	void setMaster(Creature *master = 0);

	/** Is this creature commandable (has a modifyable action queue)? */
	bool isCommandable() const;
	/** Set whether this creature commandable (has a modifyable action queue). */
	void setCommandable(bool commandable);

	// Positioning

	/** Set the creature's position. */
	void setPosition(float x, float y, float z);
	/** Set the creature's orientation. */
	void setOrientation(float x, float y, float z);

	// Object/Cursor interactions

	void enter(); ///< The cursor entered the creature. */
	void leave(); ///< The cursor left the creature. */

	/** (Un)Highlight the creature. */
	virtual void highlight(bool enabled);

	/** The creature was clicked. */
	virtual bool click(Object *triggerer = 0);


	/** Return the information needed for a character list. */
	static void getPCListInfo(const Common::UString &bic, bool local,
	                          Common::UString &name, Common::UString &classes,
	                          Common::UString &portrait);

private:
	/** Parts of a creature's body. */
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

	/** A class. */
	struct Class {
		uint32 classID; ///< Index into classes.2da.
		uint16 level;   ///< Levels of that class.
	};

	/** An assocatiate. */
	struct Associate {
		AssociateType type;  ///< The associate's type.
		Creature *associate; ///< The associate.

		Associate(AssociateType t = kAssociateTypeNone, Creature *a = 0);
	};

	/** A part of a creature body. */
	struct BodyPart {
		uint32 id; ///< Index of the part variant.
		Common::UString modelName; ///< Name of the model.
		std::list<Graphics::Aurora::PLTHandle> plts; ///< Paletted textures.

		BodyPart();
	};

	/** The time a GUI relevant property was changed last. */
	uint32 _lastChangedGUIDisplay;

	Common::UString _firstName; ///< The creature's first name.
	Common::UString _lastName;  ///< The creature's last name.

	uint32 _gender; ///< The creature's gender.
	uint32 _race;   ///< The creature's race.

	Common::UString _subRace; ///< The creature's subrace.

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
	uint8 _lawChaos; ///< The creature's law/chaos value (0-100);

	uint32 _appearanceID; ///< The creature's general appearance.
	uint32 _phenotype;    ///< The creature's phenotype.

	std::vector<BodyPart> _bodyParts; ///< The creature's body parts.

	Common::UString _partsSuperModelName; ///< The supermodel used for parts-based creatures

	uint32 _colorSkin;    ///< The color of the creature's skin.
	uint32 _colorHair;    ///< The color of the creature's hair.
	uint32 _colorTattoo1; ///< The 1. color of the creature's tattoo.
	uint32 _colorTattoo2; ///< The 2. color of the creature's tattoo.

	Creature *_master;                ///< The creature's master.
	std::list<Associate> _associates; ///< The creature's associates.

	/** Is the creature commandable (has a modifyable action queue)? */
	bool _isCommandable;

	Graphics::Aurora::Model *_model; ///< The creature's model. */

	Tooltip *_tooltip; ///< The tooltip displayed over the creature.


	/** Init the creature. */
	void init();
	/** Load from a character file. */
	void loadCharacter(const Common::UString &bic, bool local);
	/** Load from a creature instance. */
	void load(const Aurora::GFFStruct &creature);

	/** Load the creature from an instance and its blueprint. */
	void load(const Aurora::GFFStruct &instance, const Aurora::GFFStruct *blueprint);

	/** Load general creature properties. */
	void loadProperties(const Aurora::GFFStruct &gff);

	/** Load the creature's portrait. */
	static void loadPortrait(const Aurora::GFFStruct &gff, Common::UString &portrait);
	/** Load the creature's classes. */
	static void loadClasses (const Aurora::GFFStruct &gff,
	                         std::vector<Class> &classes, uint8 &hitDice);

	void loadEquippedItems(const Aurora::GFFStruct &gff);

	/** Construct the resource name of a body part model. */
	void constructModelName(const Common::UString &type, uint32 id,
	                        const Common::UString &gender,
	                        const Common::UString &race,
	                        const Common::UString &phenoType,
	                        const Common::UString &phenoTypeAlt,
	                        Common::UString &model);
	void getPartModels(); ///< Construct all body part models' resource names.

	/** Finished those paletted textures. */
	void finishPLTs(std::list<Graphics::Aurora::PLTHandle> &plts);

	void createTooltip(); ///< Create the tooltip.
	void showTooltip();   ///< Show the tooltip.
	void hideTooltip();   ///< Hide the tooltip.

	static Aurora::GFFFile *openPC(const Common::UString &bic, bool local);
	static void getClassString(const std::vector<Class> &classes, Common::UString &str);
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_CREATURE_H
