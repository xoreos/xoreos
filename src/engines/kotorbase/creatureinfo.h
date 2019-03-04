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

#include <vector>

#include "src/common/types.h"

#include "src/engines/kotorbase/types.h"

namespace Aurora {
	class GFF3Struct;
}

namespace Engines {

namespace KotORBase {

class CharacterGenerationInfo;

class CreatureInfo {
public:
	CreatureInfo();
	CreatureInfo(const Aurora::GFF3Struct &gff);
	CreatureInfo(const CharacterGenerationInfo &info);

	CreatureInfo &operator=(const CreatureInfo &other);

	// Class levels

	struct ClassLevel {
		Class characterClass;
		int level;

		ClassLevel();
		ClassLevel(const ClassLevel &other);

		ClassLevel &operator=(const ClassLevel &other);
	};

	int getClassLevel(Class charClass) const;
	Class getClassByPosition(int position) const;
	int getLevelByPosition(int position) const;

	// Abilities

	struct Abilities {
		uint32 strength { 0 };
		uint32 dexterity { 0 };
		uint32 constitution { 0 };
		uint32 intelligence { 0 };
		uint32 wisdom { 0 };
		uint32 charisma { 0 };

		Abilities &operator=(const Abilities &other);
	};

	int getAbilityScore(Ability ability) const;

	void setAbilityScore(Ability ability, uint32 score);

	// Skills

	struct Skills {
		uint32 computerUse { 0 };
		uint32 demolitions { 0 };
		uint32 stealth { 0 };
		uint32 awareness { 0 };
		uint32 persuade { 0 };
		uint32 repair { 0 };
		uint32 security { 0 };
		uint32 treatInjury { 0 };

		Skills &operator=(const Skills &other);
	};

	int getSkillRank(Skill skill) const;

	void setSkillRank(Skill skill, uint32 rank);

private:
	std::vector<ClassLevel> _levels;
	Abilities _abilities;
	Skills _skills;

	void loadClassLevels(const Aurora::GFF3Struct &gff);
	void loadSkills(const Aurora::GFF3Struct &gff);
	void loadAbilities(const Aurora::GFF3Struct &gff);
};

} // End of namespace KotORBase

} // End of namespace Engines
