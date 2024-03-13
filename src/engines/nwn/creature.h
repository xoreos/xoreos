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
 *  A creature in a Neverwinter Nights area.
 */

#ifndef ENGINES_NWN_CREATURE_H
#define ENGINES_NWN_CREATURE_H

#include <vector>
#include <list>
#include <memory>

#include "src/common/types.h"
#include "src/common/ustring.h"

#include "src/aurora/types.h"

#include "src/graphics/aurora/types.h"
#include "src/graphics/aurora/texturehandle.h"

#include "src/engines/nwn/types.h"
#include "src/engines/nwn/object.h"

namespace Engines {

namespace NWN {

class Item;

class Creature : public Object {
public:
	/** Create a dummy creature instance. Not playable as it is.*/
	Creature();
	/** Load from a creature instance. */
	Creature(const Aurora::GFF3Struct &creature);
	/** Load from a character file. */
	Creature(const Common::UString &bic, bool local);
	~Creature();

	/** Last time info was changed that's displayed in the GUI. */
	uint32_t lastChangedGUIDisplay() const;

	// Basic visuals

	void loadModel();   ///< Load the creature's model.
	void unloadModel(); ///< Unload the creature's model.

	void show(); ///< Show the creature's model.
	void hide(); ///< Hide the creature's model.

	virtual void renderImmediate(const glm::mat4 &parentTransform);

	// Basic properties

	/** Return the creature's first name. */
	const Common::UString &getFirstName() const;
	/** Return the creature's last name. */
	const Common::UString &getLastName() const;

	/** Get the creature's gender. */
	Gender getGender() const;
	/** Set the creature's gender. */
	void setGender(Gender gender);
	/** Is the creature female, do we need female dialogs tokens? */
	bool isFemale() const;

	/** Return the creature's race value. */
	uint32_t getRace() const;
	/** Set the creature's race. */
	void setRace(uint32_t race);

	/** Set the creature's portrait. */
	void setPortrait(const Common::UString &portrait);
	/** Get the creature's portrait. */
	const Common::UString &getPortrait() const;

	/** Return the creature's race as needed in conversations, e.g. "Dwarven". */
	const Common::UString &getConvRace() const;
	/** Return the creature's lowercase race as needed in conversations, e.g. "dwarven". */
	const Common::UString &getConvrace() const;
	/** Return the creature's race plural as needed in conversations, e.g. "Dwarves". */
	const Common::UString &getConvRaces() const;

	/** Get the creature's subrace. */
	const Common::UString &getSubRace() const;

	/** Get the creature's class and level at that class slot position. */
	void getClass(uint32_t position, uint32_t &classID, uint16_t &level) const;
	/** Get the creature's level for this class. */
	uint16_t getClassLevel(uint32_t classID) const;
	/** Set the creature's level for this class. */
	void changeClassLevel(uint32_t classID, int16_t levelChange);

	/** Return the creature's class as needed in conversations, e.g. "Barbarian". */
	const Common::UString &getConvClass() const;
	/** Return the creature's class as needed in conversations, e.g. "barbarian". */
	const Common::UString &getConvclass() const;
	/** Return the creature's class plural as needed in conversations, e.g. "Barbarians". */
	const Common::UString &getConvClasses() const;

	/** Return the creature's class description. */
	Common::UString getClassString() const;

	/** Returns the number of hit dice, which is effectively the total number of levels. */
	uint8_t getHitDice() const;

	/** Return a creature's ability score. */
	uint8_t getAbility(Ability ability) const;
	/** Set the creature's ability score. */
	void setAbility(Ability ability, uint8_t score);
	/** Return the creature's rank in this skill. */
	int8_t getSkillRank(uint32_t skill) const;
	/** Set the creature's skill rank. */
	void setSkillRank(size_t skill, uint8_t rank);
	/** Append a feat to the creature. */
	void setFeat(uint32_t feat);
	/** Does the creature have this feat? */
	bool hasFeat(uint32_t feat) const;

	/** Get the creature's deity. */
	const Common::UString &getDeity() const;

	/** Get the creature's good-evil alignment. */
	uint8_t getGoodEvil() const;
	/** Set the creature's good-evil alignment. */
	void setGoodEvil(uint8_t goodness);
	/** Get the creature's law-chaos alignment. */
	uint8_t getLawChaos() const;
	/** Set the creature's law-chaos alignment. */
	void setLawChaos(uint8_t lawfulness);

	/** Set the creature's sound set. */
	void setSoundSet(uint32_t soundSet);

	bool isPC() const; ///< Is the creature a player character?
	bool isDM() const; ///< Is the creature a dungeon master?

	/** Return the creature's age. */
	uint32_t getAge() const;

	/** Return the creature's XP. */
	uint32_t getXP() const;

	/** Return the current HP this creature has. */
	int32_t getCurrentHP() const;
	/** Return the max HP this creature can have. */
	int32_t getMaxHP() const;

	/** Return the creature's starting package. */
	uint8_t getStartingPackage() const;
	/** Set the creature's starting package. */
	void setStartingPackage(uint8_t package);

	/** Get clerical domains. */
	void getDomains(uint32_t classID, uint8_t &domain1, uint8_t &domain2);
	/** Set clerical domains. */
	void setDomains(uint32_t classID, uint8_t domain1, uint8_t domain2);

	/** Do the creature know the spell? */
	bool hasSpell(uint32_t classID, size_t spellLevel, uint16_t spell);
	/** Set the creature's school. */
	void setSchool(uint32_t classID, uint8_t school);
	/** Set a creature's known spell. */
	void setKnownSpell(uint32_t classID, size_t spellLevel, uint16_t spell);
	/** Set a creature's memorized spell. */
	void setMemorizedSpell(uint32_t classID, size_t spellLevel, uint16_t spell);

	/** Set the creature's appearance. */
	void setAppearance(uint32_t appearanceID);
	/** Set the creature's phenotype. */
	void setPhenotype(uint32_t phenotype);
	/** Set the creature's color skin. */
	void setColorSkin(uint32_t colorSkin);
	/** Set the creature's color hair. */
	void setColorHair(uint32_t colorHair);
	/** Set the creature's color tattoo 1. */
	void setColorTatto1(uint32_t colorTattoo1);
	/** Set the creature's color tatto 2. */
	void setColorTatto2(uint32_t colorTattoo2);
	/** Set the creature's head. */
	void setHead(uint32_t headID);

	/** Add an equippement to the creature. */
	void addEquippedItem(Item *item);

	// Object/Object interactions

	/** Set the area this creature is currently in. */
	void setArea(Area *area);

	/** Add an associate (henchman, familiar, ...). */
	void addAssociate(Creature &associate, AssociateType type);
	/** Remove an associate (henchman, familiar, ...). */
	void removeAssociate(Creature &ssociate);

	/** Get this creature's nth's associate of a specific type. */
	Creature *getAssociate(AssociateType type, size_t nth = 1) const;

	/** Return the creature's master. */
	Creature *getMaster() const;
	/** Set the creature's master. */
	void setMaster(Creature *master = 0);

	/** Is this creature commandable (has a modifiable action queue)? */
	bool isCommandable() const;
	/** Set whether this creature commandable (has a modifiable action queue). */
	void setCommandable(bool commandable);

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

	/** The creature was clicked. */
	virtual bool click(Object *triggerer = 0);

	// Animation

	/** Play a creature animation. */
	void playAnimation(const Common::UString &animation = "", bool restart = true,
	                   float length = 0.0f, float speed = 1.0f);
	/** Play a default creature animation. */
	void playAnimation(Animation animation, bool restart = true,
	                   float length = 0.0f, float speed = 1.0f);


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
		uint32_t classID; ///< Index into classes.2da.
		uint16_t level;   ///< Levels of that class.

		uint8_t domain1; ///< Cleric's domain.
		uint8_t domain2; ///< Cleric's domain.

		uint8_t school; ///< Wizard's spell school.

		std::vector<std::vector<uint16_t> > knownList;     ///< Known spells list.
		std::vector<std::vector<uint16_t> > memorizedList; ///< Memorized spells list

		Class();
	};

	/** An associate. */
	struct Associate {
		AssociateType type;  ///< The associate's type.
		Creature *associate; ///< The associate.

		Associate(AssociateType t = kAssociateTypeNone, Creature *a = 0);
	};

	/** A part of a creature body. */
	struct BodyPart {
		uint32_t id;      ///< Index of the part variant.
		uint32_t idArmor; ///< Index of the part variant when armour equipped.

		Common::UString modelName;   ///< Name of the model.
		Common::UString textureName; ///< Name of the texture.

		/** Actual textures loaded by the part's model. */
		std::list<Graphics::Aurora::TextureHandle> textures;

		BodyPart();
	};

	/** The time a GUI relevant property was changed last. */
	uint32_t _lastChangedGUIDisplay;

	Common::UString _firstName; ///< The creature's first name.
	Common::UString _lastName;  ///< The creature's last name.

	Gender   _gender; ///< The creature's gender.
	uint32_t _race;   ///< The creature's race.

	Common::UString _subRace; ///< The creature's subrace.

	bool _isPC; ///< Is the creature a PC?
	bool _isDM; ///< Is the creature a DM?

	uint32_t _age; ///< The creature's age.

	uint32_t _xp; ///< The creature's experience.

	int32_t _baseHP;    ///< The creature's base maximum health points.
	int32_t _bonusHP;   ///< The creature's bonus health points.
	int32_t _currentHP; ///< The creature's current health points.

	uint8_t _abilities[kAbilityMAX]; ///< The creature's abilities.

	std::vector<Class>    _classes; ///< The creature's classes.
	std::vector<int8_t>   _skills;  ///< The creature's skills.
	std::vector<uint32_t> _feats;   ///< The creature's feats.

	uint8_t _hitDice; ///< The creature's hit dice.

	Common::UString _deity; ///< The creature's deity.

	uint8_t _goodEvil; ///< The creature's good/evil value (0-100).
	uint8_t _lawChaos; ///< The creature's law/chaos value (0-100).

	uint32_t _appearanceID; ///< The creature's general appearance.
	uint32_t _phenotype;    ///< The creature's phenotype.

	uint8_t _startingPackage; ///< The package chosen at creature's creation.

	std::vector<BodyPart> _bodyParts; ///< The creature's body parts.

	std::vector<std::unique_ptr<Item>> _equippedItems; ///< The creature's equipped items.

	Common::UString _partsSuperModelName; ///< The supermodel used for parts-based creatures.

	Common::UString _environmentMap; ///< The environment map override to use on the model.

	uint32_t _colorSkin;    ///< The color of the creature's skin.
	uint32_t _colorHair;    ///< The color of the creature's hair.
	uint32_t _colorTattoo1; ///< The 1. color of the creature's tattoo.
	uint32_t _colorTattoo2; ///< The 2. color of the creature's tattoo.

	uint32_t _colorMetal1;   ///< The 1. color of the creature's metal armor.
	uint32_t _colorMetal2;   ///< The 2. color of the creature's metal armor.
	uint32_t _colorLeather1; ///< The 1. color of the creature's leather armor.
	uint32_t _colorLeather2; ///< The 2. color of the creature's leather armor.
	uint32_t _colorCloth1;   ///< The 1. color of the creature's cloth armor.
	uint32_t _colorCloth2;   ///< The 2. color of the creature's cloth armor.

	Creature *_master;                ///< The creature's master.
	std::list<Associate> _associates; ///< The creature's associates.

	/** Is the creature commandable (has a modifiable action queue)? */
	bool _isCommandable;

	std::unique_ptr<Graphics::Aurora::Model> _model; ///< The creature's model.


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

	/** Load the creature's portrait. */
	static void loadPortrait(const Aurora::GFF3Struct &gff, Common::UString &portrait);
	/** Load the creature's classes. */
	static void loadClasses (const Aurora::GFF3Struct &gff,
	                         std::vector<Class> &classes, uint8_t &hitDice);

	void loadEquippedItems(const Aurora::GFF3Struct &gff);

	/** Construct the resource name of a body part files. */
	void constructPartName(const Common::UString &type, uint32_t id,
	                                 const Common::UString &gender,
	                                 const Common::UString &race,
	                                 const Common::UString &phenoType,
	                                 Common::UString &part);

	/** Construct the resource name of a body part files. */
	void constructPartName(const Common::UString &type, uint32_t id,
	                                 const Common::UString &gender,
	                                 const Common::UString &race,
	                                 const Common::UString &phenoType,
	                                 const Common::UString &phenoTypeAlt,
	                                 Aurora::FileType fileType,
	                                 Common::UString &part);

	/** Construct the resource name of a body part files. */
	void constructModelName(const Common::UString &type, uint32_t id,
	                        const Common::UString &gender,
	                        const Common::UString &race,
	                        const Common::UString &phenoType,
	                        const Common::UString &phenoTypeAlt,
	                        Common::UString &model,
	                        Common::UString &texture);

	void getPartModels(); ///< Construct all body part models' resource names.
	void getArmorModels(); ///< Populate the armor info for body parts.

	/** Find the creature's class if any. */
	Class *findClass(uint32_t classID);

	/** Finished those paletted textures. */
	void finishPLTs(const std::list<Graphics::Aurora::TextureHandle> &plts);

	static Aurora::GFF3File *openPC(const Common::UString &bic, bool local);
	static void getClassString(const std::vector<Class> &classes, Common::UString &str);

	/** Create an empty tooltip. */
	bool createTooltip(Tooltip::Type type);
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_CREATURE_H
