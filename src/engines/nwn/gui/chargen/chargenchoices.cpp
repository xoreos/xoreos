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
#include "src/aurora/gff3file.h"
#include "src/aurora/talkman.h"

#include "src/engines/nwn/creature.h"
#include "src/engines/nwn/item.h"
#include "src/engines/nwn/module.h"

#include "src/engines/nwn/gui/chargen/chargenchoices.h"

namespace Engines {

namespace NWN {

FeatItem::FeatItem() : featId(Aurora::kFieldIDInvalid), list(0), isMasterFeat(false),
    masterFeat(Aurora::kFieldIDInvalid) {
}

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
	_lawfulness = 101;

	_soundSet = 0;
	_notUsedSkills = 0;

	_abilities.assign(6, 8);
	_racialAbilities.assign(6, 0);

	resetPackage();
}

void CharGenChoices::applyChoices() {
	// Rise level
	_creature->changeClassLevel(_classId, 1);

	// Set Alignment
	_creature->setGoodEvil(_goodness);
	_creature->setLawChaos(_lawfulness);

	// Set Abilities
	size_t ab = 0;
	for (; ab < 6; ++ab) {
		Ability ability = static_cast<Ability>(ab);
		_creature->setAbility(ability, _abilities[ab] + _racialAbilities[ab]);
	}

	// Set skills
	size_t skillID = 0;
	for (std::vector<uint8_t>::iterator s = _skills.begin(); s != _skills.end(); ++s, ++skillID)
		_creature->setSkillRank(skillID, *s);

	// Set feats
	for (std::vector<uint32_t>::iterator f = _classFeats.begin(); f != _classFeats.end(); ++f)
		_creature->setFeat(*f);

	for (std::vector<uint32_t>::iterator f = _racialFeats.begin(); f != _racialFeats.end(); ++f)
		_creature->setFeat(*f);

	for (std::vector<uint32_t>::iterator f = _normalFeats.begin(); f != _normalFeats.end(); ++f)
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

	// Set appearance.
	// TODO: Apply choices from user and not default values.
	_creature->setAppearance(_creature->getRace());
	Aurora::GFF3File gff("NW_CLOTH001", Aurora::kFileTypeUTI);
	Item *armor = new Item(gff.getTopLevel());
	_creature->addEquippedItem(armor);

	_creature->setPhenotype(0);
	_creature->setColorSkin(0);
	_creature->setColorHair(0);
	_creature->setColorTatto1(0);
	_creature->setColorTatto2(0);
	_creature->setHead(1);
}

void CharGenChoices::reset() {
	delete _creature;
	_creature = new Creature();

	init();

	_racialFeats.clear();
	_classFeats.clear();

	resetPackage();
}

void CharGenChoices::resetPackage() {
	_package = UINT8_MAX;

	_skills.assign(28, 0);

	_normalFeats.clear();

	_spells.clear();
	_spellSchool = UINT8_MAX;

	_domain1 = UINT8_MAX;
	_domain2 = UINT8_MAX;
}

const Creature &CharGenChoices::getCharacter() {
	return *_creature;
}

void CharGenChoices::setGender(Gender gender) {
	_creature->setGender(gender);
}

void CharGenChoices::setRace(uint32_t race) {
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

void CharGenChoices::setClass(uint32_t classId) {
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

void CharGenChoices::setAlign(uint8_t goodness, uint8_t lawfulness) {
	_goodness   = goodness;
	_lawfulness = lawfulness;
}

void CharGenChoices::setAbilities(std::vector<uint8_t> abilities,
                                  std::vector<uint8_t> racialAbilities) {

	_abilities = abilities;
	_racialAbilities = racialAbilities;

	// Exchange intelligence and wisdom to be consistent with kAbility.
	_abilities[4] = abilities[3];
	_abilities[3] = abilities[4];
}

void CharGenChoices::setPackage(uint8_t package) {
	_package = package;

	if (_package == UINT8_MAX)
		return;

	// Set package
	_creature->setStartingPackage(_package);

	if (_package != UINT8_MAX) {
		// Set Skills
		getPrefSkills(_skills);

		// Set Feats
		std::vector<uint32_t> packFeats;
		getPrefFeats(packFeats);
		std::list<FeatItem> availFeats;
		uint8_t normalFeats, bonusFeats;
		getFeatItems(availFeats, normalFeats, bonusFeats);
		while (normalFeats + bonusFeats != 0) {
			for (std::vector<uint32_t>::iterator pF = packFeats.begin(); pF != packFeats.end(); ++pF) {
				if (normalFeats + bonusFeats == 0)
					break;
				for (std::list<FeatItem>::iterator aF = availFeats.begin(); aF != availFeats.end(); ++aF) {
					if (*pF != (*aF).featId)
						continue;

					// For bonus feats and bonus feats that can be chosen as a general feat.
					if (bonusFeats > 0 && (*aF).list > 0) {
						--bonusFeats;
						_normalFeats.push_back(*pF);
					}

					// For general feat and bonus feats that can be chosen as a general feat.
					if ((normalFeats > 0 && (*aF).list == 0) ||
					    (normalFeats > 0 && (*aF).list < 2 && bonusFeats == 0)) {
						--normalFeats;
						_normalFeats.push_back(*pF);
					}

					break;
				}
			}
		}

		// For spell casters
		const Aurora::TwoDAFile &twodaClasses = TwoDAReg.get2DA("classes");
		const Aurora::TwoDARow &rowClass = twodaClasses.getRow(_classId);
		if (rowClass.getInt("SpellCaster") > 0) {
			if (rowClass.getString("SpellGainTable") == "CLS_SPGN_WIZ" &&
			    _creature->getHitDice() == 0) {
				// Set school
				setSpellSchool(getPrefSpellSchool());

				// Set spells
				getPrefSpells(_spells);

			} else if (rowClass.getString("SpellGainTable") == "CLS_SPGN_CLER" &&
			           _creature->getHitDice() == 0) {
				// Set domains.
				getPrefDomains(_domain1, _domain2);
			} else if (!rowClass.empty("SpellKnownTable")) {
				// Set spells
				getPrefSpells(_spells);
			}
		}
	}
}

void CharGenChoices::setSkill(size_t skillIndex, uint8_t rank) {
	_skills[skillIndex] = rank;
}

void CharGenChoices::setNotUsedSkills(uint8_t notUsedSkills) {
	_notUsedSkills = notUsedSkills;
}

void CharGenChoices::setFeat(uint32_t feat) {
	_normalFeats.push_back(feat);
}

void CharGenChoices::setSpellSchool(uint8_t spellSchool) {
	_spellSchool = spellSchool;
}

void CharGenChoices::setDomains(uint8_t domain1, uint8_t domain2) {
	_domain1 = domain1;
	_domain2 = domain2;
}

void CharGenChoices::setSpell(size_t spellLevel, uint16_t spell) {
	if (_spells.size() < spellLevel + 1)
		_spells.resize(spellLevel + 1);

	_spells[spellLevel].push_back(spell);
}

void CharGenChoices::setSoundSet(uint32_t soundSetID) {
	_soundSet = soundSetID;
}

void CharGenChoices::useCharacter(Module *module) {
	module->usePC(_creature);
	_characterUsed = true;
}

bool CharGenChoices::hasFeat(uint32_t featId) const {
	for (std::vector<uint32_t>::const_iterator f = _normalFeats.begin(); f != _normalFeats.end(); ++f)
		if (*f == featId)
			return true;

	for (std::vector<uint32_t>::const_iterator f = _racialFeats.begin(); f != _racialFeats.end(); ++f)
		if (*f == featId)
			return true;

	for (std::vector<uint32_t>::const_iterator f = _classFeats.begin(); f != _classFeats.end(); ++f)
		if (*f == featId)
			return true;

	return _creature->hasFeat(featId);
}

bool CharGenChoices::hasPrereqFeat(uint32_t featId, bool isClassFeat) {
	const Aurora::TwoDAFile &twodaFeats = TwoDAReg.get2DA("feat");
	const Aurora::TwoDARow  &row        = twodaFeats.getRow(featId);

	// Some feats have been removed. Check if it's the case.
	if (row.empty("FEAT"))
		return false;

	if (!row.getInt("ALLCLASSESCANUSE") && !isClassFeat)
		return false;

	// Check abilities.
	if ((uint32_t) row.getInt("MINSTR") > getAbility(kAbilityStrength))
		return false;
	if ((uint32_t) row.getInt("MINDEX") > getAbility(kAbilityDexterity))
		return false;
	if ((uint32_t) row.getInt("MININT") > getAbility(kAbilityIntelligence))
		return false;
	if ((uint32_t) row.getInt("MINWIS") > getAbility(kAbilityWisdom))
		return false;
	if ((uint32_t) row.getInt("MINCHA") > getAbility(kAbilityCharisma))
		return false;
	if ((uint32_t) row.getInt("MINCON") > getAbility(kAbilityConstitution))
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

uint32_t CharGenChoices::getClass() const {
	return _classId;
}

uint32_t CharGenChoices::getRace() const {
	return _creature->getRace();
}

bool CharGenChoices::getAlign(uint8_t &goodness, uint8_t &lawfulness) const {
	// Check if alignment has been previously set.
	if (_goodness > 100)
		return false;

	goodness   = _goodness;
	lawfulness = _lawfulness;
	return true;
}

uint8_t CharGenChoices::getAbility(Ability ability) const {
	return _abilities[ability];
}

uint8_t CharGenChoices::getTotalAbility(Ability ability) const {
	return _abilities[ability] + _racialAbilities[ability];
}

int8_t CharGenChoices::getAbilityModifier(Ability ability) {
	uint8_t totalAbility = getTotalAbility(ability);
	int8_t modifier = (totalAbility - totalAbility % 2) / 2;
	modifier -= 5;
	return modifier;
}

uint8_t CharGenChoices::getPackage() const {
	return _package;
}

uint8_t CharGenChoices::getSpellSchool() const {
	return _spellSchool;
}

void CharGenChoices::getFeats(std::vector<uint32_t> &feats) {
	feats = _racialFeats;
	feats.insert(feats.end(), _classFeats.begin(), _classFeats.end());
}

uint8_t CharGenChoices::getPrefSpellSchool() {
	const Aurora::TwoDAFile &twodaPackage = TwoDAReg.get2DA("packages");
	const Aurora::TwoDARow  &row          = twodaPackage.getRow(_package == UINT8_MAX ? _classId : _package);

	if (row.empty("School"))
		return UINT8_MAX;

	return static_cast<uint8_t>(row.getInt("School"));
}

void CharGenChoices::getPrefFeats(std::vector<uint32_t> &feats) {
	const Aurora::TwoDAFile &twodaPackage    = TwoDAReg.get2DA("packages");
	const Aurora::TwoDARow  &rowPck          = twodaPackage.getRow(_package == UINT8_MAX ? _classId : _package);
	const Aurora::TwoDAFile &twodaPckFeats   = TwoDAReg.get2DA(rowPck.getString("FeatPref2DA"));

	feats.clear();
	size_t rowIdx = 0;
	while (rowIdx < twodaPckFeats.getRowCount()) {
		const Aurora::TwoDARow  &rowFeat = twodaPckFeats.getRow(rowIdx);
		++rowIdx;
		uint32_t featID = rowFeat.getInt("FEATINDEX");
		if (hasFeat(featID))
			continue;

		if (!hasPrereqFeat(featID, true))
			continue;

		feats.push_back(featID);
	}
}

void CharGenChoices::getPrefSkills(std::vector<uint8_t> &skills) {
	const Aurora::TwoDAFile &twodaPackage   = TwoDAReg.get2DA("packages");
	const Aurora::TwoDARow  &rowPck         = twodaPackage.getRow(_package == UINT8_MAX ? _classId : _package);
	const Aurora::TwoDAFile &twodaPckSkills = TwoDAReg.get2DA(rowPck.getString("SkillPref2DA"));

	skills.clear();
	for (size_t r = 0; r < twodaPckSkills.getRowCount(); ++r)
		skills.push_back((uint8_t) twodaPckSkills.getRow(r).getInt("SKILLINDEX"));
}

void CharGenChoices::getPrefDomains(uint8_t &domain1, uint8_t &domain2) {
	const Aurora::TwoDAFile &twodaPackage    = TwoDAReg.get2DA("packages");
	const Aurora::TwoDARow  &rowPck          = twodaPackage.getRow(_package == UINT8_MAX ? _classId : _package);

	domain1 = (uint8_t) rowPck.getInt("Domain1");
	domain2 = (uint8_t) rowPck.getInt("Domain2");
}

void CharGenChoices::getPrefSpells(std::vector<std::vector<uint16_t> > &spells) {
	const Aurora::TwoDAFile &twodaSpells     = TwoDAReg.get2DA("spells");
	const Aurora::TwoDAFile &twodaPackage    = TwoDAReg.get2DA("packages");
	const Aurora::TwoDARow  &rowPck          = twodaPackage.getRow(_package == UINT8_MAX ? _classId : _package);
	const Aurora::TwoDAFile &twodaPckSpells  = TwoDAReg.get2DA(rowPck.getString("SpellPref2DA"));

	std::map<uint32_t, Common::UString> spellCasterClass;
	spellCasterClass[1]  =     "Bard";
	spellCasterClass[2]  =   "Cleric";
	spellCasterClass[3]  =    "Druid";
	spellCasterClass[6]  =  "Paladin";
	spellCasterClass[7]  =   "Ranger";
	spellCasterClass[9]  = "Wiz_Sorc";
	spellCasterClass[10] = "Wiz_Sorc";

	spells.clear();
	for (size_t r = 0; r < twodaPckSpells.getRowCount(); ++r) {
		uint16_t spellIndex = twodaPckSpells.getRow(r).getInt("SpellIndex");
		const Aurora::TwoDARow  &rowSpell = twodaSpells.getRow(spellIndex);

		size_t spellLevel = rowSpell.getInt(spellCasterClass[_classId]);
		if (spells.size() < spellLevel + 1)
			spells.resize(spellLevel + 1);

		spells[spellLevel].push_back(spellIndex);
	}
}

uint8_t CharGenChoices::computeAvailSkillRank() {
	int8_t availRank = getAbilityModifier(kAbilityIntelligence);
	if (availRank < 0)
		availRank = 0;

	const Aurora::TwoDAFile &twodaPackage = TwoDAReg.get2DA("classes");
	const Aurora::TwoDARow  &rowClass       = twodaPackage.getRow(_classId);
	availRank += (int8_t) rowClass.getInt("SkillPointBase");

	// If human (have Quick to master feat), add an extra point.
	if (hasFeat(258))
		++availRank;

	if (_creature->getHitDice() == 0)
		availRank *= 4;

	return (uint8_t) availRank;
}

void CharGenChoices::getSkillItems(std::vector<SkillItem> &skills) {
	skills.clear();

	const Aurora::TwoDAFile &twodaClasses     = TwoDAReg.get2DA("classes");
	const Aurora::TwoDAFile &twodaSkills      = TwoDAReg.get2DA("skills");
	const Aurora::TwoDARow  &rowClasses       = twodaClasses.getRow(_classId);
	const Common::UString   skillsClassFile   = rowClasses.getString("SkillsTable");
	const Aurora::TwoDAFile &twoDaSkillsClass = TwoDAReg.get2DA(skillsClassFile);

	for (size_t s = 0; s < twoDaSkillsClass.getRowCount(); ++s) {
		const Aurora::TwoDARow &skillsClassRow = twoDaSkillsClass.getRow(s);
		size_t skillIndex = skillsClassRow.getInt("SkillIndex");
		const Aurora::TwoDARow &skillRow = twodaSkills.getRow(skillIndex);

		Common::UString skillName = TalkMan.getString(skillRow.getInt("Name"));
		Common::UString icon      = skillRow.getString("Icon");
		Common::UString helpText  = TalkMan.getString(skillRow.getInt("Description"));

		bool classSkill = skillsClassRow.getInt("ClassSkill") != 0;

		uint8_t maxRank = 4 + _creature->getHitDice();

		if (classSkill) {
			// Add information about wether it is a skill class.
			skillName += " " + TalkMan.getString(52951);
		} else {
			maxRank = (maxRank - maxRank % 2) / 2;
		}

		SkillItem skill;
		skill.rank         =          0;
		skill.maxRank      =    maxRank;
		skill.minRank      =          0;
		skill.skillID      = skillIndex;
		skill.isClassSkill = classSkill;
		skill.name         =  skillName;
		skill.icon         =       icon;
		skill.help         =   helpText;

		skills.push_back(skill);
	}
}

void CharGenChoices::getFeatItems(std::list<FeatItem> &feats, uint8_t &normalFeats, uint8_t &bonusFeats) {
	uint8_t level = _creature->getHitDice();
	normalFeats = 0;
	bonusFeats  = 0;

	// Get an additional feat each new level multiple of 3.
	if ((level + 1) % 3)
		++normalFeats;
	// Get an additional feat at level 1.
	if (level == 0)
		++normalFeats;

	// Bonus feat
	const Aurora::TwoDAFile &twodaClass = TwoDAReg.get2DA("classes");
	if (twodaClass.headerToColumn("BonusFeatsTable") != Aurora::kFieldIDInvalid) {
		const Aurora::TwoDAFile &twodaBonusFeats =
		        TwoDAReg.get2DA(twodaClass.getRow(_classId).getString("BonusFeatsTable"));

		bonusFeats = twodaBonusFeats.getRow(_creature->getHitDice()).getInt("Bonus");
	} else {
		// The number of bonus feats is hardcoded before HotU.
		// TODO: Hardcoded bonus feats.
	}

	// Build list from all possible feats.
	const Aurora::TwoDAFile &twodaFeats = TwoDAReg.get2DA("feat");

	feats.clear();
	for (size_t it = 0; it < twodaFeats.getRowCount(); ++it) {
		if (!hasPrereqFeat(it, false))
			continue;

		const Aurora::TwoDARow &featRow = twodaFeats.getRow(it);

		FeatItem feat;
		feat.featId = it;
		feat.name = TalkMan.getString(featRow.getInt("FEAT"));
		feat.icon = featRow.getString("ICON");
		feat.description = TalkMan.getString(featRow.getInt("DESCRIPTION"));
		feat.masterFeat = 0xFFFFFFFF;
		feat.isMasterFeat = false;

		// Check is the feat belongs to a masterfeat.
		if (!featRow.empty("MASTERFEAT"))
			feat.masterFeat = featRow.getInt("MASTERFEAT");

		feats.push_back(feat);
	}

	// Add class feats.
	const Aurora::TwoDAFile &twodaClsFeat = TwoDAReg.get2DA(twodaClass.getRow(_classId).getString("FeatsTable"));
	for (size_t it = 0; it < twodaClsFeat.getRowCount(); ++it) {
		const Aurora::TwoDARow &clsFeatRow = twodaClsFeat.getRow(it);

		int32_t list = clsFeatRow.getInt("List");
		// Check if it is automatically granted.
		if (list == 3)
			continue;

		if (!hasPrereqFeat(clsFeatRow.getInt("FeatIndex"), true))
			continue;

		// When list=1, the feat can be both in the general and bonus feat list.
		uint32_t id = clsFeatRow.getInt("FeatIndex");
		if (list == 1) {
			for (std::list<FeatItem>::iterator f = feats.begin(); f != feats.end(); ++f) {
				if (id != (*f).featId)
					continue;

				(*f).list = 1;
				break;
			}
			continue;
		}

		const Aurora::TwoDARow &featRow = twodaFeats.getRow(id);
		FeatItem feat;
		feat.featId = id;
		feat.name = TalkMan.getString(featRow.getInt("FEAT"));
		feat.icon = featRow.getString("ICON");
		feat.description = TalkMan.getString(featRow.getInt("DESCRIPTION"));
		feat.list = list;
		feat.masterFeat = 0xFFFFFFFF;
		feat.isMasterFeat = false;

		// Check is the feat belongs to a masterfeat.
		if (!featRow.empty("MASTERFEAT"))
			feat.masterFeat = featRow.getInt("MASTERFEAT");

		feats.push_back(feat);
	}
}

} // End of namespace NWN

} // End of namespace Engines
