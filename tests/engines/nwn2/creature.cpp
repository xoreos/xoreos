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

#include <memory>

#include "gtest/gtest.h"

#include "tests/engines/nwn2/creature.h"
#include "tests/engines/nwn2/creature2.h"
#include "tests/engines/nwn2/creature3.h"

#include "src/common/memreadstream.h"
#include "src/common/error.h"

#include "src/aurora/gff3file.h"

#include "src/engines/nwn2/creature.h"
#include "src/engines/nwn2/types.h"
#include "src/engines/nwn2/item.h"
#include "src/engines/nwn2/itemproperty.h"

static const float kEpsilon = 1e-5f;

GTEST_TEST(NWN2Creature, creature1) {
	std::unique_ptr<Common::MemoryReadStream> stream = std::make_unique<Common::MemoryReadStream>(kDataCreature1);
	if (!stream)
		throw Common::Exception("No test data available");
	std::unique_ptr<Aurora::GFF3File> gff = std::make_unique<Aurora::GFF3File>(stream.release(), MKTAG('G', 'I', 'T', ' '));

	// Get the creature list
	const Aurora::GFF3Struct &top = gff->getTopLevel();
	const Aurora::GFF3List &crlist = top.getList("Creature List");
	ASSERT_EQ(crlist.size(), 1);

	// Get the creature data
	Aurora::GFF3List::const_iterator it = crlist.begin();
	std::unique_ptr<Engines::NWN2::Creature> cr = std::make_unique<Engines::NWN2::Creature>(**it);

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
	uint32_t classID;
	uint16_t level;
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
	std::unique_ptr<Common::MemoryReadStream> stream = std::make_unique<Common::MemoryReadStream>(kDataCreature2);
	if (!stream)
		throw Common::Exception("No test data available");
	std::unique_ptr<Aurora::GFF3File> gff = std::make_unique<Aurora::GFF3File>(stream.release(), MKTAG('B', 'I', 'C', ' '));

	// Load the PC
	const Aurora::GFF3Struct &top = gff->getTopLevel();
	std::unique_ptr<Engines::NWN2::Creature> cr = std::make_unique<Engines::NWN2::Creature>(top);

	EXPECT_EQ(cr->getRace(), 21);
	EXPECT_EQ(cr->getSubRace(), 20);
	EXPECT_EQ(cr->getGender(), 1);
	EXPECT_EQ(cr->getClassLevel(8), 4);
	EXPECT_EQ(cr->getHitDice(), 4);
	EXPECT_EQ(cr->getAbility(Engines::NWN2::kAbilityStrength), 13);
	EXPECT_EQ(cr->getAbility(Engines::NWN2::kAbilityDexterity), 17);
	EXPECT_EQ(cr->getAbility(Engines::NWN2::kAbilityConstitution), 10);
	EXPECT_EQ(cr->getAbility(Engines::NWN2::kAbilityIntelligence), 10);
	EXPECT_EQ(cr->getAbility(Engines::NWN2::kAbilityWisdom), 11);
	EXPECT_EQ(cr->getAbility(Engines::NWN2::kAbilityCharisma), 16);
	EXPECT_EQ(cr->getGoodEvil(), 85);
	EXPECT_EQ(cr->getLawChaos(), 15);
	EXPECT_EQ(cr->getAge(), 20);

	// Skills and feats
	EXPECT_EQ(cr->getSkillRank(Engines::NWN2::kSkillDisableDevice, true), 4);
	EXPECT_EQ(cr->getSkillRank(Engines::NWN2::kSkillHide, true), 4);
	EXPECT_EQ(cr->getSkillRank(Engines::NWN2::kSkillMoveSilently, true), 4);
	EXPECT_EQ(cr->getSkillRank(Engines::NWN2::kSkillOpenLock, true), 4);
	EXPECT_EQ(cr->getSkillRank(Engines::NWN2::kSkillDiplomacy, true), 6);
	EXPECT_EQ(cr->getSkillRank(Engines::NWN2::kSkillSleightOfHand, true), 6);
	EXPECT_EQ(cr->getSkillRank(Engines::NWN2::kSkillSearch, true), 4);
	EXPECT_EQ(cr->getSkillRank(Engines::NWN2::kSkillSetTrap, true), 7);
	EXPECT_EQ(cr->getSkillRank(Engines::NWN2::kSkillSpot, true), 4);
	EXPECT_EQ(cr->getSkillRank(Engines::NWN2::kSkillUseMagicDevice, true), 4);
	EXPECT_EQ(cr->getSkillRank(Engines::NWN2::kSkillAppraise, true), 5);
	EXPECT_EQ(cr->getSkillRank(Engines::NWN2::kSkillTumble, true), 4);
	EXPECT_EQ(cr->getSkillRank(Engines::NWN2::kSkillBluff, true), 4);
	EXPECT_EQ(cr->getSkillRank(Engines::NWN2::kSkillIntimidate, true), 3);
	EXPECT_TRUE(cr->hasFeat(3))    << "Feat: Armor Proficiency (Light)";
	EXPECT_TRUE(cr->hasFeat(10))   << "Feat: Dodge";
	EXPECT_TRUE(cr->hasFeat(46))   << "Feat: Weapon Proficiency (Simple)";
	EXPECT_TRUE(cr->hasFeat(50))   << "Feat: Weapon Proficiency (Rogue)";
	EXPECT_TRUE(cr->hasFeat(195))  << "Feat: Uncanny Dodge 1";
	EXPECT_TRUE(cr->hasFeat(206))  << "Feat: Evasion";
	EXPECT_TRUE(cr->hasFeat(221))  << "Feat: Sneak Attack 1";
	EXPECT_TRUE(cr->hasFeat(228))  << "Feat: Darkvision";
	EXPECT_TRUE(cr->hasFeat(345))  << "Feat: Seank Attack 2";
	EXPECT_TRUE(cr->hasFeat(408))  << "Feat: Blind Fight";
	EXPECT_TRUE(cr->hasFeat(1387)) << "Feat: Trap Sense 1";
	EXPECT_TRUE(cr->hasFeat(1857)) << "Feat: Trapfinding";
	EXPECT_TRUE(cr->hasFeat(1870)) << "Feat: Fire Genasi Resistance";
	EXPECT_TRUE(cr->hasFeat(1874)) << "Feat: Fire Genasi Reach to the Blaze";
}

GTEST_TEST(NWN2Creature, inventory) {
	std::unique_ptr<Common::MemoryReadStream> stream = std::make_unique<Common::MemoryReadStream>(kDataCreature3);
	if (!stream)
		throw Common::Exception("No test data available");
	std::unique_ptr<Aurora::GFF3File> gff = std::make_unique<Aurora::GFF3File>(stream.release(), MKTAG('R', 'O', 'S', ' '));

	// Load the PC
	const Aurora::GFF3Struct &top = gff->getTopLevel();
	std::unique_ptr<Engines::NWN2::Creature> cr = std::make_unique<Engines::NWN2::Creature>(top);

	EXPECT_STREQ(cr->getTag().c_str(), "elanee");

	Engines::NWN2::Item *item = cr->getFirstItemInInventory();
	EXPECT_TRUE(item != nullptr);
	if (item != nullptr) {
		EXPECT_STREQ(item->getTag().c_str(), "NW_WBLMCL002");

		// Get the first item property
		Engines::NWN2::ItemProperty *prop = item->getFirstItemProperty();
		EXPECT_TRUE(item->getItemHasItemProperty(Engines::NWN2::kItemPropertyEnhancementBonus));
		EXPECT_TRUE(prop != nullptr);
		if (prop) {
			EXPECT_EQ(prop->getItemPropertyType(), 6);
			EXPECT_EQ(prop->getItemPropertySubType(), 0);
			EXPECT_EQ(prop->getItemPropertyParam1(), 255);
			EXPECT_EQ(prop->getItemPropertyParam1Value(), 255);
			EXPECT_EQ(prop->getItemPropertyCostTable(), 2);
			EXPECT_EQ(prop->getItemPropertyCostTableValue(), 1);
		}
	}

	item = cr->getNextItemInInventory();
	EXPECT_TRUE(item != nullptr);
	if (item != nullptr) {
		EXPECT_STREQ(item->getTag().c_str(),  "NW_IT_SPARSCR204");

		// Check the item properties
		Engines::NWN2::ItemProperty *prop1 = item->getFirstItemProperty();
		EXPECT_TRUE(item->getItemHasItemProperty(Engines::NWN2::kItemPropertyCastSpell));
		EXPECT_TRUE(item->getItemHasItemProperty(Engines::NWN2::kItemPropertyUseLimitationClass));
		EXPECT_TRUE(prop1 != nullptr);
		if (prop1) {
			EXPECT_EQ(prop1->getItemPropertyType(), 15);
			EXPECT_EQ(prop1->getItemPropertySubType(), 281);
			EXPECT_EQ(prop1->getItemPropertyCostTable(), 3);
			EXPECT_EQ(prop1->getItemPropertyCostTableValue(), 1);
		}
		Engines::NWN2::ItemProperty *prop2 = item->getNextItemProperty();
		EXPECT_TRUE(prop2 != nullptr);
		if (prop2) {
			EXPECT_EQ(prop2->getItemPropertyType(), 63);
			EXPECT_EQ(prop2->getItemPropertySubType(), 1);
			EXPECT_EQ(prop2->getItemPropertyCostTable(), 0);
			EXPECT_EQ(prop2->getItemPropertyCostTableValue(), 0);
		}
		Engines::NWN2::ItemProperty *prop3 = item->getNextItemProperty();
		EXPECT_TRUE(prop3 != nullptr);
		if (prop3) {
			EXPECT_EQ(prop3->getItemPropertyType(), 63);
			EXPECT_EQ(prop3->getItemPropertySubType(), 9);
		}
		Engines::NWN2::ItemProperty *prop4 = item->getNextItemProperty();
		EXPECT_TRUE(prop4 != nullptr);
		if (prop4) {
			EXPECT_EQ(prop4->getItemPropertyType(), 63);
			EXPECT_EQ(prop4->getItemPropertySubType(), 10);
		}
	}

	item = cr->getNextItemInInventory();
	EXPECT_TRUE(item != nullptr);
	if (item != nullptr) {
		EXPECT_STREQ(item->getTag().c_str(),  "NW_WSPSC001");
	}

	item = cr->getNextItemInInventory();
	EXPECT_TRUE(item != nullptr);
	if (item != nullptr) {
		EXPECT_STREQ(item->getTag().c_str(),  "X2_IT_SPDVSCR001");
	}

	item = cr->getNextItemInInventory();
	EXPECT_TRUE(item != nullptr);
	if (item != nullptr) {
		EXPECT_STREQ(item->getTag().c_str(),  "NW_WMGWN005");
	}

	item = cr->getNextItemInInventory();
	EXPECT_TRUE(item != nullptr);
	if (item != nullptr) {
		EXPECT_STREQ(item->getTag().c_str(),  "NW_IT_SPARSCR202");
	}

	item = cr->getNextItemInInventory();
	EXPECT_TRUE(item != nullptr);
	if (item != nullptr) {
		EXPECT_STREQ(item->getTag().c_str(),  "NW_IT_SPDVSCR203");
	}

	item = cr->getNextItemInInventory();
	EXPECT_TRUE(item != nullptr);
	if (item != nullptr) {
		EXPECT_STREQ(item->getTag().c_str(),  "x2_nash_boot");
	}

	item = cr->getNextItemInInventory();
	EXPECT_TRUE(item != nullptr);
	if (item != nullptr) {
		EXPECT_STREQ(item->getTag().c_str(),  "NW_WSWSC001");
	}

	item = cr->getItemInSlot(Engines::NWN2::kInventorySlotRightHand);
	EXPECT_TRUE(item != nullptr);
	if (item != nullptr) {
		EXPECT_STREQ(item->getTag().c_str(),  "NW_WBLMCL002");
	}
}
