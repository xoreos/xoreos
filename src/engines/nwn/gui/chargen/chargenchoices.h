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

#ifndef ENGINES_NWN_GUI_CHARGEN_CHARGENCHOICES_H
#define ENGINES_NWN_GUI_CHARGEN_CHARGENCHOICES_H

#include <vector>
#include <list>

#include "src/common/types.h"
#include "src/common/ustring.h"

#include "src/engines/nwn/types.h"

namespace Engines {

namespace NWN {

class Creature;
class Module;

struct SkillItem {
	uint8_t rank;
	uint8_t maxRank;
	uint8_t minRank;
	size_t skillID;
	bool isClassSkill;
	Common::UString name;
	Common::UString icon;
	Common::UString help;
};

struct FeatItem {
	uint32_t featId;
	Common::UString name;
	Common::UString description;
	Common::UString icon;
	uint8_t list; ///< The kind of list the feat belongs to: 0 (general feat), 2 (bonus feat), 1 (both)
	bool isMasterFeat;
	uint32_t masterFeat;

	bool operator < (const FeatItem& f) const {
		return (name < f.name);
	}
	FeatItem();
};

class CharGenChoices {
public:
	CharGenChoices();
	~CharGenChoices();

	void applyChoices();

	void reset();
	void resetPackage();
	const Creature &getCharacter();
	void setGender(Gender gender);
	void setRace(uint32_t race);
	void setPortrait(const Common::UString &portrait);
	void setClass(uint32_t classId);
	void setAlign(uint8_t goodness, uint8_t lawfulness);
	void setAbilities(std::vector<uint8_t> abilities, std::vector<uint8_t> racialAbilities);
	void setPackage(uint8_t package);
	void setSkill(size_t skillIndex, uint8_t rank);
	void setNotUsedSkills(uint8_t notUsedSkills);
	void setFeat(uint32_t feat);
	void setSpellSchool(uint8_t spellSchool);
	void setDomains(uint8_t domain1, uint8_t domain2);
	void setSpell(size_t spellLevel, uint16_t spell);
	void setSoundSet(uint32_t soundSetID);
	void useCharacter(Module *module);

	bool hasFeat(uint32_t featId) const;
	bool hasPrereqFeat(uint32_t featId, bool isClassFeat);
	uint32_t getClass() const;
	uint32_t getRace() const;
	bool getAlign(uint8_t &goodness, uint8_t &lawfulness) const;
	uint8_t getAbility(Ability ability) const;
	uint8_t getTotalAbility(Ability ability) const;
	int8_t getAbilityModifier(Ability ability);
	void getFeats(std::vector<uint32_t> &feats);
	uint8_t getPackage() const;
	uint8_t getSpellSchool() const;

	void getPrefFeats(std::vector<uint32_t> &feats);
	uint8_t getPrefSpellSchool();
	void getPrefSkills(std::vector<uint8_t> &skills);
	void getPrefDomains(uint8_t &domain1, uint8_t &domain2);
	void getPrefSpells(std::vector<std::vector<uint16_t> > &spells);

	uint8_t computeAvailSkillRank();
	void getSkillItems(std::vector<SkillItem> &skills);
	void getFeatItems(std::list<FeatItem> &feats, uint8_t &normalFeats, uint8_t &bonusFeats);

private:
	Creature *_creature;
	std::vector<std::vector<uint16_t> > _spells;
	std::vector<uint32_t> _normalFeats;
	std::vector<uint32_t> _racialFeats;
	std::vector<uint32_t> _classFeats;
	std::vector<uint8_t> _abilities;
	std::vector<uint8_t> _racialAbilities;
	std::vector<uint8_t> _skills;

	uint32_t _classId;
	uint32_t _soundSet;
	uint8_t _goodness;
	uint8_t _lawfulness;
	uint8_t _package;
	uint8_t _notUsedSkills;
	uint8_t _spellSchool;
	uint8_t _domain1;
	uint8_t _domain2;

	void init();
	bool _characterUsed;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_CHARGEN_CHARGENCHOICES_H
