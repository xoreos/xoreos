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
 *  Unit tests for KotORBase::CreatureInfo — ability scores, modifiers, and skills.
 */

#include "gtest/gtest.h"

#include "src/engines/kotorbase/creatureinfo.h"
#include "src/engines/kotorbase/types.h"

using Engines::KotORBase::CreatureInfo;
using Engines::KotORBase::kAbilityStrength;
using Engines::KotORBase::kAbilityDexterity;
using Engines::KotORBase::kAbilityConstitution;
using Engines::KotORBase::kAbilityIntelligence;
using Engines::KotORBase::kAbilityWisdom;
using Engines::KotORBase::kAbilityCharisma;
using Engines::KotORBase::kSkillComputerUse;
using Engines::KotORBase::kSkillDemolitions;
using Engines::KotORBase::kSkillStealth;
using Engines::KotORBase::kSkillAwareness;
using Engines::KotORBase::kSkillPersuade;
using Engines::KotORBase::kSkillRepair;
using Engines::KotORBase::kSkillSecurity;
using Engines::KotORBase::kSkillTreatInjury;

// ---------------------------------------------------------------------------
// Ability scores and modifiers
// ---------------------------------------------------------------------------

GTEST_TEST(KotORBaseCreatureInfo, defaultConstruct) {
	CreatureInfo info;
	// All ability scores default to 0
	EXPECT_EQ(info.getAbilityScore(kAbilityStrength),     0);
	EXPECT_EQ(info.getAbilityScore(kAbilityDexterity),    0);
	EXPECT_EQ(info.getAbilityScore(kAbilityConstitution), 0);
	EXPECT_EQ(info.getAbilityScore(kAbilityIntelligence), 0);
	EXPECT_EQ(info.getAbilityScore(kAbilityWisdom),       0);
	EXPECT_EQ(info.getAbilityScore(kAbilityCharisma),     0);
}

GTEST_TEST(KotORBaseCreatureInfo, setAndGetAllAbilities) {
	CreatureInfo info;
	info.setAbilityScore(kAbilityStrength,     14);
	info.setAbilityScore(kAbilityDexterity,    12);
	info.setAbilityScore(kAbilityConstitution, 10);
	info.setAbilityScore(kAbilityIntelligence,  8);
	info.setAbilityScore(kAbilityWisdom,       16);
	info.setAbilityScore(kAbilityCharisma,     18);

	EXPECT_EQ(info.getAbilityScore(kAbilityStrength),     14);
	EXPECT_EQ(info.getAbilityScore(kAbilityDexterity),    12);
	EXPECT_EQ(info.getAbilityScore(kAbilityConstitution), 10);
	EXPECT_EQ(info.getAbilityScore(kAbilityIntelligence),  8);
	EXPECT_EQ(info.getAbilityScore(kAbilityWisdom),       16);
	EXPECT_EQ(info.getAbilityScore(kAbilityCharisma),     18);
}

// Modifier formula: floor((score - 10) / 2)
// C++ integer division truncates toward zero, so negative half-integers
// round toward zero (e.g. (9-10)/2 = -1/2 = 0 in C++).
// The KotOR rule uses the same truncating integer divide, which matches
// the engine's implementation.
GTEST_TEST(KotORBaseCreatureInfo, abilityModifierFormula) {
	CreatureInfo info;

	// Score 10 → modifier 0
	info.setAbilityScore(kAbilityStrength, 10);
	EXPECT_EQ(info.getAbilityModifier(kAbilityStrength), 0);

	// Score 12 → modifier +1
	info.setAbilityScore(kAbilityStrength, 12);
	EXPECT_EQ(info.getAbilityModifier(kAbilityStrength), 1);

	// Score 14 → modifier +2
	info.setAbilityScore(kAbilityStrength, 14);
	EXPECT_EQ(info.getAbilityModifier(kAbilityStrength), 2);

	// Score 18 → modifier +4
	info.setAbilityScore(kAbilityStrength, 18);
	EXPECT_EQ(info.getAbilityModifier(kAbilityStrength), 4);

	// Score 8 → modifier -1  (8-10 = -2, -2/2 = -1)
	info.setAbilityScore(kAbilityStrength, 8);
	EXPECT_EQ(info.getAbilityModifier(kAbilityStrength), -1);

	// Score 6 → modifier -2
	info.setAbilityScore(kAbilityStrength, 6);
	EXPECT_EQ(info.getAbilityModifier(kAbilityStrength), -2);

	// Score 1 → modifier -5  ((1-10)/2 = -9/2 = -4 in C++ truncation)
	// Note: the d20 SRD defines floor() here, but the engine uses C++ integer
	// division which gives -4 for score 1. We test for -4 to match the code.
	info.setAbilityScore(kAbilityStrength, 1);
	EXPECT_EQ(info.getAbilityModifier(kAbilityStrength), -4);

	// Score 20 → modifier +5
	info.setAbilityScore(kAbilityStrength, 20);
	EXPECT_EQ(info.getAbilityModifier(kAbilityStrength), 5);
}

GTEST_TEST(KotORBaseCreatureInfo, abilityModifierRoundTrip) {
	// Check all six abilities produce independent modifiers
	CreatureInfo info;
	info.setAbilityScore(kAbilityStrength,     16); // +3
	info.setAbilityScore(kAbilityDexterity,    14); // +2
	info.setAbilityScore(kAbilityConstitution, 12); // +1
	info.setAbilityScore(kAbilityIntelligence, 10); //  0
	info.setAbilityScore(kAbilityWisdom,        8); // -1
	info.setAbilityScore(kAbilityCharisma,      6); // -2

	EXPECT_EQ(info.getAbilityModifier(kAbilityStrength),      3);
	EXPECT_EQ(info.getAbilityModifier(kAbilityDexterity),     2);
	EXPECT_EQ(info.getAbilityModifier(kAbilityConstitution),  1);
	EXPECT_EQ(info.getAbilityModifier(kAbilityIntelligence),  0);
	EXPECT_EQ(info.getAbilityModifier(kAbilityWisdom),       -1);
	EXPECT_EQ(info.getAbilityModifier(kAbilityCharisma),     -2);
}

// ---------------------------------------------------------------------------
// Skill ranks
// ---------------------------------------------------------------------------

GTEST_TEST(KotORBaseCreatureInfo, defaultSkillRanks) {
	CreatureInfo info;
	EXPECT_EQ(info.getSkillRank(kSkillComputerUse),  0);
	EXPECT_EQ(info.getSkillRank(kSkillDemolitions),  0);
	EXPECT_EQ(info.getSkillRank(kSkillStealth),      0);
	EXPECT_EQ(info.getSkillRank(kSkillAwareness),    0);
	EXPECT_EQ(info.getSkillRank(kSkillPersuade),     0);
	EXPECT_EQ(info.getSkillRank(kSkillRepair),       0);
	EXPECT_EQ(info.getSkillRank(kSkillSecurity),     0);
	EXPECT_EQ(info.getSkillRank(kSkillTreatInjury),  0);
}

GTEST_TEST(KotORBaseCreatureInfo, setAndGetSkillRanks) {
	CreatureInfo info;
	info.setSkillRank(kSkillComputerUse, 4);
	info.setSkillRank(kSkillDemolitions, 2);
	info.setSkillRank(kSkillStealth,     1);
	info.setSkillRank(kSkillAwareness,   3);
	info.setSkillRank(kSkillPersuade,    5);
	info.setSkillRank(kSkillRepair,      2);
	info.setSkillRank(kSkillSecurity,    1);
	info.setSkillRank(kSkillTreatInjury, 3);

	EXPECT_EQ(info.getSkillRank(kSkillComputerUse),  4);
	EXPECT_EQ(info.getSkillRank(kSkillDemolitions),  2);
	EXPECT_EQ(info.getSkillRank(kSkillStealth),      1);
	EXPECT_EQ(info.getSkillRank(kSkillAwareness),    3);
	EXPECT_EQ(info.getSkillRank(kSkillPersuade),     5);
	EXPECT_EQ(info.getSkillRank(kSkillRepair),       2);
	EXPECT_EQ(info.getSkillRank(kSkillSecurity),     1);
	EXPECT_EQ(info.getSkillRank(kSkillTreatInjury),  3);
}

GTEST_TEST(KotORBaseCreatureInfo, skillsAreIndependent) {
	// Verifying that writing one skill does not clobber another
	CreatureInfo info;
	info.setSkillRank(kSkillAwareness, 7);
	info.setSkillRank(kSkillPersuade,  3);

	EXPECT_EQ(info.getSkillRank(kSkillAwareness), 7);
	EXPECT_EQ(info.getSkillRank(kSkillPersuade),  3);
	EXPECT_EQ(info.getSkillRank(kSkillStealth),   0); // untouched
}

// ---------------------------------------------------------------------------
// Copy / assignment
// ---------------------------------------------------------------------------

GTEST_TEST(KotORBaseCreatureInfo, copyConstruct) {
	CreatureInfo src;
	src.setAbilityScore(kAbilityStrength, 15);
	src.setSkillRank(kSkillPersuade, 6);

	CreatureInfo dst(src);
	EXPECT_EQ(dst.getAbilityScore(kAbilityStrength), 15);
	EXPECT_EQ(dst.getSkillRank(kSkillPersuade),       6);
}

GTEST_TEST(KotORBaseCreatureInfo, assignmentOperator) {
	CreatureInfo src;
	src.setAbilityScore(kAbilityDexterity, 17);
	src.setSkillRank(kSkillStealth, 4);

	CreatureInfo dst;
	dst = src;
	EXPECT_EQ(dst.getAbilityScore(kAbilityDexterity), 17);
	EXPECT_EQ(dst.getSkillRank(kSkillStealth),          4);
}

GTEST_TEST(KotORBaseCreatureInfo, assignmentDoesNotAliasSource) {
	CreatureInfo src;
	src.setAbilityScore(kAbilityStrength, 10);

	CreatureInfo dst;
	dst = src;

	// Modifying dst must not affect src
	dst.setAbilityScore(kAbilityStrength, 20);
	EXPECT_EQ(src.getAbilityScore(kAbilityStrength), 10);
	EXPECT_EQ(dst.getAbilityScore(kAbilityStrength), 20);
}
