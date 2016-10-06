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
 *  Unit tests for our VISFile class.
 */

#include "gtest/gtest.h"

#include "src/common/error.h"
#include "src/common/memreadstream.h"

#include "src/aurora/visfile.h"

static const char *kVISFile =
  "Room01 3\n"
  "  Room02\n"
  "  Room03  \n"
  "  Room04\n"
  "Room09 0\n"
  "Room02 4\n"
  "  Room01 \n"
  "  Room03 \n"
  "  Room04 \n"
  "  Room05 \n"
  "[Adjacent]\n"
  "Room01 2\n"
  "  Room02\n"
  "  Room03\n"
  "Room02 2\n"
  "  Room01\n"
  "  Room03\n";

GTEST_TEST(VISFile, getVisibilityArray) {
	Common::MemoryReadStream stream(kVISFile);
	Aurora::VISFile vis;
	vis.load(stream);

	const std::vector<Common::UString> &room1 = vis.getVisibilityArray("Room01");
	ASSERT_EQ(room1.size(), 3);

	EXPECT_STREQ(room1[0].c_str(), "Room02");
	EXPECT_STREQ(room1[1].c_str(), "Room03");
	EXPECT_STREQ(room1[2].c_str(), "Room04");

	const std::vector<Common::UString> &room2 = vis.getVisibilityArray("room02");
	ASSERT_EQ(room2.size(), 4);

	EXPECT_STREQ(room2[0].c_str(), "Room01");
	EXPECT_STREQ(room2[1].c_str(), "Room03");
	EXPECT_STREQ(room2[2].c_str(), "Room04");
	EXPECT_STREQ(room2[3].c_str(), "Room05");

	const std::vector<Common::UString> &room9 = vis.getVisibilityArray("ROOM09");
	ASSERT_EQ(room9.size(), 0);

	const std::vector<Common::UString> &roomNope = vis.getVisibilityArray("Nope");
	ASSERT_EQ(roomNope.size(), 0);
}

GTEST_TEST(VISFile, clear) {
	Common::MemoryReadStream stream(kVISFile);
	Aurora::VISFile vis;
	vis.load(stream);

	vis.clear();

	EXPECT_EQ(vis.getVisibilityArray("Room01").size(), 0);
	EXPECT_EQ(vis.getVisibilityArray("Room02").size(), 0);
	EXPECT_EQ(vis.getVisibilityArray("Room09").size(), 0);

	EXPECT_EQ(vis.getVisibilityArray("Nope").size(), 0);
}

GTEST_TEST(VISFile, brokenCounts) {
	static const char *kVISFileVariant =
		"Room01\n"
		"  Room02\n"
		"  Room03\n"
		"  Room04\n"
		"Room02 4\n"
		"  Room02\n"
		"  Room03\n"
		"  Room04\n"
		"Room03 2\n"
		"  Room02\n"
		"  Room03\n"
		"  Room04\n";

	Common::MemoryReadStream stream(kVISFileVariant);
	Aurora::VISFile vis;
	vis.load(stream);

	const std::vector<Common::UString> &room1 = vis.getVisibilityArray("Room01");
	ASSERT_EQ(room1.size(), 3);

	EXPECT_STREQ(room1[0].c_str(), "Room02");
	EXPECT_STREQ(room1[1].c_str(), "Room03");
	EXPECT_STREQ(room1[2].c_str(), "Room04");

	const std::vector<Common::UString> &room2 = vis.getVisibilityArray("Room02");
	ASSERT_EQ(room2.size(), 3);

	EXPECT_STREQ(room2[0].c_str(), "Room02");
	EXPECT_STREQ(room2[1].c_str(), "Room03");
	EXPECT_STREQ(room2[2].c_str(), "Room04");

	const std::vector<Common::UString> &room3 = vis.getVisibilityArray("Room03");
	ASSERT_EQ(room3.size(), 3);

	EXPECT_STREQ(room3[0].c_str(), "Room02");
	EXPECT_STREQ(room3[1].c_str(), "Room03");
	EXPECT_STREQ(room3[2].c_str(), "Room04");
}
