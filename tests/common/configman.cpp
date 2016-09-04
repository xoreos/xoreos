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
 *  Unit tests for our config manager.
 */

#include "gtest/gtest.h"

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/memreadstream.h"
#include "src/common/memwritestream.h"
#include "src/common/configman.h"

static const char *kConfigFile = "[xoreos]\n"
                                 "width=640\n"
                                 "height=480\n\n"
                                 "[game1]\n"
                                 "path=/path/to/game1/\n"
                                 "width=800\n"
                                 "height=600\n"
                                 "fullscreen=true\n\n"
                                 "[game2]\n"
                                 "path=/path/to/game2\n\n";

class ConfigManager : public ::testing::Test {
protected:
	void SetUp() {
		ConfigMan.clear();
	}

	void TearDown() {
		Common::ConfigManager::destroy();
	}
};

static void loadConfigMan() {
	Common::MemoryReadStream stream(kConfigFile);
	ConfigMan.load(stream);
}


GTEST_TEST_F(ConfigManager, create) {
	ConfigMan.create();
}

GTEST_TEST_F(ConfigManager, load) {
	loadConfigMan();
}

GTEST_TEST_F(ConfigManager, hasGame) {
	loadConfigMan();

	EXPECT_STREQ(ConfigMan.findGame("/path/to/game1/").c_str(), "game1");
	EXPECT_STREQ(ConfigMan.findGame("/path/to/game1" ).c_str(), "game1");

	EXPECT_STREQ(ConfigMan.findGame("/path/to/game2/").c_str(), "game2");
	EXPECT_STREQ(ConfigMan.findGame("/path/to/game2" ).c_str(), "game2");

	EXPECT_STREQ(ConfigMan.findGame("/path/to/nope/" ).c_str(), "");
	EXPECT_STREQ(ConfigMan.findGame("/path/to/nope"  ).c_str(), "");
}

GTEST_TEST_F(ConfigManager, findGame) {
	loadConfigMan();

	EXPECT_TRUE(ConfigMan.hasGame("game1"));
	EXPECT_TRUE(ConfigMan.hasGame("game2"));

	EXPECT_FALSE(ConfigMan.hasGame("nope"));
}

GTEST_TEST_F(ConfigManager, setGame) {
	loadConfigMan();

	EXPECT_FALSE(ConfigMan.isInGame());

	EXPECT_TRUE(ConfigMan.setGame("game1"));
	EXPECT_TRUE(ConfigMan.isInGame());

	EXPECT_TRUE(ConfigMan.setGame(""));
	EXPECT_FALSE(ConfigMan.isInGame());
}

GTEST_TEST_F(ConfigManager, hasKey) {
	loadConfigMan();

	EXPECT_TRUE(ConfigMan.hasKey("width"));
	EXPECT_FALSE(ConfigMan.hasKey("fullscreen"));

	EXPECT_TRUE(ConfigMan.setGame("game1"));
	EXPECT_TRUE(ConfigMan.hasKey("width"));
	EXPECT_TRUE(ConfigMan.hasKey("fullscreen"));

	EXPECT_TRUE(ConfigMan.setGame("game2"));
	EXPECT_TRUE(ConfigMan.hasKey("width"));
	EXPECT_FALSE(ConfigMan.hasKey("fullscreen"));
}

GTEST_TEST_F(ConfigManager, get) {
	loadConfigMan();

	EXPECT_EQ(ConfigMan.getInt("width"), 640);

	EXPECT_TRUE(ConfigMan.setGame("game1"));
	EXPECT_EQ(ConfigMan.getInt("width"), 800);

	EXPECT_TRUE(ConfigMan.setGame("game2"));
	EXPECT_EQ(ConfigMan.getInt("width"), 640);
}

GTEST_TEST_F(ConfigManager, set) {
	loadConfigMan();

	EXPECT_FALSE(ConfigMan.changed());

	EXPECT_EQ(ConfigMan.getInt("width"), 640);

	ConfigMan.setInt("width", 1000);
	EXPECT_EQ(ConfigMan.getInt("width"), 1000);

	EXPECT_TRUE(ConfigMan.changed());

	EXPECT_TRUE(ConfigMan.setGame("game1"));
	EXPECT_EQ(ConfigMan.getInt("width"), 800);

	ConfigMan.setInt("width", 1200);
	EXPECT_EQ(ConfigMan.getInt("width"), 1200);

	EXPECT_TRUE(ConfigMan.setGame("game2"));
	EXPECT_EQ(ConfigMan.getInt("width"), 1000);
}

GTEST_TEST_F(ConfigManager, createGame) {
	loadConfigMan();

	EXPECT_STREQ(ConfigMan.createGame("/path/to/game3/").c_str(), "game3");
	EXPECT_STREQ(ConfigMan.createGame("/path/to/game3/").c_str(), "game3_0");
	EXPECT_STREQ(ConfigMan.createGame("/path/to/game3/").c_str(), "game3_1");
}

static void compareStream(Common::MemoryWriteStreamDynamic &stream, const char *data) {
	ASSERT_EQ(stream.size(), strlen(data));

	for (size_t i = 0; i < strlen(data); i++)
		EXPECT_EQ(stream.getData()[i], data[i]) << "At index " << i;
}

GTEST_TEST_F(ConfigManager, save) {
	loadConfigMan();

	Common::MemoryWriteStreamDynamic writeStream(true);
	ConfigMan.save(writeStream);

	compareStream(writeStream, kConfigFile);
}

GTEST_TEST_F(ConfigManager, newSave) {
	ConfigMan.create();

	ConfigMan.setInt("width", 640);
	ConfigMan.setInt("height", 480);

	const Common::UString game1 = ConfigMan.createGame("/path/to/game1/");

	ConfigMan.setGame(game1);

	ConfigMan.setString("path", "/path/to/game1/");
	ConfigMan.setInt("width", 800);
	ConfigMan.setInt("height", 600);
	ConfigMan.setBool("fullscreen", true);

	const Common::UString game2 = ConfigMan.createGame("/path/to/game2/");

	ConfigMan.setGame(game2);

	ConfigMan.setString("path", "/path/to/game2");

	Common::MemoryWriteStreamDynamic writeStream(true);
	ConfigMan.save(writeStream);

	compareStream(writeStream, kConfigFile);
}
