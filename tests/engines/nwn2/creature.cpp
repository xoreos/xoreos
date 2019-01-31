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
 *  Unit tests for NWN2 creature objects.
 */

#include "gtest/gtest.h"

#include "tests/engines/nwn2/creature.h"
#include "tests/engines/nwn2/creature2.h"

#include "src/common/memreadstream.h"
#include "src/common/scopedptr.h"
#include "src/common/error.h"

#include "src/aurora/gff3file.h"

#include "src/engines/nwn2/creature.h"
#include "src/engines/nwn2/types.h"

static const float kEpsilon = 1e-5f;

GTEST_TEST(NWN2Creature, creature1) {
	Common::ScopedPtr<Common::MemoryReadStream> stream(new Common::MemoryReadStream(kDataCreature1));
	if (!stream)
		throw Common::Exception("No test data available");
	Common::ScopedPtr<Aurora::GFF3File> gff(new Aurora::GFF3File(stream.release(), MKTAG('G', 'I', 'T', ' ')));

	// Get the creature list
	const Aurora::GFF3Struct &top = gff->getTopLevel();
	const Aurora::GFF3List &crlist = top.getList("Creature List");
	ASSERT_EQ(crlist.size(), 1);

	// Get the creature data
	Aurora::GFF3List::const_iterator it = crlist.begin();
	Common::ScopedPtr<Engines::NWN2::Creature> cr(new Engines::NWN2::Creature(**it));

	EXPECT_STREQ(cr->getTag().c_str(), "c_halforc");
	EXPECT_STREQ(cr->getFirstName().c_str(), "[???]"); // Should be "Orcus"
	EXPECT_STREQ(cr->getLastName().c_str(), "[???]");  // Should be "Bloodbath"
	EXPECT_EQ(cr->getGender(), 0);
	EXPECT_FALSE(cr->isFemale());
	EXPECT_EQ(cr->getRace(), 5);
	EXPECT_EQ(cr->getSubRace(), 16);
	EXPECT_EQ(cr->getHitDice(), 1);
	EXPECT_EQ(cr->getGoodEvil(), 100);
	EXPECT_EQ(cr->getLawChaos(), 100);
	EXPECT_EQ(cr->getMaxHP(), 5);

	// Abilities
	EXPECT_EQ(cr->getAbility(Engines::NWN2::kAbilityStrength), 13);
	EXPECT_EQ(cr->getAbility(Engines::NWN2::kAbilityDexterity), 11);
	EXPECT_EQ(cr->getAbility(Engines::NWN2::kAbilityConstitution), 10);
	EXPECT_EQ(cr->getAbility(Engines::NWN2::kAbilityIntelligence), 10);
	EXPECT_EQ(cr->getAbility(Engines::NWN2::kAbilityWisdom), 9);
	EXPECT_EQ(cr->getAbility(Engines::NWN2::kAbilityCharisma), 8);

	// Classes
	uint32 classID;
	uint16 level;
	cr->getClass(0, classID, level);
	EXPECT_EQ(classID, Engines::NWN2::kCClassFighter);
	EXPECT_EQ(level, 1);

	// Skills and feats
	EXPECT_EQ(cr->getSkillRank(Engines::NWN2::kSkillParry, true), 4);
	EXPECT_FALSE(cr->hasFeat(0))   << "Feat: Alertness";
	EXPECT_TRUE(cr->hasFeat(2))    << "Feat: Armor Proficiency (Heavy)";
	EXPECT_TRUE(cr->hasFeat(4))    << "Feat: Armor Proficiency (Medium)";
	EXPECT_TRUE(cr->hasFeat(3))    << "Feat: Armor Proficiency (Light)";
	EXPECT_TRUE(cr->hasFeat(228))  << "Feat: Darkvision";
	EXPECT_TRUE(cr->hasFeat(28))   << "Feat: Power Attack";
	EXPECT_TRUE(cr->hasFeat(32))   << "Shield Proficiency";
	EXPECT_TRUE(cr->hasFeat(1115)) << "Tower Shield Proficiency";
	EXPECT_TRUE(cr->hasFeat(106))  << "Feat: Weapon Focus (longsword)";
	EXPECT_TRUE(cr->hasFeat(45))   << "Feat: Weapon Proficiency (Martial)";
	EXPECT_TRUE(cr->hasFeat(46))   << "Feat: Weapon Proficiency (Simple)";

	float x, y, z, angle;
	cr->getPosition(x, y, z);
	EXPECT_TRUE(abs(x - 5.17448) < kEpsilon);
	EXPECT_TRUE(abs(y - 7.05498) < kEpsilon);
	EXPECT_TRUE(abs(z) < kEpsilon);

	cr->getOrientation(x, y, z, angle);
	EXPECT_TRUE(abs(angle) < kEpsilon);
}

GTEST_TEST(NWN2Creature, creature2) {
	Common::ScopedPtr<Common::MemoryReadStream> stream(new Common::MemoryReadStream(kDataCreature2));
	if (!stream)
		throw Common::Exception("No test data available");
	Common::ScopedPtr<Aurora::GFF3File> gff(new Aurora::GFF3File(stream.release(), MKTAG('B', 'I', 'C', ' ')));

	// Load the PC
	const Aurora::GFF3Struct &top = gff->getTopLevel();
	Common::ScopedPtr<Engines::NWN2::Creature> cr(new Engines::NWN2::Creature(top));

	EXPECT_EQ(cr->getRace(), 21);
	EXPECT_EQ(cr->getSubRace(), 20);
	EXPECT_EQ(cr->getGender(), 1);
}
