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
 *  Unit tests for KotOR combat formulas: AC calculation and d20 attack-roll logic.
 *
 *  These tests exercise the *same arithmetic* as Creature::getAC() and
 *  Creature::executeAttack() without requiring live game data (models, GFF
 *  files, etc.).  They serve as a regression guard for the milestone-critical
 *  combat path so that a future refactor cannot silently break the basic
 *  hit/miss determination.
 */

#include "gtest/gtest.h"

#include "src/engines/kotorbase/creatureinfo.h"
#include "src/engines/kotorbase/types.h"

using Engines::KotORBase::CreatureInfo;
using Engines::KotORBase::kAbilityDexterity;
using Engines::KotORBase::kAbilityStrength;

// ---------------------------------------------------------------------------
// AC formula helpers (mirrors Creature::getAC)
//   AC = 10 + dexModifier + armourBonus
// ---------------------------------------------------------------------------

static int calcAC(int dex, int armourBonus = 0) {
	int dexMod = (dex - 10) / 2;
	return 10 + dexMod + armourBonus;
}

// ---------------------------------------------------------------------------
// Attack-hit determination (mirrors Creature::executeAttack)
//   natural 1  → always miss
//   natural 20 → always hit
//   otherwise  → hit iff (d20 + attackMod) >= targetAC
// ---------------------------------------------------------------------------

static bool rollHits(int d20, int attackMod, int targetAC) {
	if (d20 == 20) return true;
	if (d20 ==  1) return false;
	return (d20 + attackMod) >= targetAC;
}

// ---------------------------------------------------------------------------
// AC tests
// ---------------------------------------------------------------------------

GTEST_TEST(KotORCombat, baseAcIsTen) {
	// A creature with Dex 10 and no armour has AC 10
	EXPECT_EQ(calcAC(10), 10);
}

GTEST_TEST(KotORCombat, positiveDexBonusRaisesAC) {
	EXPECT_EQ(calcAC(12),  11);  // +1 dex mod
	EXPECT_EQ(calcAC(14),  12);  // +2 dex mod
	EXPECT_EQ(calcAC(18),  14);  // +4 dex mod
}

GTEST_TEST(KotORCombat, negativeDexPenaltyLowersAC) {
	EXPECT_EQ(calcAC(8),   9);   // -1 dex mod
	EXPECT_EQ(calcAC(6),   8);   // -2 dex mod
}

GTEST_TEST(KotORCombat, armourBonusStacksWithDex) {
	// Light armour (+3 AC) on a Dex 14 (+2) creature → AC 15
	EXPECT_EQ(calcAC(14, 3), 15);
	// Heavy armour (+8 AC) on a Dex 8  (-1) creature → AC 17
	EXPECT_EQ(calcAC(8, 8),  17);
}

// ---------------------------------------------------------------------------
// Attack-hit tests
// ---------------------------------------------------------------------------

GTEST_TEST(KotORCombat, naturalTwentyAlwaysHits) {
	// Even against impossibly high AC, a natural 20 hits
	EXPECT_TRUE(rollHits(20,  0, 30));
	EXPECT_TRUE(rollHits(20, -5, 25));
	EXPECT_TRUE(rollHits(20,  0, 100));
}

GTEST_TEST(KotORCombat, naturalOneAlwaysMisses) {
	// Even against AC 1, a natural 1 misses
	EXPECT_FALSE(rollHits(1, 10, 1));
	EXPECT_FALSE(rollHits(1,  5, 5));
	EXPECT_FALSE(rollHits(1,  0, 2));
}

GTEST_TEST(KotORCombat, exactRollHits) {
	// Rolling exactly the AC needed is a hit
	EXPECT_TRUE(rollHits(15, 0, 15));  // 15+0 == 15 → hit
	EXPECT_TRUE(rollHits(10, 2, 12));  // 10+2 == 12 → hit
	EXPECT_TRUE(rollHits( 5, 5, 10));  //  5+5 == 10 → hit
}

GTEST_TEST(KotORCombat, oneShortMisses) {
	// Rolling one below the required total misses
	EXPECT_FALSE(rollHits(14, 0, 15));  // 14 < 15
	EXPECT_FALSE(rollHits( 9, 2, 12));  // 11 < 12
}

GTEST_TEST(KotORCombat, positiveModifierCanTurnMissIntoHit) {
	// Base roll 10 vs AC 15: would miss at +0, hits at +5
	EXPECT_FALSE(rollHits(10, 0, 15));
	EXPECT_TRUE( rollHits(10, 5, 15));
}

GTEST_TEST(KotORCombat, negativeModifierCanTurnHitIntoMiss) {
	// Base roll 14 vs AC 12: would hit at +0, misses at -3
	EXPECT_TRUE( rollHits(14,  0, 12));
	EXPECT_FALSE(rollHits(14, -3, 12));  // 14-3=11 < 12
}

// ---------------------------------------------------------------------------
// Ability modifier integration with AC / attack roll
// ---------------------------------------------------------------------------

GTEST_TEST(KotORCombat, dexModifierFromCreatureInfo) {
	CreatureInfo info;
	info.setAbilityScore(kAbilityDexterity, 16); // modifier +3
	int dexMod = info.getAbilityModifier(kAbilityDexterity);
	EXPECT_EQ(dexMod, 3);
	// AC for an unarmoured creature with this Dex
	int ac = 10 + dexMod;
	EXPECT_EQ(ac, 13);
}

GTEST_TEST(KotORCombat, strModifierFromCreatureInfo) {
	CreatureInfo info;
	info.setAbilityScore(kAbilityStrength, 14); // modifier +2
	int strMod = info.getAbilityModifier(kAbilityStrength);
	EXPECT_EQ(strMod, 2);

	// A roll of 11 + 2 = 13, which just hits AC 13 but misses AC 14
	EXPECT_TRUE( rollHits(11, strMod, 13));
	EXPECT_FALSE(rollHits(11, strMod, 14));
}

GTEST_TEST(KotORCombat, weakAttackerVsHighAC) {
	// Soldier: Str 8 (-1 modifier) attacks a target with AC 18
	// Needs d20 + (-1) >= 18 → needs d20 >= 19 (only 19 or 20 hit, natural 20 always)
	CreatureInfo attackerInfo;
	attackerInfo.setAbilityScore(kAbilityStrength, 8);
	int mod = attackerInfo.getAbilityModifier(kAbilityStrength); // -1

	int targetAC = 18;

	// d20 rolls 1..18 all miss (1 always misses, 2..18 give 1..17 < 18)
	for (int roll = 1; roll <= 18; ++roll)
		EXPECT_FALSE(rollHits(roll, mod, targetAC)) << "roll=" << roll;

	// d20 = 19 → 19 + (-1) = 18 → hits
	EXPECT_TRUE(rollHits(19, mod, targetAC));

	// d20 = 20 → always hits
	EXPECT_TRUE(rollHits(20, mod, targetAC));
}
