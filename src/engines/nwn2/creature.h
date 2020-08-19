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
#include "src/engines/nwn2/inventory.h"
#include "src/engines/nwn2/feats.h"

namespace Engines {

namespace NWN2 {

class PersonalReputation;

class Creature : public Object, public Inventory {
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
	uint32_t getGender() const;
	/** Set the creature's gender. */
	void setGender(uint32_t gender);
	/** Is the creature female, do we need female dialogs tokens? */
	bool isFemale() const;

	/** Return the creature's race value. */
	uint32_t getRace() const;
	/** Set the creature's race. */
	void setRace(uint32_t race);

	/** Return the creature's subrace value. */
	uint32_t getSubRace() const;
	/** Set the creature's subrace. */
	void setSubRace(uint32_t subRace);

	/** Get the creature's class and level at that class slot position. */
	void getClass(uint32_t position, uint32_t &classID, uint16_t &level) const;
	/** Get the creature's level for this class. */
	uint16_t getClassLevel(uint32_t classID) const;

	/** Returns the number of hit dice, which is effectively the total number of levels. */
	uint8_t getHitDice() const;

	/** Return a creature's ability score. */
	uint8_t getAbility(Ability ability) const;
	/** Return a creature's ability score modifier */
	int8_t getAbilityModifier(Ability ability) const;
	/** Return true if skill is valid and useable */
	bool getHasSkill(uint32_t skill) const;
	/** Return the creature's rank in this skill. */
	int8_t getSkillRank(uint32_t skill, bool baseOnly = false) const;
	/** Does the creature have this feat? */
	bool  hasFeat(uint32_t feat) const;

	bool featAdd(uint32_t feat, bool checkRequirements);

	/** Get the creature's deity. */
	const Common::UString &getDeity() const;

	uint8_t getGoodEvil() const;
	uint8_t getLawChaos() const;

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

	bool getIsDead() const;
	bool getImmortal() const;
	bool getPlotFlag() const;
	bool getLootable() const;
	bool getIsCreatureDisarmable() const;

	void setImmortal(bool immortal);
	void setPlotFlag(bool plotFlag);
	void setLootable(bool lootable);

	/** Create an item in the creature's inventory. */
	Item *createItemOnObject(const Common::UString &blueprint, uint16_t stackSize, const Common::UString &tag);

	// Reputation

	uint8_t getReputation(Object *source) const;

	/** Add a journal quest entry. */
	void addJournalQuestEntry(const Common::UString &plotID, uint32_t state, bool override);
	/** Remove a journal quest entry. */
	void removeJournalQuestEntry(const Common::UString &plotID);
	/** Return the state of a journal quest entry. */
	uint32_t getJournalEntry(const Common::UString &plotID);

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

	// Tests

	/** Make a skill check */
	bool getIsSkillSuccessful(uint32_t skill, int DC);

private:
	typedef Common::PtrList<Graphics::Aurora::Model> ModelParts;

	/** A class. */
	struct Class {
		uint32_t classID; ///< Index into classes.2da.
		uint16_t level;   ///< Levels of that class.
	};

	/** A level. */
	struct LevelStats {
		uint32_t classID;
		uint16_t skillPoints;
		uint8_t  ability;          ///< Ability to increase
		uint8_t  hitDice;          ///< Hit Dice for the class
		uint8_t  ranks[kSkillMAX]; ///< Rank increases
	};

	/** A quest status. */
	struct QuestState {
		Common::UString plotID; ///< Journal category identifier.
		uint32_t state;         ///< Journal entry id.
	};

	Common::UString _firstName; ///< The creature's first name.
	Common::UString _lastName;  ///< The creature's last name.

	uint32_t _gender;  ///< The creature's gender.
	uint32_t _race;    ///< The creature's race.
	uint32_t _subRace; ///< The creature's subrace.

	bool _isPC; ///< Is the creature a PC?
	bool _isDM; ///< Is the creature a DM?

	uint32_t _age; ///< The creature's age.

	uint32_t _xp; ///< The creature's experience.

	int32_t _baseHP;    ///< The creature's base maximum health points.
	int32_t _bonusHP;   ///< The creature's bonus health points.
	int32_t _currentHP; ///< The creature's current health points.

	bool _isImmortal;           ///< Can the creature never drop below 1 hp?
	bool _isPlot;               ///< Is this a plot creature that takes no damage?
	bool _isLootable;           ///< Does the creature leave behind a lootable corpse?
	bool _isDisarmable;         ///< Can the creature be disarmed?
	bool _isInterruptable;      ///< Can conversation with this creature be interrupted?
	bool _isNoPermanentDeath;   ///< Can the creature not permanently die?
	bool _isSelectableWhenDead; ///< Can the body can be selected when dead.

	uint8_t _abilities[kAbilityMAX]; ///< The creature's abilities.

	std::vector<Class> _classes;     ///< The creature's classes.
	std::vector<LevelStats> _levels; ///< Level data for the creature.
	std::unique_ptr<Feats> _feats;   ///< The creature's feats.

	uint8_t _ranks[kSkillMAX]; ///< Total skill ranks across levels.

	uint8_t _hitDice; ///< The creature's hit dice.

	Common::UString _deity; ///< The creature's deity.

	uint8_t _goodEvil; ///< The creature's good/evil value (0-100).
	uint8_t _lawChaos; ///< The creature's law/chaos value (0-100).

	std::unique_ptr<PersonalReputation> _personalRep; ///< The creature's reputation tracker.
	std::vector<QuestState> _questStates;             ///< Status of the creature's quests.

	uint32_t _appearanceID; ///< The creature's general appearance.

	uint8_t _armorVisualType;
	uint8_t _armorVariation;

	uint8_t _bootsVisualType;
	uint8_t _bootsVariation;

	uint8_t _appearanceHead;  ///< The model variant used for the head.
	uint8_t _appearanceMHair; ///< The model variant used for male hair.
	uint8_t _appearanceFHair; ///< The model variant used for female hair.

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
	                         std::vector<Class> &classes, uint8_t &hitDice);

	/** Load the level stats for the creature. */
	static void loadLevelStats(const Aurora::GFF3Struct &gff,
	                           std::vector<LevelStats> &levelStats,
	                           std::unique_ptr<Feats> &feats);

	/** Load the creature's skill ranks. */
	static void loadSkills(const Aurora::GFF3Struct &gff,
	                       uint8_t ranks[]);

	/** Load the creature's feats. */
	static void loadFeats(const Aurora::GFF3Struct &gff,
	                      std::unique_ptr<Feats> &feats,
	                      uint32_t level = 0);

	/** Load the listening patterns. */
	void loadListenPatterns(const Aurora::GFF3Struct &gff);

	// Model loaders

	Common::UString getBaseModel(const Common::UString &base);
	bool loadArmorModel(const Common::UString &body, const Common::UString &armor,
	                    uint8_t visualType, uint8_t variation);
	bool loadHeadModel(uint8_t appearance);
	bool loadHairModel(uint8_t appearance);

	static Aurora::GFF3File *openPC(const Common::UString &bic, bool local);
};

} // End of namespace NWN2

} // End of namespace Engines

#endif // ENGINES_NWN2_CREATURE_H
