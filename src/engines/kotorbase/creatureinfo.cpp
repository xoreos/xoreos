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
 *  Abstract creature information in KotOR games.
 */

#include <algorithm>

#include "src/common/util.h"
#include "src/common/error.h"

#include "src/aurora/gff3file.h"

#include "src/engines/kotorbase/creatureinfo.h"

#include "src/engines/kotorbase/gui/chargeninfo.h"

namespace Engines {

namespace KotORBase {

CreatureInfo::ClassLevel::ClassLevel() {
}

CreatureInfo::ClassLevel::ClassLevel(const ClassLevel &other) {
	operator=(other);
}

CreatureInfo::ClassLevel &CreatureInfo::ClassLevel::operator=(const ClassLevel &other) {
	characterClass = other.characterClass;
	level = other.level;

	return *this;
}

CreatureInfo::Skills &CreatureInfo::Skills::operator=(const Skills &other) {
	computerUse = other.computerUse;
	demolitions = other.demolitions;
	stealth = other.stealth;
	awareness = other.awareness;
	persuade = other.persuade;
	repair = other.repair;
	security = other.security;
	treatInjury = other.treatInjury;

	return *this;
}

CreatureInfo::Abilities &CreatureInfo::Abilities::operator=(const Abilities &other) {
	strength = other.strength;
	dexterity = other.dexterity;
	constitution = other.constitution;
	intelligence = other.intelligence;
	wisdom = other.wisdom;
	charisma = other.charisma;

	return *this;
}

CreatureInfo::CreatureInfo() {
}

CreatureInfo::CreatureInfo(const Aurora::GFF3Struct &gff) {
	loadClassLevels(gff);
	loadSkills(gff);
	loadAbilities(gff);
}

CreatureInfo::CreatureInfo(const CharacterGenerationInfo &info) {
	_levels.resize(1);
	_levels[0].characterClass = info.getClass();
	_levels[0].level = 1;
	_abilities = info.getAbilities();
}

CreatureInfo &CreatureInfo::operator=(const CreatureInfo &other) {
	_levels = other._levels;
	_skills = other._skills;
	_abilities = other._abilities;
	_inventory = other._inventory;
	_equipment = other._equipment;

	return *this;
}

int CreatureInfo::getClassLevel(Class charClass) const {
	auto it = std::find_if(_levels.begin(), _levels.end(), [&](const ClassLevel &cl) {
		return cl.characterClass == charClass;
	});

	if (it == _levels.end()) {
		warning("CreatureInfo::getClassLevel(): Invalid character class \"%d\"", charClass);
		return 0;
	}

	return it->level;
}

Class CreatureInfo::getClassByPosition(int position) const {
	if (position >= static_cast<int>(_levels.size())) {
		warning("CreatureInfo::getClassByPosition(): Invalid position \"%d\"", position);
		return kClassInvalid;
	}

	return _levels[position].characterClass;
}

int CreatureInfo::getLevelByPosition(int position) const {
	if (position >= static_cast<int>(_levels.size())) {
		warning("CreatureInfo::getLevelByPosition(): Invalid position \"%d\"", position);
		return 0;
	}

	return _levels[position].level;
}

int CreatureInfo::getAbilityScore(Ability ability) const {
	switch (ability) {
		case kAbilityStrength:
			return _abilities.strength;
		case kAbilityDexterity:
			return _abilities.dexterity;
		case kAbilityConstitution:
			return _abilities.constitution;
		case kAbilityIntelligence:
			return _abilities.intelligence;
		case kAbilityWisdom:
			return _abilities.wisdom;
		case kAbilityCharisma:
			return _abilities.charisma;
		default:
			warning("CreatureInfo::getAbilityScore(): Invalid ability \"%d\"", ability);
			return 0;
	}
}

int CreatureInfo::getAbilityModifier(Ability ability) const {
	int score;

	switch (ability) {
		case kAbilityStrength:
			score = _abilities.strength;
			break;
		case kAbilityDexterity:
			score = _abilities.dexterity;
			break;
		case kAbilityConstitution:
			score = _abilities.constitution;
			break;
		case kAbilityIntelligence:
			score = _abilities.intelligence;
			break;
		case kAbilityWisdom:
			score = _abilities.wisdom;
			break;
		case kAbilityCharisma:
			score = _abilities.charisma;
			break;
		default:
			return -1;
	}

	return (score - 10) / 2;
}

void CreatureInfo::setAbilityScore(Ability ability, uint32_t score) {
	switch (ability) {
		case kAbilityStrength:
			_abilities.strength = score;
			break;
		case kAbilityDexterity:
			_abilities.dexterity = score;
			break;
		case kAbilityConstitution:
			_abilities.constitution = score;
			break;
		case kAbilityIntelligence:
			_abilities.intelligence = score;
			break;
		case kAbilityWisdom:
			_abilities.wisdom = score;
			break;
		case kAbilityCharisma:
			_abilities.charisma = score;
			break;
		default:
			warning("CreatureInfo::setAbilityScore(): Invalid ability \"%d\"", ability);
			break;
	}
}

int CreatureInfo::getSkillRank(Skill skill) const {
	switch (skill) {
		case kSkillComputerUse:
			return _skills.computerUse;
		case kSkillDemolitions:
			return _skills.demolitions;
		case kSkillStealth:
			return _skills.stealth;
		case kSkillAwareness:
			return _skills.awareness;
		case kSkillPersuade:
			return _skills.persuade;
		case kSkillRepair:
			return _skills.repair;
		case kSkillSecurity:
			return _skills.security;
		case kSkillTreatInjury:
			return _skills.treatInjury;
		default:
			warning("CreatureInfo::getSkillRank(): Invalid skill \"%d\"", skill);
			return -1;
	}
}

void CreatureInfo::setSkillRank(Skill skill, uint32_t rank) {
	switch (skill) {
		case kSkillComputerUse:
			_skills.computerUse = rank;
			break;
		case kSkillDemolitions:
			_skills.demolitions = rank;
			break;
		case kSkillStealth:
			_skills.stealth = rank;
			break;
		case kSkillAwareness:
			_skills.awareness = rank;
			break;
		case kSkillPersuade:
			_skills.persuade = rank;
			break;
		case kSkillRepair:
			_skills.repair = rank;
			break;
		case kSkillSecurity:
			_skills.security = rank;
			break;
		case kSkillTreatInjury:
			_skills.treatInjury = rank;
			break;
		default:
			warning("CreatureInfo::setSkillRank(): Invalid skill \"%d\"", skill);
			break;
	}
}

void CreatureInfo::loadClassLevels(const Aurora::GFF3Struct &gff) {
	_levels.clear();

	if (gff.hasField("ClassList")) {
		Aurora::GFF3List classList = gff.getList("ClassList");
		for (const auto &charClass : classList) {
			ClassLevel classLevel;
			classLevel.characterClass = Class(charClass->getSint("Class"));
			classLevel.level = charClass->getSint("ClassLevel");
			_levels.push_back(classLevel);
		}
	}
}

void CreatureInfo::loadSkills(const Aurora::GFF3Struct &gff) {
	if (gff.hasField("SkillList")) {
		Aurora::GFF3List skillList = gff.getList("SkillList");
		int index = 0;
		for (const auto &skill : skillList) {
			setSkillRank(Skill(index++), skill->getUint("Rank"));
		}
	}
}

void CreatureInfo::loadAbilities(const Aurora::GFF3Struct &gff) {
	_abilities.strength     = gff.getUint("Str");
	_abilities.dexterity    = gff.getUint("Dex");
	_abilities.constitution = gff.getUint("Con");
	_abilities.intelligence = gff.getUint("Int");
	_abilities.wisdom       = gff.getUint("Wis");
	_abilities.charisma     = gff.getUint("Cha");
}

Inventory &CreatureInfo::getInventory() {
	return _inventory;
}

void CreatureInfo::addInventoryItem(const Common::UString &tag, int count) {
	_inventory.addItem(tag, count);
}

void CreatureInfo::removeInventoryItem(const Common::UString &tag, int count) {
	_inventory.removeItem(tag, count);
}

const Common::UString &CreatureInfo::getEquippedItem(InventorySlot slot) const {
	auto it = _equipment.find(slot);
	if (it == _equipment.end())
		throw Common::Exception("CreatureInfo::getEquippedItem(): Inventory slot \"%d\" is empty", slot);

	return it->second;
}

bool CreatureInfo::isInventorySlotEquipped(InventorySlot slot) const {
	return _equipment.find(slot) != _equipment.end();
}

void CreatureInfo::equipItem(const Common::UString &tag, InventorySlot slot) {
	if (_equipment.find(slot) != _equipment.end())
		throw Common::Exception("CreatureInfo::equipItem(): Inventory slot \"%d\" is not empty", slot);

	_equipment.insert(std::make_pair(slot, tag));
}

void CreatureInfo::unequipInventorySlot(InventorySlot slot) {
	_equipment.erase(slot);
}

} // End of namespace KotORBase

} // End of namespace Engines
