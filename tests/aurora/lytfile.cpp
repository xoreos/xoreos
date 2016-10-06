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
 *  Unit tests for our LYTFile class.
 */

#include "gtest/gtest.h"

#include "src/common/error.h"
#include "src/common/memreadstream.h"

#include "src/aurora/lytfile.h"

static const char *kLYTFile =
  "#MAXLAYOUT ASCII\n"
  "filedependancy foobar.max\n"
  "beginlayout\n"
  "   roomcount 3\n"
  "      Room01 0.0 1.0 2.0\n"
  "      Room02 0.1 1.1 2.1\n"
  "      Room03 0.2 1.2 2.2\n"
  "   trackcount 0\n"
  "   obstaclecount 0\n"
  "   doorhookcount 4\n"
  "      Room01 Door01 10.0 11.0 12.0 13.0 14.0 15.0 16.0 17.0\n"
  "      Room02 Door02 10.1 11.1 12.1 13.1 14.1 15.1 16.1 17.1\n"
  "      Room02 Door03 10.2 11.2 12.2 13.2 14.2 15.2 16.2 17.2\n"
  "      Room03 Door04 10.3 11.3 12.3 13.3 14.3 15.3 16.3 17.3\n"
  "   artplaceablecount 5\n"
  "      AP01 20.0 21.0 22.0\n"
  "      AP02 20.1 21.1 22.1\n"
  "      AP03 20.2 21.2 22.2\n"
  "      AP04 20.3 21.3 22.3\n"
  "      AP05 20.4 21.4 22.4\n"
  "   walkmeshRooms 6\n"
  "      WMRoom01\n"
  "      WMRoom02\n"
  "      WMRoom03\n"
  "      WMRoom04\n"
  "      WMRoom05\n"
  "      WMRoom06\n"
  "donelayout\n";

GTEST_TEST(LYTFile, getFileDependency) {
	Common::MemoryReadStream stream(kLYTFile);
	Aurora::LYTFile lyt;
	lyt.load(stream);

	EXPECT_STREQ(lyt.getFileDependency().c_str(), "foobar.max");
}

GTEST_TEST(LYTFile, getRooms) {
	Common::MemoryReadStream stream(kLYTFile);
	Aurora::LYTFile lyt;
	lyt.load(stream);

	const Aurora::LYTFile::RoomArray &rooms = lyt.getRooms();
	ASSERT_EQ(rooms.size(), 3);

	EXPECT_STREQ(rooms[0].model.c_str(), "Room01");
	EXPECT_FLOAT_EQ(rooms[0].x, 0.0f);
	EXPECT_FLOAT_EQ(rooms[0].y, 1.0f);
	EXPECT_FLOAT_EQ(rooms[0].z, 2.0f);

	EXPECT_STREQ(rooms[1].model.c_str(), "Room02");
	EXPECT_FLOAT_EQ(rooms[1].x, 0.1f);
	EXPECT_FLOAT_EQ(rooms[1].y, 1.1f);
	EXPECT_FLOAT_EQ(rooms[1].z, 2.1f);

	EXPECT_STREQ(rooms[2].model.c_str(), "Room03");
	EXPECT_FLOAT_EQ(rooms[2].x, 0.2f);
	EXPECT_FLOAT_EQ(rooms[2].y, 1.2f);
	EXPECT_FLOAT_EQ(rooms[2].z, 2.2f);
}

GTEST_TEST(LYTFile, getDoorHooks) {
	Common::MemoryReadStream stream(kLYTFile);
	Aurora::LYTFile lyt;
	lyt.load(stream);

	const Aurora::LYTFile::DoorHookArray &doorHooks = lyt.getDoorHooks();
	ASSERT_EQ(doorHooks.size(), 4);

	EXPECT_STREQ(doorHooks[0].room.c_str(), "Room01");
	EXPECT_STREQ(doorHooks[0].name.c_str(), "Door01");
	EXPECT_FLOAT_EQ(doorHooks[0].x, 10.0);
	EXPECT_FLOAT_EQ(doorHooks[0].y, 11.0);
	EXPECT_FLOAT_EQ(doorHooks[0].z, 12.0);
	EXPECT_FLOAT_EQ(doorHooks[0].unk1, 13.0);
	EXPECT_FLOAT_EQ(doorHooks[0].unk2, 14.0);
	EXPECT_FLOAT_EQ(doorHooks[0].unk3, 15.0);
	EXPECT_FLOAT_EQ(doorHooks[0].unk4, 16.0);
	EXPECT_FLOAT_EQ(doorHooks[0].unk5, 17.0);

	EXPECT_STREQ(doorHooks[1].room.c_str(), "Room02");
	EXPECT_STREQ(doorHooks[1].name.c_str(), "Door02");
	EXPECT_FLOAT_EQ(doorHooks[1].x, 10.1);
	EXPECT_FLOAT_EQ(doorHooks[1].y, 11.1);
	EXPECT_FLOAT_EQ(doorHooks[1].z, 12.1);
	EXPECT_FLOAT_EQ(doorHooks[1].unk1, 13.1);
	EXPECT_FLOAT_EQ(doorHooks[1].unk2, 14.1);
	EXPECT_FLOAT_EQ(doorHooks[1].unk3, 15.1);
	EXPECT_FLOAT_EQ(doorHooks[1].unk4, 16.1);
	EXPECT_FLOAT_EQ(doorHooks[1].unk5, 17.1);

	EXPECT_STREQ(doorHooks[2].room.c_str(), "Room02");
	EXPECT_STREQ(doorHooks[2].name.c_str(), "Door03");
	EXPECT_FLOAT_EQ(doorHooks[2].x, 10.2);
	EXPECT_FLOAT_EQ(doorHooks[2].y, 11.2);
	EXPECT_FLOAT_EQ(doorHooks[2].z, 12.2);
	EXPECT_FLOAT_EQ(doorHooks[2].unk1, 13.2);
	EXPECT_FLOAT_EQ(doorHooks[2].unk2, 14.2);
	EXPECT_FLOAT_EQ(doorHooks[2].unk3, 15.2);
	EXPECT_FLOAT_EQ(doorHooks[2].unk4, 16.2);
	EXPECT_FLOAT_EQ(doorHooks[2].unk5, 17.2);

	EXPECT_STREQ(doorHooks[3].room.c_str(), "Room03");
	EXPECT_STREQ(doorHooks[3].name.c_str(), "Door04");
	EXPECT_FLOAT_EQ(doorHooks[3].x, 10.3);
	EXPECT_FLOAT_EQ(doorHooks[3].y, 11.3);
	EXPECT_FLOAT_EQ(doorHooks[3].z, 12.3);
	EXPECT_FLOAT_EQ(doorHooks[3].unk1, 13.3);
	EXPECT_FLOAT_EQ(doorHooks[3].unk2, 14.3);
	EXPECT_FLOAT_EQ(doorHooks[3].unk3, 15.3);
	EXPECT_FLOAT_EQ(doorHooks[3].unk4, 16.3);
	EXPECT_FLOAT_EQ(doorHooks[3].unk5, 17.3);
}

GTEST_TEST(LYTFile, getArtPlaceables) {
	Common::MemoryReadStream stream(kLYTFile);
	Aurora::LYTFile lyt;
	lyt.load(stream);

	const Aurora::LYTFile::ArtPlaceableArray &artPlaceables = lyt.getArtPlaceables();
	ASSERT_EQ(artPlaceables.size(), 5);

	EXPECT_STREQ(artPlaceables[0].model.c_str(), "AP01");
	EXPECT_FLOAT_EQ(artPlaceables[0].x, 20.0);
	EXPECT_FLOAT_EQ(artPlaceables[0].y, 21.0);
	EXPECT_FLOAT_EQ(artPlaceables[0].z, 22.0);

	EXPECT_STREQ(artPlaceables[1].model.c_str(), "AP02");
	EXPECT_FLOAT_EQ(artPlaceables[1].x, 20.1);
	EXPECT_FLOAT_EQ(artPlaceables[1].y, 21.1);
	EXPECT_FLOAT_EQ(artPlaceables[1].z, 22.1);

	EXPECT_STREQ(artPlaceables[2].model.c_str(), "AP03");
	EXPECT_FLOAT_EQ(artPlaceables[2].x, 20.2);
	EXPECT_FLOAT_EQ(artPlaceables[2].y, 21.2);
	EXPECT_FLOAT_EQ(artPlaceables[2].z, 22.2);

	EXPECT_STREQ(artPlaceables[3].model.c_str(), "AP04");
	EXPECT_FLOAT_EQ(artPlaceables[3].x, 20.3);
	EXPECT_FLOAT_EQ(artPlaceables[3].y, 21.3);
	EXPECT_FLOAT_EQ(artPlaceables[3].z, 22.3);

	EXPECT_STREQ(artPlaceables[4].model.c_str(), "AP05");
	EXPECT_FLOAT_EQ(artPlaceables[4].x, 20.4);
	EXPECT_FLOAT_EQ(artPlaceables[4].y, 21.4);
	EXPECT_FLOAT_EQ(artPlaceables[4].z, 22.4);
}

GTEST_TEST(LYTFile, clear) {
	Common::MemoryReadStream stream(kLYTFile);
	Aurora::LYTFile lyt;
	lyt.load(stream);

	lyt.clear();

	EXPECT_STREQ(lyt.getFileDependency().c_str(), "");

	ASSERT_EQ(lyt.getRooms().size(), 0);
	ASSERT_EQ(lyt.getArtPlaceables().size(), 0);
	ASSERT_EQ(lyt.getDoorHooks().size(), 0);
}

GTEST_TEST(LYTFile, doubleRooms) {
	static const char *kLYTFileVariant =
		"#MAXLAYOUT ASCII\n"
		"filedependancy foobar.max\n"
		"beginlayout\n"
		"   roomcount 3\n"
		"      Room01 0.0 1.0 2.0\n"
		"      Room02 0.1 1.1 2.1\n"
		"      Room03 0.2 1.2 2.2\n"
		"   roomcount 4\n"
		"      Room01 0.0 1.0 2.0\n"
		"      Room02 0.1 1.1 2.1\n"
		"      Room03 0.2 1.2 2.2\n"
		"      Room04 0.3 1.3 2.3\n"
		"donelayout\n";

	Common::MemoryReadStream stream(kLYTFileVariant);
	Aurora::LYTFile lyt;
	lyt.load(stream);

	ASSERT_EQ(lyt.getRooms().size(), 4);
}

GTEST_TEST(LYTFile, doubleRoomsAfterDoneLayout) {
	static const char *kLYTFileVariant =
		"#MAXLAYOUT ASCII\n"
		"filedependancy foobar.max\n"
		"beginlayout\n"
		"   roomcount 3\n"
		"      Room01 0.0 1.0 2.0\n"
		"      Room02 0.1 1.1 2.1\n"
		"      Room03 0.2 1.2 2.2\n"
		"donelayout\n"
		"   roomcount 4\n"
		"      Room01 0.0 1.0 2.0\n"
		"      Room02 0.1 1.1 2.1\n"
		"      Room03 0.2 1.2 2.2\n"
		"      Room04 0.3 1.3 2.3\n";

	Common::MemoryReadStream stream(kLYTFileVariant);
	Aurora::LYTFile lyt;
	lyt.load(stream);

	ASSERT_EQ(lyt.getRooms().size(), 3);
}

GTEST_TEST(LYTFile, tooManyRooms) {
	static const char *kLYTFileVariant =
		"#MAXLAYOUT ASCII\n"
		"filedependancy foobar.max\n"
		"beginlayout\n"
		"   roomcount 3\n"
		"      Room01 0.0 1.0 2.0\n"
		"      Room02 0.1 1.1 2.1\n"
		"      Room03 0.2 1.2 2.2\n"
		"      Room04 0.3 1.3 2.3\n"
		"donelayout\n";

	Common::MemoryReadStream stream(kLYTFileVariant);
	Aurora::LYTFile lyt;

	EXPECT_THROW(lyt.load(stream), Common::Exception);
}

GTEST_TEST(LYTFile, tooFewRooms) {
	static const char *kLYTFileVariant =
		"#MAXLAYOUT ASCII\n"
		"filedependancy foobar.max\n"
		"beginlayout\n"
		"   roomcount 3\n"
		"      Room01 0.0 1.0 2.0\n"
		"      Room02 0.1 1.1 2.1\n"
		"donelayout\n";

	Common::MemoryReadStream stream(kLYTFileVariant);
	Aurora::LYTFile lyt;

	EXPECT_THROW(lyt.load(stream), Common::Exception);
}

GTEST_TEST(LYTFile, tooManyTokensRooms) {
	static const char *kLYTFileVariant =
		"#MAXLAYOUT ASCII\n"
		"filedependancy foobar.max\n"
		"beginlayout\n"
		"   roomcount 2\n"
		"      Room01 0.0 1.0 2.0\n"
		"      Room02 0.1 1.1 2.1 3.1\n"
		"donelayout\n";

	Common::MemoryReadStream stream(kLYTFileVariant);
	Aurora::LYTFile lyt;

	EXPECT_THROW(lyt.load(stream), Common::Exception);
}

GTEST_TEST(LYTFile, tooFewTokensRooms) {
	static const char *kLYTFileVariant =
		"#MAXLAYOUT ASCII\n"
		"filedependancy foobar.max\n"
		"beginlayout\n"
		"   roomcount 2\n"
		"      Room01 0.0 1.0 2.0\n"
		"      Room02 0.1 1.1\n"
		"donelayout\n";

	Common::MemoryReadStream stream(kLYTFileVariant);
	Aurora::LYTFile lyt;

	EXPECT_THROW(lyt.load(stream), Common::Exception);
}

GTEST_TEST(LYTFile, tooManyTokensDoors) {
	static const char *kLYTFileVariant =
		"#MAXLAYOUT ASCII\n"
		"filedependancy foobar.max\n"
		"beginlayout\n"
		"   doorhookcount 2\n"
		"      Room01 Door01 10.0 11.0 12.0 13.0 14.0 15.0 16.0 17.0\n"
		"      Room02 Door02 10.1 11.1 12.1 13.1 14.1 15.1 16.1 17.1 18.1\n"
		"donelayout\n";

	Common::MemoryReadStream stream(kLYTFileVariant);
	Aurora::LYTFile lyt;

	EXPECT_THROW(lyt.load(stream), Common::Exception);
}

GTEST_TEST(LYTFile, tooFewTokensDoors) {
	static const char *kLYTFileVariant =
		"#MAXLAYOUT ASCII\n"
		"filedependancy foobar.max\n"
		"beginlayout\n"
		"   doorhookcount 2\n"
		"      Room01 Door01 10.0 11.0 12.0 13.0 14.0 15.0 16.0 17.0\n"
		"      Room02 Door02 10.1 11.1 12.1 13.1 14.1 15.1 16.1\n"
		"donelayout\n";

	Common::MemoryReadStream stream(kLYTFileVariant);
	Aurora::LYTFile lyt;

	EXPECT_THROW(lyt.load(stream), Common::Exception);
}

GTEST_TEST(LYTFile, tooManyTokensArtPlaceables) {
	static const char *kLYTFileVariant =
		"#MAXLAYOUT ASCII\n"
		"filedependancy foobar.max\n"
		"beginlayout\n"
		"   artplaceablecount 2\n"
		"      AP01 20.0 21.0 22.0\n"
		"      AP02 20.1 21.1 22.1 23.1\n"
		"donelayout\n";

	Common::MemoryReadStream stream(kLYTFileVariant);
	Aurora::LYTFile lyt;

	EXPECT_THROW(lyt.load(stream), Common::Exception);
}

GTEST_TEST(LYTFile, tooFewTokensArtPlaceables) {
	static const char *kLYTFileVariant =
		"#MAXLAYOUT ASCII\n"
		"filedependancy foobar.max\n"
		"beginlayout\n"
		"   artplaceablecount 2\n"
		"      AP01 20.0 21.0 22.0\n"
		"      AP02 20.1 21.1\n"
		"donelayout\n";

	Common::MemoryReadStream stream(kLYTFileVariant);
	Aurora::LYTFile lyt;

	EXPECT_THROW(lyt.load(stream), Common::Exception);
}

GTEST_TEST(LYTFile, wrongTokens) {
	static const char *kLYTFileVariant =
		"#MAXLAYOUT ASCII\n"
		"filedependancy foobar.max\n"
		"beginlayout\n"
		"   roomcount 2\n"
		"      Room01 Foo Bar Barfoo\n"
		"      Room02 0.1 1.1 2.1\n"
		"donelayout\n";

	Common::MemoryReadStream stream(kLYTFileVariant);
	Aurora::LYTFile lyt;

	EXPECT_THROW(lyt.load(stream), Common::Exception);
}

GTEST_TEST(LYTFile, unknownToken) {
	static const char *kLYTFileVariant =
		"#MAXLAYOUT ASCII\n"
		"filedependancy foobar.max\n"
		"beginlayout\n"
		"   nopecount 2\n"
		"      Nope01 0.1 1.1 2.1\n"
		"      Nope02 0.1 1.1 2.1\n"
		"donelayout\n";

	Common::MemoryReadStream stream(kLYTFileVariant);
	Aurora::LYTFile lyt;

	EXPECT_THROW(lyt.load(stream), Common::Exception);
}
