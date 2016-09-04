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
 *  Unit tests for our config (INI) file reader.
 */

#include "gtest/gtest.h"

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/memreadstream.h"
#include "src/common/memwritestream.h"
#include "src/common/configfile.h"

static const char *kConfigFile = "[section1]\n"
                                 "key1 = 23\n"
                                 "key2=-5\n"
                                 "key3=string\n"
                                 "key4=true\n"
                                 "key5=false\n"
                                 "key6=0.5\n"
                                 "key7= need trimming   \n";

static void loadConfig(Common::ConfigFile &config, const char *string) {
	Common::MemoryReadStream stream(string);

	config.load(stream);
}

GTEST_TEST(ConfigFile, isValidName) {
	EXPECT_TRUE(Common::ConfigFile::isValidName("foobar"));
	EXPECT_TRUE(Common::ConfigFile::isValidName("Foobar"));
	EXPECT_TRUE(Common::ConfigFile::isValidName("Foobar Barfoo"));
	EXPECT_TRUE(Common::ConfigFile::isValidName("Foobar_Barfoo"));

	EXPECT_FALSE(Common::ConfigFile::isValidName("foo#bar"));
	EXPECT_FALSE(Common::ConfigFile::isValidName("foo=bar"));
	EXPECT_FALSE(Common::ConfigFile::isValidName("foo[bar"));
	EXPECT_FALSE(Common::ConfigFile::isValidName("foo]bar"));
}

GTEST_TEST(ConfigFile, parseFail) {
	Common::ConfigFile config;
	EXPECT_THROW(loadConfig(config, "[section\nkey1=5"), Common::Exception);

	config.clear();
	EXPECT_THROW(loadConfig(config, "[section]\n]key1"), Common::Exception);

	config.clear();
	EXPECT_THROW(loadConfig(config, "[section]\n=value"), Common::Exception);
}

GTEST_TEST(ConfigFile, hasDomain) {
	Common::ConfigFile config;
	loadConfig(config, kConfigFile);

	EXPECT_TRUE(config.hasDomain("section1"));
	EXPECT_TRUE(config.hasDomain("Section1"));

	EXPECT_FALSE(config.hasDomain("key1"));
	EXPECT_FALSE(config.hasDomain("nope"));
}

GTEST_TEST(ConfigFile, clear) {
	Common::ConfigFile config;
	loadConfig(config, kConfigFile);

	EXPECT_TRUE(config.hasDomain("section1"));

	config.clear();

	EXPECT_FALSE(config.hasDomain("section1"));
}

GTEST_TEST(ConfigFile, renameDomain) {
	Common::ConfigFile config;
	loadConfig(config, kConfigFile);

	EXPECT_FALSE(config.renameDomain("nope", "section2"));

	EXPECT_FALSE(config.renameDomain("section1", "section1"));

	EXPECT_TRUE(config.renameDomain("section1", "section2"));

	EXPECT_FALSE(config.hasDomain("section1"));
	EXPECT_TRUE(config.hasDomain("section2"));
}

GTEST_TEST(ConfigFile, removeDomain) {
	Common::ConfigFile config;
	loadConfig(config, kConfigFile);

	EXPECT_FALSE(config.removeDomain("nope"));

	EXPECT_TRUE(config.removeDomain("section1"));

	EXPECT_FALSE(config.hasDomain("section1"));
}

GTEST_TEST(ConfigFile, addDomain) {
	Common::ConfigFile config;
	loadConfig(config, kConfigFile);

	Common::ConfigDomain *domain = 0;

	domain = config.addDomain("section1");
	ASSERT_NE(domain, static_cast<Common::ConfigDomain *>(0));

	EXPECT_TRUE(domain->hasKey("key1"));

	domain = config.addDomain("newsection");
	ASSERT_NE(domain, static_cast<Common::ConfigDomain *>(0));

	EXPECT_FALSE(domain->hasKey("key1"));

	EXPECT_EQ(config.getDomain("newsection"), domain);
}

GTEST_TEST(ConfigDomain, getName) {
	Common::ConfigFile config;
	loadConfig(config, kConfigFile);

	Common::ConfigDomain *domain = config.getDomain("section1");
	ASSERT_NE(domain, static_cast<Common::ConfigDomain *>(0));

	EXPECT_STREQ(domain->getName().c_str(), "section1");
}

GTEST_TEST(ConfigDomain, hasKey) {
	Common::ConfigFile config;
	loadConfig(config, kConfigFile);

	Common::ConfigDomain *domain = config.getDomain("section1");
	ASSERT_NE(domain, static_cast<Common::ConfigDomain *>(0));

	EXPECT_TRUE(domain->hasKey("key1"));
	EXPECT_TRUE(domain->hasKey("Key1"));

	EXPECT_FALSE(domain->hasKey("nope"));
}

GTEST_TEST(ConfigDomain, renameKey) {
	Common::ConfigFile config;
	loadConfig(config, kConfigFile);

	Common::ConfigDomain *domain = config.getDomain("section1");
	ASSERT_NE(domain, static_cast<Common::ConfigDomain *>(0));

	EXPECT_FALSE(domain->renameKey("nope", "newkey"));

	EXPECT_FALSE(domain->renameKey("key1", "key2"));

	EXPECT_TRUE(domain->renameKey("key1", "newkey"));

	EXPECT_FALSE(domain->hasKey("key1"));
	EXPECT_TRUE(domain->hasKey("newkey"));
}

GTEST_TEST(ConfigDomain, removeKey) {
	Common::ConfigFile config;
	loadConfig(config, kConfigFile);

	Common::ConfigDomain *domain = config.getDomain("section1");
	ASSERT_NE(domain, static_cast<Common::ConfigDomain *>(0));

	EXPECT_FALSE(domain->removeKey("nope"));

	EXPECT_TRUE(domain->removeKey("key1"));

	EXPECT_FALSE(domain->hasKey("key1"));
}

GTEST_TEST(ConfigDomain, getString) {
	Common::ConfigFile config;
	loadConfig(config, kConfigFile);

	Common::ConfigDomain *domain = config.getDomain("section1");
	ASSERT_NE(domain, static_cast<Common::ConfigDomain *>(0));

	EXPECT_STREQ(domain->getString("key1").c_str(), "23");
	EXPECT_STREQ(domain->getString("key2").c_str(), "-5");
	EXPECT_STREQ(domain->getString("key3").c_str(), "string");
	EXPECT_STREQ(domain->getString("key4").c_str(), "true");
	EXPECT_STREQ(domain->getString("key5").c_str(), "false");
	EXPECT_STREQ(domain->getString("key6").c_str(), "0.5");
	EXPECT_STREQ(domain->getString("key7").c_str(), "need trimming");

	EXPECT_STREQ(domain->getString("nope").c_str(), "");
}

GTEST_TEST(ConfigDomain, getBool) {
	Common::ConfigFile config;
	loadConfig(config, kConfigFile);

	Common::ConfigDomain *domain = config.getDomain("section1");
	ASSERT_NE(domain, static_cast<Common::ConfigDomain *>(0));

	EXPECT_EQ(domain->getBool("key1"), false);
	EXPECT_EQ(domain->getBool("key2"), false);
	EXPECT_EQ(domain->getBool("key3"), false);
	EXPECT_EQ(domain->getBool("key4"), true);
	EXPECT_EQ(domain->getBool("key5"), false);
	EXPECT_EQ(domain->getBool("key6"), false);
	EXPECT_EQ(domain->getBool("key7"), false);

	EXPECT_EQ(domain->getBool("nope"), false);
}

GTEST_TEST(ConfigDomain, getInt) {
	Common::ConfigFile config;
	loadConfig(config, kConfigFile);

	Common::ConfigDomain *domain = config.getDomain("section1");
	ASSERT_NE(domain, static_cast<Common::ConfigDomain *>(0));

	EXPECT_EQ(domain->getInt("key1"), 23);
	EXPECT_EQ(domain->getInt("key2"), -5);
	EXPECT_EQ(domain->getInt("key3"), 0);
	EXPECT_EQ(domain->getInt("key4"), 0);
	EXPECT_EQ(domain->getInt("key5"), 0);
	EXPECT_EQ(domain->getInt("key6"), 0);
	EXPECT_EQ(domain->getInt("key7"), 0);

	EXPECT_EQ(domain->getInt("nope"), 0);
}

GTEST_TEST(ConfigDomain, getUint) {
	Common::ConfigFile config;
	loadConfig(config, kConfigFile);

	Common::ConfigDomain *domain = config.getDomain("section1");
	ASSERT_NE(domain, static_cast<Common::ConfigDomain *>(0));

	EXPECT_EQ(domain->getUint("key1"), 23);
	EXPECT_EQ(domain->getUint("key2"), 0);
	EXPECT_EQ(domain->getUint("key3"), 0);
	EXPECT_EQ(domain->getUint("key4"), 0);
	EXPECT_EQ(domain->getUint("key5"), 0);
	EXPECT_EQ(domain->getUint("key6"), 0);
	EXPECT_EQ(domain->getUint("key7"), 0);

	EXPECT_EQ(domain->getUint("nope"), 0);
}

GTEST_TEST(ConfigDomain, getDouble) {
	Common::ConfigFile config;
	loadConfig(config, kConfigFile);

	Common::ConfigDomain *domain = config.getDomain("section1");
	ASSERT_NE(domain, static_cast<Common::ConfigDomain *>(0));

	EXPECT_EQ(domain->getDouble("key1"), 23.0);
	EXPECT_EQ(domain->getDouble("key2"), -5.0);
	EXPECT_EQ(domain->getDouble("key3"), 0.0);
	EXPECT_EQ(domain->getDouble("key4"), 0.0);
	EXPECT_EQ(domain->getDouble("key5"), 0.0);
	EXPECT_EQ(domain->getDouble("key6"), 0.5);
	EXPECT_EQ(domain->getDouble("key7"), 0.0);

	EXPECT_EQ(domain->getDouble("nope"), 0.0);
}

GTEST_TEST(ConfigDomain, setString) {
	Common::ConfigFile config;
	loadConfig(config, kConfigFile);

	Common::ConfigDomain *domain = config.getDomain("section1");
	ASSERT_NE(domain, static_cast<Common::ConfigDomain *>(0));

	domain->setString("newkey", "newvalue");

	EXPECT_STREQ(domain->getString("newkey").c_str(), "newvalue");
}

GTEST_TEST(ConfigDomain, setBool) {
	Common::ConfigFile config;
	loadConfig(config, kConfigFile);

	Common::ConfigDomain *domain = config.getDomain("section1");
	ASSERT_NE(domain, static_cast<Common::ConfigDomain *>(0));

	domain->setBool("newkey", true);

	EXPECT_TRUE(domain->getBool("newkey"));
}

GTEST_TEST(ConfigDomain, setInt) {
	Common::ConfigFile config;
	loadConfig(config, kConfigFile);

	Common::ConfigDomain *domain = config.getDomain("section1");
	ASSERT_NE(domain, static_cast<Common::ConfigDomain *>(0));

	domain->setInt("newkey", -42);

	EXPECT_EQ(domain->getInt("newkey"), -42);
}

GTEST_TEST(ConfigDomain, setUint) {
	Common::ConfigFile config;
	loadConfig(config, kConfigFile);

	Common::ConfigDomain *domain = config.getDomain("section1");
	ASSERT_NE(domain, static_cast<Common::ConfigDomain *>(0));

	domain->setUint("newkey", 42);

	EXPECT_EQ(domain->getUint("newkey"), 42);
}

GTEST_TEST(ConfigDomain, setDouble) {
	Common::ConfigFile config;
	loadConfig(config, kConfigFile);

	Common::ConfigDomain *domain = config.getDomain("section1");
	ASSERT_NE(domain, static_cast<Common::ConfigDomain *>(0));

	domain->setDouble("newkey", 42.23);

	EXPECT_EQ(domain->getDouble("newkey"), 42.23);
}

GTEST_TEST(ConfigDomain, setNoClobber) {
	Common::ConfigFile config;
	loadConfig(config, kConfigFile);

	Common::ConfigDomain *oldDomain = config.getDomain("section1");
	ASSERT_NE(oldDomain, static_cast<Common::ConfigDomain *>(0));

	Common::ConfigDomain newDomain("newsection");
	newDomain.setInt("key1", 42);
	newDomain.setInt("newkey", 50);

	oldDomain->set(newDomain, false);

	EXPECT_EQ(oldDomain->getInt("key1"), 23);
	EXPECT_EQ(oldDomain->getInt("newkey"), 50);
}

GTEST_TEST(ConfigDomain, setClobber) {
	Common::ConfigFile config;
	loadConfig(config, kConfigFile);

	Common::ConfigDomain *oldDomain = config.getDomain("section1");
	ASSERT_NE(oldDomain, static_cast<Common::ConfigDomain *>(0));

	Common::ConfigDomain newDomain("newsection");
	newDomain.setInt("key1", 42);
	newDomain.setInt("newkey", 50);

	oldDomain->set(newDomain, true);

	EXPECT_EQ(oldDomain->getInt("key1"), 42);
	EXPECT_EQ(oldDomain->getInt("newkey"), 50);
}

GTEST_TEST(ConfigFile, create) {
	static const char *compareFile = "[section]\n"
	                                 "key1=string\n"
	                                 "key2=true\n"
	                                 "key3=-42\n"
	                                 "key4=42\n\n";

	Common::ConfigFile config;

	Common::ConfigDomain *domain = config.addDomain("section");
	ASSERT_NE(domain, static_cast<Common::ConfigDomain *>(0));

	domain->setString("key1", "string");
	domain->setBool  ("key2", true);
	domain->setInt   ("key3", -42);
	domain->setUint  ("key4", 42);

	Common::MemoryWriteStreamDynamic writeStream(true);
	config.save(writeStream);

	ASSERT_EQ(writeStream.size(), strlen(compareFile));

	for (size_t i = 0; i < strlen(compareFile); i++)
		EXPECT_EQ(writeStream.getData()[i], compareFile[i]) << "At index " << i;
}
