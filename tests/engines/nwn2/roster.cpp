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
 *  Unit tests for NWN2 roster objects.
 */

#include <memory>

#include "gtest/gtest.h"

#include "tests/engines/nwn2/roster.h"

#include "src/common/memreadstream.h"
#include "src/common/error.h"

#include "src/aurora/gff3file.h"

#include "src/engines/nwn2/roster.h"

// Utility class for testing Engines::NWN2::Roster
class TestRoster : public Engines::NWN2::Roster {
public:
	TestRoster() {
		Common::ScopedPtr<Common::MemoryReadStream> stream(new Common::MemoryReadStream(kDataRoster));
		if (!stream)
			throw Common::Exception("No test data available");
		Common::ScopedPtr<Aurora::GFF3File> gff(new Aurora::GFF3File(stream.release(), MKTAG('R', 'S', 'T', ' ')));
		const Aurora::GFF3Struct &top = gff->getTopLevel();

		// Insert the roster members
		const Aurora::GFF3List &rList = top.getList("RosMembers");
		for (Aurora::GFF3List::const_iterator it = rList.begin(); it != rList.end(); ++it) 
			loadMember(**it);
	}
	~TestRoster() {}
};

/**
 * Test the roster names in the ROSTER.rst file data.
 */

GTEST_TEST(NWN2Roster, rosterNames) {
	std::unique_ptr<TestRoster> roster(new TestRoster);

	EXPECT_STREQ(roster->getFirstRosterMember().c_str(), "ammon_jerro");
	EXPECT_STREQ(roster->getNextRosterMember().c_str(),  "bishop");
	EXPECT_STREQ(roster->getNextRosterMember().c_str(),  "casavir");
	EXPECT_STREQ(roster->getNextRosterMember().c_str(),  "construct");
	EXPECT_STREQ(roster->getNextRosterMember().c_str(),  "elanee");
	EXPECT_STREQ(roster->getNextRosterMember().c_str(),  "grobnar");
	EXPECT_STREQ(roster->getNextRosterMember().c_str(),  "khelgar");
	EXPECT_STREQ(roster->getNextRosterMember().c_str(),  "neeshka");
	EXPECT_STREQ(roster->getNextRosterMember().c_str(),  "qara");
	EXPECT_STREQ(roster->getNextRosterMember().c_str(),  "sand");
	EXPECT_STREQ(roster->getNextRosterMember().c_str(),  "shandra");
	EXPECT_STREQ(roster->getNextRosterMember().c_str(),  "zhjaeve");
	EXPECT_STREQ(roster->getNextRosterMember().c_str(),  "npc_bevil");
	EXPECT_STREQ(roster->getNextRosterMember().c_str(),  "");
	EXPECT_STREQ(roster->getFirstRosterMember().c_str(), "ammon_jerro");
	EXPECT_STREQ(roster->getNextRosterMember().c_str(),  "bishop");
}

/**
 * Test the roster member settings in the ROSTER.rst file data.
 */
GTEST_TEST(NWN2Roster, rosterSettings) {
	std::unique_ptr<TestRoster> roster(new TestRoster);

	EXPECT_TRUE(roster->getIsRosterMemberAvailable("ammon_jerro"));
	EXPECT_FALSE(roster->getIsRosterMemberAvailable("khelgar"));
	EXPECT_TRUE(roster->getIsRosterMemberCampaignNPC("qara"));
	EXPECT_FALSE(roster->getIsRosterMemberCampaignNPC("neeshka"));
	EXPECT_TRUE(roster->getIsRosterMemberSelectable("shandra"));
	EXPECT_FALSE(roster->getIsRosterMemberSelectable("npc_bevil"));
}

/**
 * Test the Roster class 'set' calls.
 */
GTEST_TEST(NWN2Roster, rosterSetCalls) {
	std::unique_ptr<Engines::NWN2::Roster> roster(new Engines::NWN2::Roster);

	// Bypass the ResMan resource check by inserting "[GTEST]" at the template start
	EXPECT_TRUE(roster->addRosterMemberByTemplate("adam_ant", "[GTEST]n_aldanon"));
	EXPECT_TRUE(roster->addRosterMemberByTemplate("bettie_boop", "[GTEST]n_brelaina"));
	EXPECT_TRUE(roster->addRosterMemberByTemplate("cookie_cutter", "[GTEST]n_calindra"));

	EXPECT_STREQ(roster->getFirstRosterMember().c_str(), "adam_ant");
	EXPECT_STREQ(roster->getNextRosterMember().c_str(),  "bettie_boop");
	EXPECT_STREQ(roster->getNextRosterMember().c_str(),  "cookie_cutter");

	EXPECT_TRUE(roster->setIsRosterMemberCampaignNPC("bettie_boop", true));
	EXPECT_FALSE(roster->getIsRosterMemberCampaignNPC("adam_ant"));
	EXPECT_TRUE(roster->getIsRosterMemberCampaignNPC("bettie_boop"));
	EXPECT_FALSE(roster->getIsRosterMemberCampaignNPC("cookie_cutter"));
}
