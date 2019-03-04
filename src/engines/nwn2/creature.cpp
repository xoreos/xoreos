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

#include <cassert>

#include "src/common/scopedptr.h"
#include "src/common/util.h"
#include "src/common/maths.h"
#include "src/common/configman.h"
#include "src/common/readfile.h"
#include "src/common/random.h"

#include "src/aurora/types.h"
#include "src/aurora/gff3file.h"
#include "src/aurora/2dafile.h"
#include "src/aurora/2dareg.h"

#include "src/graphics/aurora/model.h"
#include "src/graphics/aurora/cursorman.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/model.h"

#include "src/engines/nwn2/util.h"
#include "src/engines/nwn2/types.h"
#include "src/engines/nwn2/module.h"
#include "src/engines/nwn2/area.h"
#include "src/engines/nwn2/creature.h"
#include "src/engines/nwn2/inventory.h"
#include "src/engines/nwn2/faction.h"
#include "src/engines/nwn2/cursor.h"

static const uint32 kBICID = MKTAG('B', 'I', 'C', ' ');

namespace Engines {

namespace NWN2 {

Creature::Creature() : Object(kObjectTypeCreature), Inventory() {
	init();
}

Creature::Creature(const Aurora::GFF3Struct &creature) :
	Object(kObjectTypeCreature), Inventory(creature) {
	init();

	load(creature);
}

Creature::Creature(const Common::UString &bic, bool local) : Object(kObjectTypeCreature) {
	init();

	loadCharacter(bic, local);
}

Creature::~Creature() {
}

void Creature::init() {
	_static = false;
	_usable = true;

	_gender  = kGenderNone;
	_race    = kRaceInvalid;
	_subRace = kSubRaceInvalid;

	_isPC = false;
	_isDM = false;

	_age = 0;

	_xp = 0;

	_baseHP    = 0;
	_bonusHP   = 0;
	_currentHP = 0;

	_isImmortal = false;
	_isPlot = false;
	_isLootable = false;
	_isDisarmable = false;
	_isInterruptable = false;
	_isNoPermanentDeath = false;
	_isSelectableWhenDead = false;
	_container = true; // Creatures always have an inventory

	_hitDice = 0;

	_goodEvil = 0;
	_lawChaos = 0;

	_personalRep.reset(new PersonalReputation());

	_appearanceID = Aurora::kFieldIDInvalid;

	_appearanceHead  = 0;
	_appearanceMHair = 0;
	_appearanceFHair = 0;

	_armorVisualType = 0;
	_armorVariation  = 0;

	_bootsVisualType = 0;
	_bootsVariation  = 0;

	for (size_t i = 0; i < kAbilityMAX; i++)
		_abilities[i] = 0;

	for (size_t i = 0; i < kSkillMAX; i++)
		_ranks[i] = 0;
}

void Creature::show() {
	for (ModelParts::iterator m = _modelParts.begin(); m != _modelParts.end(); ++m)
		(*m)->show();
}

void Creature::hide() {
	for (ModelParts::iterator m = _modelParts.begin(); m != _modelParts.end(); ++m)
		(*m)->hide();
}

void Creature::setPosition(float x, float y, float z) {
	Object::setPosition(x, y, z);
	Object::getPosition(x, y, z);

	for (ModelParts::iterator m = _modelParts.begin(); m != _modelParts.end(); ++m)
		(*m)->setPosition(x, y, z);
}

void Creature::setOrientation(float x, float y, float z, float angle) {
	Object::setOrientation(x, y, z, angle);
	Object::getOrientation(x, y, z, angle);

	for (ModelParts::iterator m = _modelParts.begin(); m != _modelParts.end(); ++m)
		(*m)->setOrientation(x, y, z, angle);
}

void Creature::enter() {
	// Set the appropriate cursor for the creature
	if (!getIsDead()) {
		// Creature is alive
		if (!getIsEnemy(getArea()->getModule().getPC())) {
			// Creature is not hostile to the PC
			CursorMan.setGroup(kCursorTalk);
		} else {
			CursorMan.setGroup(kCursorAttack);
		}
	} else {
		if (_isSelectableWhenDead) {
			CursorMan.setGroup(kCursorPickup);
		} else if (getLootable()) {
			// Allow pickup until looted
			CursorMan.setGroup(kCursorPickup);
		} else {
			CursorMan.setGroup(kCursorDefault);
		}
	}
	highlight(true);
}

void Creature::leave() {
	CursorMan.setGroup(kCursorDefault);
	highlight(false);
}

void Creature::highlight(bool enabled) {
	for (ModelParts::iterator m = _modelParts.begin(); m != _modelParts.end(); ++m)
		(*m)->drawBound(enabled);
}

bool Creature::click(Object *triggerer) {
	// Try the onDialog script first
	if (hasScript(kScriptDialogue))
		return runScript(kScriptDialogue, this, triggerer);

	// Next, look we have a generic onClick script
	if (hasScript(kScriptClick))
		return runScript(kScriptClick, this, triggerer);

	return false;
}

const Common::UString &Creature::getFirstName() const {
	return _firstName;
}

const Common::UString &Creature::getLastName() const {
	return _lastName;
}

uint32 Creature::getGender() const {
	return _gender;
}

void Creature::setGender(uint32 gender) {
	_gender = gender;
}

bool Creature::isFemale() const {
	// Male and female are hardcoded.  Other genders (none, both, other)
	// count as male when it comes to tokens in text strings.

	return _gender == kGenderFemale;
}

uint32 Creature::getRace() const {
	return _race;
}

void Creature::setRace(uint32 race) {
	_race = race;
}

uint32 Creature::getSubRace() const {
	return _subRace;
}

void Creature::setSubRace(uint32 subRace) {
	_subRace = subRace;
}

bool Creature::isPC() const {
	return _isPC;
}

bool Creature::isDM() const {
	return _isDM;
}

uint32 Creature::getAge() const {
	return _age;
}

uint32 Creature::getXP() const {
	return _xp;
}

int32 Creature::getCurrentHP() const {
	return _currentHP + _bonusHP;
}

int32 Creature::getMaxHP() const {
	return _baseHP + _bonusHP;
}

bool Creature::getIsDead() const {
	return (getCurrentHP() <= 0);
}

bool Creature::getImmortal() const {
	return _isImmortal;
}

bool Creature::getPlotFlag() const {
	return _isPlot;
}

bool Creature::getLootable() const {
	return _isLootable;
}

bool Creature::getIsCreatureDisarmable() const {
	return _isDisarmable;
}

void Creature::setImmortal(bool immortal) {
	_isImmortal = immortal;
}

void Creature::setPlotFlag(bool plotFlag) {
	_isPlot = plotFlag;
}

void Creature::setLootable(bool lootable) {
	_isLootable = lootable;
}

Item *Creature::createItemOnObject(const Common::UString &blueprint, uint16 stackSize, const Common::UString &tag) {
	return createItem(blueprint, stackSize, tag);
}

/**
 * Perform a skill check against the DC.
 *
 * Modifier effects are applied via data lookup calls.
 */
bool Creature::getIsSkillSuccessful(uint32 skill, int DC) {
	if (!getHasSkill(skill))
		return false;

	// Simulate a d20 roll
	int32 roll = RNG.getNext(1, 21); // TODO: Seed randomized?

	// Make a skill check vs the DC
	bool result = !(roll + getSkillRank(skill) < DC);

	return result;
}

Common::UString Creature::getBaseModel(const Common::UString &base) {
	const Aurora::TwoDARow &appearance = TwoDAReg.get2DA("appearance").getRow(_appearanceID);

	Common::UString baseModel = appearance.getString(base);

	// Male/Female
	baseModel.replaceAll('?', isFemale() ? 'F' : 'M');

	return baseModel;
}

bool Creature::loadArmorModel(const Common::UString &body,
		const Common::UString &armor, uint8 visualType, uint8 variation) {

	const Aurora::TwoDARow &armorVisual = TwoDAReg.get2DA("armorvisualdata").getRow(visualType);
	Common::UString armorPrefix = armorVisual.getString("Prefix");

	Common::UString modelFile;
	modelFile = Common::UString::format("%s_%s_%s%02d",
	                                     body.c_str(), armorPrefix.c_str(), armor.c_str(), variation + 1);

	Graphics::Aurora::Model *model = loadModelObject(modelFile);
	if (model)
		_modelParts.push_back(model);

	return model != 0;
}

bool Creature::loadHeadModel(uint8 appearance) {
	if (appearance == 0)
		return false;

	Common::UString head = getBaseModel("NWN2_Model_Head");
	if (head.empty())
		return false;

	Common::UString modelFile;
	modelFile = Common::UString::format("%s%02d", head.c_str(), appearance);

	Graphics::Aurora::Model *model = loadModelObject(modelFile);
	if (model)
		_modelParts.push_back(model);

	return model != 0;
}

bool Creature::loadHairModel(uint8 appearance) {
	if (appearance == 0)
		return false;

	Common::UString hair = getBaseModel("NWN2_Model_Hair");
	if (hair.empty())
		return false;

	Common::UString modelFile;
	modelFile = Common::UString::format("%s%02d", hair.c_str(), appearance);

	Graphics::Aurora::Model *model = loadModelObject(modelFile);
	if (model)
		_modelParts.push_back(model);

	return model != 0;
}

void Creature::loadModel() {
	if (!_modelParts.empty())
		return;

	if (_appearanceID == Aurora::kFieldIDInvalid) {
		warning("Creature \"%s\" has no appearance", _tag.c_str());
		return;
	}

	Common::UString body = getBaseModel("NWN2_Model_Body");
	if (body.empty()) {
		warning("Creature \"%s\" has no body", _tag.c_str());
		return;
	}

	// Main body model
	loadArmorModel(body, "BODY", _armorVisualType, _armorVariation);

	const Aurora::TwoDARow &appearance = TwoDAReg.get2DA("appearance").getRow(_appearanceID);
	if (appearance.getInt("BodyType") == 1) {
		// Creature with more part models than just the body

		loadHeadModel(_appearanceHead);
		loadHairModel(_appearanceMHair);
		loadHairModel(_appearanceFHair);

		loadArmorModel(body, "BOOTS" , _bootsVisualType, _bootsVariation);
		if (!loadArmorModel(body, "GLOVES", 10, 0))
			loadArmorModel(body, "GLOVES", 0, 0);
	}

	// Positioning

	float x, y, z, angle;

	getPosition(x, y, z);
	setPosition(x, y, z);

	getOrientation(x, y, z, angle);
	setOrientation(x, y, z, angle);

	for (ModelParts::iterator m = _modelParts.begin(); m != _modelParts.end(); ++m) {
		(*m)->setTag(_tag);
		(*m)->setClickable(isClickable());

		_ids.push_back((*m)->getID());
	}
}

void Creature::unloadModel() {
	hide();

	_modelParts.clear();
}

void Creature::load(const Aurora::GFF3Struct &creature) {
	Common::UString temp = creature.getString("TemplateResRef");

	Common::ScopedPtr<Aurora::GFF3File> utc;
	if (!temp.empty())
		utc.reset(loadOptionalGFF3(temp, Aurora::kFileTypeUTC, MKTAG('U', 'T', 'C', ' ')));

	load(creature, utc ? &utc->getTopLevel() : 0);
}

void Creature::loadCharacter(const Common::UString &bic, bool local) {
	Common::ScopedPtr<Aurora::GFF3File> gff(openPC(bic, local));

	load(gff->getTopLevel(), 0);

	// All BICs should be PCs.
	_isPC = true;
	_faction = kStandardPC;

	// Set the PC tag to something recognizable for now.
	// Let's hope no script depends on it being "".

	_tag = Common::UString::format("[PC: %s]", _name.c_str());
}

void Creature::load(const Aurora::GFF3Struct &instance, const Aurora::GFF3Struct *blueprint) {
	// General properties

	if (blueprint)
		loadProperties(*blueprint); // Blueprint
	loadProperties(instance);    // Instance

	// Position

	setPosition(instance.getDouble("XPosition"),
	            instance.getDouble("YPosition"),
	            instance.getDouble("ZPosition"));

	// Orientation

	float bearingX = instance.getDouble("XOrientation");
	float bearingY = instance.getDouble("YOrientation");

	setOrientation(0.0f, 0.0f, 1.0f, -Common::rad2deg(atan2(bearingX, bearingY)));
}

void Creature::loadProperties(const Aurora::GFF3Struct &gff) {
	// Tag

	_tag = gff.getString("Tag", _tag);

	// Name

	_firstName = gff.getString("FirstName", _firstName);
	_lastName  = gff.getString("LastName" , _lastName);

	_name = _firstName + " " + _lastName;
	_name.trim();

	// Description

	_description = gff.getString("Description", _description);

	// Conversation

	_conversation = gff.getString("Conversation", _conversation);

	// Sound Set

	_soundSet = gff.getUint("SoundSetFile", Aurora::kFieldIDInvalid);

	// Gender
	_gender = gff.getUint("Gender", _gender);

	// Race
	_race = gff.getUint("Race", _race);

	// Subrace
	_subRace = gff.getUint("Subrace", _subRace);

	// PC and DM
	_isPC = gff.getBool("IsPC", _isPC);
	_isDM = gff.getBool("IsDM", _isDM);

	// Bit flags
	_isImmortal = gff.getBool("IsImmortal", _isImmortal);
	_isPlot = gff.getBool("Plot", _isPlot);
	_isLootable = gff.getBool("Lootable", _isLootable);
	_isDisarmable = gff.getBool("Disarmable", _isDisarmable);
	_isInterruptable = gff.getBool("Interruptable", _isInterruptable);
	_isNoPermanentDeath = gff.getBool("NoPermDeath", _isNoPermanentDeath);
	_isSelectableWhenDead = gff.getBool("DeadSelectable", _isSelectableWhenDead);

	// Age
	_age = gff.getUint("Age", _age);

	// Experience
	_xp = gff.getUint("Experience", _xp);

	// Abilities
	_abilities[kAbilityStrength]     = gff.getUint("Str", _abilities[kAbilityStrength]);
	_abilities[kAbilityDexterity]    = gff.getUint("Dex", _abilities[kAbilityDexterity]);
	_abilities[kAbilityConstitution] = gff.getUint("Con", _abilities[kAbilityConstitution]);
	_abilities[kAbilityIntelligence] = gff.getUint("Int", _abilities[kAbilityIntelligence]);
	_abilities[kAbilityWisdom]       = gff.getUint("Wis", _abilities[kAbilityWisdom]);
	_abilities[kAbilityCharisma]     = gff.getUint("Cha", _abilities[kAbilityCharisma]);

	// Classes
	loadClasses(gff, _classes, _hitDice);

	// Levels and/or feats
	_feats.reset(new Feats());
	if (gff.hasField("LvlStatList")) {
		// Player characters have individual level stats
		loadLevelStats(gff, _levels, _feats);
	} else {
		// Creatures have a combined list of feats
		loadFeats(gff, _feats);
	}

	/**
	 * The 'SkillsList' under the top level is a cumulative list of
	 * skill ranks for the creature. The 'LvlStatList' has 'SkillsList'
	 * arrays for the individual levels and those are stored separately.
	 */
	loadSkills(gff, _ranks);

	// Listening patterns
	loadListenPatterns(gff);

	// Deity
	_deity = gff.getString("Deity", _deity);

	// Health
	if (gff.hasField("HitPoints")) {
		_baseHP    = gff.getSint("HitPoints");
		_bonusHP   = gff.getSint("MaxHitPoints", _baseHP) - _baseHP;
		_currentHP = gff.getSint("CurrentHitPoints", _baseHP);
	}

	// Faction
	_faction = gff.getUint("FactionID", _faction);

	// Alignment

	_goodEvil = gff.getUint("GoodEvil", _goodEvil);
	_lawChaos = gff.getUint("LawfulChaotic", _lawChaos);

	// Appearance

	_appearanceID = gff.getUint("Appearance_Type", _appearanceID);

	_appearanceHead  = gff.getUint("Appearance_Head" , _appearanceHead);
	_appearanceMHair = gff.getUint("Appearance_Hair" , _appearanceMHair);
	_appearanceFHair = gff.getUint("Appearance_FHair", _appearanceFHair);

	_armorVisualType = gff.getUint("ArmorVisualType", _armorVisualType);
	_armorVariation  = gff.getUint("Variation"      , _armorVariation);

	if (gff.hasField("Boots")) {
		const Aurora::GFF3Struct &boots = gff.getStruct("Boots");

		_bootsVisualType = boots.getUint("ArmorVisualType", _bootsVisualType);
		_bootsVariation  = boots.getUint("Variation"      , _bootsVariation);
	}

	// Scripts and variables
	readScripts(gff);
	readVarTable(gff);
}

void Creature::loadClasses(const Aurora::GFF3Struct &gff,
                           std::vector<Class> &classes, uint8 &hitDice) {

	if (!gff.hasField("ClassList"))
		return;

	classes.clear();
	hitDice = 0;

	const Aurora::GFF3List &cClasses = gff.getList("ClassList");
	for (Aurora::GFF3List::const_iterator c = cClasses.begin(); c != cClasses.end(); ++c) {
		classes.push_back(Class());

		const Aurora::GFF3Struct &cClass = **c;

		classes.back().classID = cClass.getUint("Class");
		classes.back().level   = cClass.getUint("ClassLevel");

		hitDice += classes.back().level;
	}
}

void Creature::loadLevelStats(const Aurora::GFF3Struct &gff,
                              std::vector<LevelStats> &levelStats,
                              Common::ScopedPtr<Feats> &feats) {

	if (!gff.hasField("LvlStatList"))
		return;

	levelStats.clear();

	uint32 level = 0;
	const Aurora::GFF3List &cLvlStatList = gff.getList("LvlStatList");
	for (Aurora::GFF3List::const_iterator c = cLvlStatList.begin(); c != cLvlStatList.end(); ++c) {
		levelStats.push_back(LevelStats());
		level++;

		// Initialization
		for (int i = 0; i < kSkillMAX; i++)
			levelStats.back().ranks[i] = 0;

		const Aurora::GFF3Struct &cLevelStats = **c;

		levelStats.back().hitDice = cLevelStats.getUint("LvlStatHitDie");
		levelStats.back().classID = cLevelStats.getUint("LvlStatClass");
		levelStats.back().skillPoints = cLevelStats.getUint("SkillPoints");

		// Ability to increase
		if (cLevelStats.hasField("LvlStatAbility")) {
			levelStats.back().ability = cLevelStats.getUint("LvlStatAbility");
		} else {
			levelStats.back().ability = kAbilityMAX; // Set to an invalid ability
		}

		// Skill ranks
		loadSkills(cLevelStats, levelStats.back().ranks);

		// Feats
		loadFeats(cLevelStats, feats, level);
	}
}

void Creature::loadSkills(const Aurora::GFF3Struct &gff,
                          uint8 ranks[]) {

	if (!gff.hasField("SkillList"))
		return;

	uint32 i = 0;
	const Aurora::GFF3List &skills = gff.getList("SkillList");
	for (Aurora::GFF3List::const_iterator s = skills.begin(); s != skills.end(); ++s) {
		const Aurora::GFF3Struct &skill = **s;

		ranks[i++] = skill.getSint("Rank");
	}
}

void Creature::loadFeats(const Aurora::GFF3Struct &gff,
                         Common::ScopedPtr<Feats> &feats,
                         uint32 level) {

	if (!gff.hasField("FeatList"))
		return;

	const Aurora::GFF3List &featList = gff.getList("FeatList");
	for (Aurora::GFF3List::const_iterator f = featList.begin(); f != featList.end(); ++f) {
		const Aurora::GFF3Struct &feat = **f;

		feats->featAdd(feat.getUint("Feat"), level);
	}
}

void Creature::loadListenPatterns(const Aurora::GFF3Struct &gff) {

	setListening(gff.getBool("Listening", getIsListening()));

	if (!gff.hasField("ExpressionList"))
		return;

	const Aurora::GFF3List &patternList = gff.getList("ExpressionList");
	for (Aurora::GFF3List::const_iterator p = patternList.begin(); p != patternList.end(); ++p) {
		const Aurora::GFF3Struct &pattern = **p;

		setListenPattern(pattern.getString("ExpressionString"), pattern.getSint("ExpressionId"));
	}
}

void Creature::getClass(uint32 position, uint32 &classID, uint16 &level) const {
	if (position >= _classes.size()) {
		classID = kClassInvalid;
		level   = 0;
		return;
	}

	classID = _classes[position].classID;
	level   = _classes[position].level;
}

uint16 Creature::getClassLevel(uint32 classID) const {
	for (std::vector<Class>::const_iterator c = _classes.begin(); c != _classes.end(); ++c)
		if (c->classID == classID)
			return c->level;

	return 0;
}

const Common::UString &Creature::getDeity() const {
	return _deity;
}

uint8 Creature::getGoodEvil() const {
	return _goodEvil;
}

uint8 Creature::getLawChaos() const {
	return _lawChaos;
}

uint8 Creature::getHitDice() const {
	return _hitDice;
}

uint8 Creature::getAbility(Ability ability) const {
	assert((ability >= 0) && (ability < kAbilityMAX));

	return _abilities[ability];
}

int8 Creature::getAbilityModifier(Ability ability) const {
	assert((ability >= 0) && (ability < kAbilityMAX));

	return floor((_abilities[ability] - 10) / 2);
}

/** Return true if skill is valid and useable */
bool Creature::getHasSkill(uint32 skill) const {
	if (skill >= kSkillMAX)
		return false;

	bool hasSkill = true;
	if (getSkillRank(skill, true) < 1) {
		// Trained skills require at least one rank
		switch (skill) {
			case kSkillAnimalEmpathy:
			case kSkillDisableDevice:
			case kSkillOpenLock:
			case kSkillPerform:
			case kSkillSetTrap:
			case kSkillSleightOfHand:
			case kSkillSpellcraft:
			case kSkillTumble:
			case kSkillUseMagicDevice:
				// Requires 1+ trained ranks
				hasSkill = false;
				break;

			default:
				break;
		}
	}
	return hasSkill;
}

int8 Creature::getSkillRank(uint32 skill, bool baseOnly) const {
	if (skill >= kSkillMAX)
		return -1;

	// Check for the baseOnly option
	if (baseOnly)
		return _ranks[skill];

	// Check for skill synergies
	int modSynergy = 0;
	switch (skill) {
		case kSkillDisableDevice:
			if (getSkillRank(kSkillSetTrap, true) > 4)
				modSynergy = +2;
			break;
		case kSkillSetTrap:
			if (getSkillRank(kSkillDisableDevice, true) > 4)
				modSynergy = +2;
			break;
		case kSkillSurvival:
			if (getSkillRank(kSkillSearch, true) > 4)
				modSynergy = +2;
			break;
		default:
			// Bluff synergies weren't implemented in original
			break;
	}

	// Get the ability modifier for the skill
	int modAbility = 0;
	switch (skill) {
		case kSkillDisableDevice:
		case kSkillHide:
		case kSkillMoveSilently:
		case kSkillOpenLock:
		case kSkillParry:
		case kSkillSetTrap:
		case kSkillSleightOfHand:
		case kSkillTumble:
			// Dexterity skills
			modAbility = getAbilityModifier(kAbilityDexterity);
			break;
		case kSkillConcentration:
			// Constitution skills
			modAbility = getAbilityModifier(kAbilityConstitution);
			break;
		case kSkillAppraise:
		case kSkillCraftAlchemy:
		case kSkillCraftArmor:
		case kSkillCraftTrap:
		case kSkillCraftWeapon:
		case kSkillLore:
		case kSkillSearch:
		case kSkillSpellcraft:
			// Intelligence skills
			modAbility = getAbilityModifier(kAbilityIntelligence);
			break;
		case kSkillHeal:
		case kSkillListen:
		case kSkillSpot:
		case kSkillSurvival:
			// Wisdom skills
			modAbility = getAbilityModifier(kAbilityWisdom);
			break;
		case kSkillBluff:
		case kSkillDiplomacy:
		case kSkillIntimidate:
		case kSkillPerform:
		case kSkillTaunt:
		case kSkillUseMagicDevice:
			// Charisma skills
			modAbility = getAbilityModifier(kAbilityCharisma);
			break;
		default:
			break;
	}

	// Get the cumulative feats skill modifier
	int modFeats = _feats->getFeatsSkillBonus(skill);

	// Add custom feat modifiers
	switch (skill) {
		case kSkillSearch:
			// Nature sense: +2 while in natural area
			if (_feats->getHasCustomFeat(Feats::kCustomNatureSense))
				if (getArea()->getIsAreaNatural())
					modFeats += 2;

			// Stonecunning: +2 while in interior area
			if (_feats->getHasCustomFeat(Feats::kCustomStonecunning))
				if (getArea()->getIsAreaInterior())
					modFeats += 2;
			break;
		case kSkillSpot:
			// Nature sense: +2 while in natural area
			if (_feats->getHasCustomFeat(Feats::kCustomNatureSense))
				if (getArea()->getIsAreaNatural())
					modFeats += 2;
			break;
		default:
			break;
	}

	// Return the modified ranks
	return _ranks[skill] + modSynergy + modAbility + modFeats;
}

bool Creature::hasFeat(uint32 feat) const {
	return _feats->getHasFeat(feat);
}

bool Creature::featAdd(uint32 feat, bool checkRequirements) {
	if (checkRequirements) {
		if (!_feats->meetsRequirements(*this, feat))
			return false;
	}

	// Add feat at the current hit dice
	_feats->featAdd(feat, getHitDice());
	return true;
}

/**
 * This call is equivalent to the GetReputation() script
 * function. It returns how the source feels about this
 * creature. If this creature has a PersonalReputation
 * struct for the source and that struct hasn't
 * decayed, return that reputation. Otherwise, return
 * this creature's reputation with the source's faction.
 */
uint8 Creature::getReputation(Object *source) const {
	assert(source);

	// Get this creature's reputation with the source
	uint8 repute = 0;
	if (!_personalRep->getPersonalRep(source, &repute))
		repute = _area->getFactionReputation(source, _faction);

	return repute;
}

Aurora::GFF3File *Creature::openPC(const Common::UString &bic, bool local) {
	const Common::UString pcDir  = ConfigMan.getString(local ? "NWN2_localPCDir" : "NWN2_serverPCDir");
	const Common::UString pcFile = pcDir + "/" + bic + ".bic";

	return new Aurora::GFF3File(new Common::ReadFile(pcFile), kBICID);
}

} // End of namespace NWN2

} // End of namespace Engines
