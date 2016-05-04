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
 *  The character choices in the character generator.
 */

#include "src/common/util.h"

#include "src/aurora/2dareg.h"
#include "src/aurora/2dafile.h"

#include "src/engines/nwn/creature.h"
#include "src/engines/nwn/module.h"

#include "src/engines/nwn/gui/chargen/chargenchoices.h"

namespace Engines {

namespace NWN {

CharGenChoices::CharGenChoices() : _characterUsed(false) {
	_creature = new Creature();

	init();
}

CharGenChoices::~CharGenChoices() {
	if (!_characterUsed)
		delete _creature;
}

void CharGenChoices::init() {
	_classId = 0;

	_goodness = 101;
	_loyalty = 101;

	_abilities.assign(6, 8);
	_racialAbilities.assign(6, 0);

	_package = UINT8_MAX;
	_skills.assign(28, 0);

	_spellSchool = UINT8_MAX;

	_domain1 = UINT8_MAX;
	_domain2 = UINT8_MAX;
}

void CharGenChoices::applyChoices() {
	// Rise level
	_creature->changeClassLevel(_classId, 1);

	// Set Alignment
	_creature->setGoodEvil(_goodness);
	_creature->setLawChaos(_loyalty);

	// Set Abilities
	size_t ab = 0;
	for (;ab < 6; ++ab) {
		Ability ability = static_cast<Ability>(ab);
		_creature->setAbility(ability, _abilities[ab] + _racialAbilities[ab]);
	}

	// Set package
	if (_package != UINT8_MAX)
		_creature->setStartingPackage(_package);

	//TODO Apply package

	// Set skills
	size_t skillID = 0;
	for (std::vector<uint8>::iterator s = _skills.begin(); s != _skills.end(); ++s, ++skillID)
		_creature->setSkillRank(skillID, *s);

	// Set feats
	for (std::vector<uint32>::iterator f = _classFeats.begin(); f != _classFeats.end(); ++f)
		_creature->setFeat(*f);

	for (std::vector<uint32>::iterator f = _racialFeats.begin(); f != _racialFeats.end(); ++f)
		_creature->setFeat(*f);

	for (std::vector<uint32>::iterator f = _normalFeats.begin(); f != _normalFeats.end(); ++f)
		_creature->setFeat(*f);

	// Set domains if any.
	if (_domain1 != UINT8_MAX && _domain2 != UINT8_MAX)
		_creature->setDomains(_classId, _domain1, _domain2);

	// Set spells if any.
	if (!_spells.empty()) {
		for (size_t lvl = 0; lvl < _spells.size(); ++lvl) {
			for (size_t s = 0; s < _spells[lvl].size(); ++s)
				_creature->setKnownSpell(_classId, lvl, _spells[lvl][s]);
		}

		if (_spellSchool != UINT8_MAX)
			_creature->setSchool(_classId, _spellSchool);
	}

	// Set sound set.
	_creature->setSoundSet(_soundSet);
}

void CharGenChoices::reset() {
	delete _creature;
	_creature = new Creature();

	init();

	_racialFeats.clear();
	_normalFeats.clear();
	_classFeats.clear();
}

const Creature &CharGenChoices::getCharacter() {
	return *_creature;
}

void CharGenChoices::setGender(Gender gender) {
	_creature->setGender(gender);
}

void CharGenChoices::setRace(uint32 race) {
	if (race == kRaceInvalid) {
		warning("Setting invalid race: %d", race);
		return;
	}

	_creature->setRace(race);

	_racialFeats.clear();

	const Aurora::TwoDAFile &twodaRace = TwoDAReg.get2DA("racialtypes");
	const Aurora::TwoDAFile &twodaFeatRace = TwoDAReg.get2DA(
		twodaRace.getRow(race).getString("FeatsTable"));

	for (size_t it = 0; it < twodaFeatRace.getRowCount(); ++it) {
		const Aurora::TwoDARow &rowFeatRace = twodaFeatRace.getRow(it);
		_racialFeats.push_back(rowFeatRace.getInt("FeatIndex"));
	}
}

void CharGenChoices::setPortrait(const Common::UString &portrait) {
	_creature->setPortrait(portrait);
}

void CharGenChoices::setClass(uint32 classId) {
	_classId = classId;

	// Add granted class feats.
	_classFeats.clear();
	const Aurora::TwoDAFile &twodaClasses = TwoDAReg.get2DA("classes");
	const Aurora::TwoDAFile &twodaClsFeat = TwoDAReg.get2DA(twodaClasses.getRow(classId).getString("FeatsTable"));
	for (size_t it = 0; it < twodaClsFeat.getRowCount(); ++it) {
		const Aurora::TwoDARow &rowFeat = twodaClsFeat.getRow(it);
		if (rowFeat.getInt("List") != 3)
			continue;

		if (rowFeat.getInt("GrantedOnLevel") != _creature->getHitDice() + 1)
			continue;

		if (!hasFeat(rowFeat.getInt("FeatIndex")))
			_classFeats.push_back(rowFeat.getInt("FeatIndex"));
	}
}

void CharGenChoices::setAlign(uint8 goodness, uint8 loyalty) {
	_goodness = goodness;
	_loyalty  = loyalty;
}

void CharGenChoices::setAbilities(std::vector<uint8> abilities,
                                  std::vector<uint8> racialAbilities) {
	_abilities = abilities;
	_racialAbilities = racialAbilities;
}

void CharGenChoices::setPackage(uint8 package) {
	_package = package;
}

void CharGenChoices::setSkill(size_t skillIndex, uint8 rank) {
	_skills[skillIndex] = rank;
}

void CharGenChoices::setNotUsedSkills(uint8 notUsedSkills) {
	_notUsedSkills = notUsedSkills;
}

void CharGenChoices::setFeat(uint32 feat) {
	_normalFeats.push_back(feat);
}

void CharGenChoices::setSpellSchool(uint8 spellSchool) {
	_spellSchool = spellSchool;
}

void CharGenChoices::setDomains(uint8 domain1, uint8 domain2) {
	_domain1 = domain1;
	_domain2 = domain2;
}

void CharGenChoices::setSpell(size_t spellLevel, uint16 spell) {
	if (_spells.size() < spellLevel + 1)
		_spells.resize(spellLevel + 1);

	_spells[spellLevel].push_back(spell);
}

void CharGenChoices::setSoundSet(uint32 soundSetID) {
	_soundSet = soundSetID;
}

void CharGenChoices::useCharacter(Module *module) {
	module->usePC(_creature);
	_characterUsed = true;
}

bool CharGenChoices::hasFeat(uint32 featId) const {
	for (std::vector<uint32>::const_iterator f = _normalFeats.begin(); f != _normalFeats.end(); ++f)
		if (*f == featId)
			return true;

	for (std::vector<uint32>::const_iterator f = _racialFeats.begin(); f != _racialFeats.end(); ++f)
		if (*f == featId)
			return true;

	for (std::vector<uint32>::const_iterator f = _classFeats.begin(); f != _classFeats.end(); ++f)
		if (*f == featId)
			return true;

	return _creature->hasFeat(featId);
}

bool CharGenChoices::hasPrereqFeat(uint32 featId, bool isClassFeat) {
	const Aurora::TwoDAFile &twodaFeats = TwoDAReg.get2DA("feat");
	const Aurora::TwoDARow  &row        = twodaFeats.getRow(featId);

	// Some feats have been removed. Check if it's the case.
	if (row.empty("FEAT"))
		return false;

	if (!row.getInt("ALLCLASSESCANUSE") && !isClassFeat)
		return false;

	// Check abilities.
	if ((uint32) row.getInt("MINSTR") > getAbility(kAbilityStrength))
		return false;
	if ((uint32) row.getInt("MINDEX") > getAbility(kAbilityDexterity))
		return false;
	if ((uint32) row.getInt("MININT") > getAbility(kAbilityIntelligence))
		return false;
	if ((uint32) row.getInt("MINWIS") > getAbility(kAbilityWisdom))
		return false;
	if ((uint32) row.getInt("MINCHA") > getAbility(kAbilityCharisma))
		return false;
	if ((uint32) row.getInt("MINCON") > getAbility(kAbilityConstitution))
		return false;

	// Check if the character has the prerequisite feats.
	if (!row.empty("PREREQFEAT1") && !hasFeat(row.getInt("PREREQFEAT1")))
		return false;
	if (!row.empty("PREREQFEAT2") && !hasFeat(row.getInt("PREREQFEAT2")))
		return false;

	if (!row.empty("OrReqFeat0")) {
		bool OrReqFeat = hasFeat(row.getInt("OrReqFeat0"));
		if (!row.empty("OrReqFeat1")) {
			OrReqFeat = OrReqFeat || hasFeat(row.getInt("OrReqFeat1"));
			if (!row.empty("OrReqFeat2")) {
				OrReqFeat = OrReqFeat || hasFeat(row.getInt("OrReqFeat2"));
				if (!row.empty("OrReqFeat3")) {
					OrReqFeat = OrReqFeat || hasFeat(row.getInt("OrReqFeat3"));
					if (!row.empty("OrReqFeat4"))
						OrReqFeat = OrReqFeat || hasFeat(row.getInt("OrReqFeat4"));
				}
			}
		}

		if (!OrReqFeat)
			return false;
	}

	// TODO Check base bonus attack
	if (row.getInt("PreReqEpic") > 0 && _creature->getHitDice() < 21)
		return false;

	// Check minimun level.
	if ((_creature->getClassLevel(row.getInt("MinLevelClass")) < row.getInt("MinLevel")) && !row.empty("MinLevel"))
		return false;

	// Check maximum level.
	if ((_creature->getHitDice() >= row.getInt("MaxLevel")) && !row.empty("MaxLevel"))
		return false;

	// Check skill rank.
	if (!row.empty("REQSKILL")) {
		if (_skills[row.getInt("REQSKILL")] == 0)
			return false;
		if ((row.getInt("ReqSkillMinRanks") > _skills[row.getInt("REQSKILL")]) && !row.empty("ReqSkillMinRanks"))
			return false;
	}
	if (!row.empty("REQSKILL2")) {
		if (_skills[row.getInt("REQSKILL2")] == 0)
			return false;
		if ((row.getInt("ReqSkillMinRanks2") > _skills[row.getInt("REQSKILL2")]) && !row.empty("ReqSkillMinRanks2"))
			return false;
	}

	// Check if the character already has the feat.
	if (hasFeat(featId)) {
		if (!row.getInt("GAINMULTIPLE"))
			return false;
	}

	// TODO Check spell level
	// TODO Check saving throw
	return true;
}

uint32 CharGenChoices::getClass() const {
	return _classId;
}

uint32 CharGenChoices::getRace() const {
	return _creature->getRace();
}

bool CharGenChoices::getAlign(uint8 &goodness, uint8 &loyalty) const {
	// Check if alignment has been previously set.
	if (_goodness > 100)
		return false;

	goodness = _goodness;
	loyalty  = _loyalty;
	return true;
}

uint8 CharGenChoices::getAbility(size_t ability) const {
	return _abilities[ability];
}

uint8 CharGenChoices::getTotalAbility(size_t ability) const {
	return _abilities[ability] + _racialAbilities[ability];
}

int32 CharGenChoices::getAbilityModifier(Ability ability) {
	uint32 totalAbility = _abilities[ability] + _racialAbilities[ability];
	totalAbility -= 6;
	int32 modifier = (totalAbility - totalAbility % 2) / 2;
	modifier -= 2;
	return modifier;
}

uint8 CharGenChoices::getPackage() const {
	return _package;
}

uint32 CharGenChoices::getSpellSchool() const {
	return _spellSchool;
}

std::vector<uint32> CharGenChoices::getFeats() {
	std::vector<uint32> allFeats(_racialFeats);
	allFeats.insert(allFeats.end(), _classFeats.begin(), _classFeats.end());
	return allFeats;
}

} // End of namespace NWN

} // End of namespace Engines
