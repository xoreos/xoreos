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

#include "src/common/types.h"

#include "src/engines/nwn/types.h"

namespace Common {
class UString;
}

namespace Engines {

namespace NWN {

class Creature;

class CharGenChoices {
public:
	CharGenChoices();
	~CharGenChoices();

	void reset();
	const Creature &getCharacter();
	void setCharGender(Gender gender);
	void setCharRace(uint32 race);
	void setCharPortrait(const Common::UString &portrait);
	void setCharClass(uint32 classId);
	void setCharAlign(uint32 goodness, uint32 loyalty);
	void setCharAbilities(std::vector<uint32> abilities, std::vector<uint32> racialAbilities);
	void setCharPackage(uint32 package);
	void setCharSkill(size_t skillIndex, uint8 rank);
	void setCharNotUsedSkills(uint8 notUsedSkills);
	void setCharFeat(uint32 feat);
	void setSpellSchool(uint32 spellSchool);

	bool hasFeat(uint32 featId) const;
	bool hasPrereqFeat(uint32 featId, bool isClassFeat);
	uint32 getClass() const;
	uint32 getRace() const;
	bool getAlign(uint32 &goodness, uint32 &loyalty) const;
	uint32 getAbility(uint32 ability) const;
	std::vector<uint32> getFeats();
	uint32 getPackage() const;
	uint32 getSpellSchool() const;

private:
	Creature *_creature;
	std::vector<uint32> _normalFeats;
	std::vector<uint32> _racialFeats;
	std::vector<uint32> _classFeats;
	std::vector<uint32> _abilities;
	std::vector<uint32> _racialAbilities;
	std::vector<uint8> _skills;

	uint32 _classId;
	uint32 _goodness;
	uint32 _loyalty;
	uint32 _package;
	uint8 _notUsedSkills;
	uint32 _spellSchool;

};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_CHARGEN_CHARGENCHOICES_H
