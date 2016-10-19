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
	uint8 rank;
	uint8 maxRank;
	uint8 minRank;
	size_t skillID;
	bool isClassSkill;
	Common::UString name;
	Common::UString icon;
	Common::UString help;
};

struct FeatItem {
	uint32 featId;
	Common::UString name;
	Common::UString description;
	Common::UString icon;
	uint8 list; ///< The kind of list the feat belongs to: 0 (general feat), 2 (bonus feat), 1 (both)
	bool isMasterFeat;
	uint32 masterFeat;

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
	void setRace(uint32 race);
	void setPortrait(const Common::UString &portrait);
	void setClass(uint32 classId);
	void setAlign(uint8 goodness, uint8 lawfulness);
	void setAbilities(std::vector<uint8> abilities, std::vector<uint8> racialAbilities);
	void setPackage(uint8 package);
	void setSkill(size_t skillIndex, uint8 rank);
	void setNotUsedSkills(uint8 notUsedSkills);
	void setFeat(uint32 feat);
	void setSpellSchool(uint8 spellSchool);
	void setDomains(uint8 domain1, uint8 domain2);
	void setSpell(size_t spellLevel, uint16 spell);
	void setSoundSet(uint32 soundSetID);
	void useCharacter(Module *module);

	bool hasFeat(uint32 featId) const;
	bool hasPrereqFeat(uint32 featId, bool isClassFeat);
	uint32 getClass() const;
	uint32 getRace() const;
	bool getAlign(uint8 &goodness, uint8 &lawfulness) const;
	uint8 getAbility(Ability ability) const;
	uint8 getTotalAbility(Ability ability) const;
	int8 getAbilityModifier(Ability ability);
	void getFeats(std::vector<uint32> &feats);
	uint8 getPackage() const;
	uint8 getSpellSchool() const;

	void getPrefFeats(std::vector<uint32> &feats);
	uint8 getPrefSpellSchool();
	void getPrefSkills(std::vector<uint8> &skills);
	void getPrefDomains(uint8 &domain1, uint8 &domain2);
	void getPrefSpells(std::vector<std::vector<uint16> > &spells);

	uint8 computeAvailSkillRank();
	void getSkillItems(std::vector<SkillItem> &skills);
	void getFeatItems(std::list<FeatItem> &feats, uint8 &normalFeats, uint8 &bonusFeats);

private:
	Creature *_creature;
	std::vector<std::vector<uint16> > _spells;
	std::vector<uint32> _normalFeats;
	std::vector<uint32> _racialFeats;
	std::vector<uint32> _classFeats;
	std::vector<uint8> _abilities;
	std::vector<uint8> _racialAbilities;
	std::vector<uint8> _skills;

	uint32 _classId;
	uint32 _soundSet;
	uint8 _goodness;
	uint8 _lawfulness;
	uint8 _package;
	uint8 _notUsedSkills;
	uint8 _spellSchool;
	uint8 _domain1;
	uint8 _domain2;

	void init();
	bool _characterUsed;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_CHARGEN_CHARGENCHOICES_H
