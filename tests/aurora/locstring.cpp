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
 *  Unit tests for our LocString class.
 */

#include <cstdio>

#include "gtest/gtest.h"

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/ustring.h"
#include "src/common/memreadstream.h"

#include "src/aurora/locstring.h"
#include "src/aurora/language.h"
#include "src/aurora/talkman.h"

static const char *kUTF8String  = "F""\xc3""\xb6""\xc3""\xb6""b""\xc3""\xa4""r";
static const char *kUTF8String0 = "F""\xc3""\xb6""\xc3""\xb6""b""\xc3""\xa4""r0";
static const char *kUTF8String1 = "F""\xc3""\xb6""\xc3""\xb6""b""\xc3""\xa4""r1";

class LocString : public ::testing::Test {
protected:
	static void SetUpTestCase() {
		LangMan.clear();
		TalkMan.clear();

		LangMan.addLanguage(Aurora::kLanguageEnglish, 0, Common::kEncodingUTF8);
		LangMan.addLanguage(Aurora::kLanguageFrench , 1, Common::kEncodingCP1252);
	}

	static void TearDownTestCase() {
		Aurora::LanguageManager::destroy();
		Aurora::TalkManager::destroy();
	}
};


GTEST_TEST_F(LocString, empty) {
	const Aurora::LocString locString;

	EXPECT_TRUE(locString.empty());
}

GTEST_TEST_F(LocString, setID) {
	Aurora::LocString locString;

	locString.setID(23);

	EXPECT_EQ(locString.getID(), 23);
}

GTEST_TEST_F(LocString, getStrRefString) {
	const Aurora::LocString locString;

	EXPECT_STREQ(locString.getStrRefString().c_str(), "");
}

GTEST_TEST_F(LocString, setStringGender) {
	Aurora::LocString locString;

	locString.setString(Aurora::kLanguageEnglish, Aurora::kLanguageGenderFemale, kUTF8String);

	EXPECT_FALSE(locString.hasString(Aurora::kLanguageEnglish, Aurora::kLanguageGenderMale  ));
	EXPECT_TRUE(locString.hasString(Aurora::kLanguageEnglish, Aurora::kLanguageGenderFemale));

	EXPECT_STREQ(locString.getString(Aurora::kLanguageEnglish, Aurora::kLanguageGenderMale  ).c_str(),
	             "");
	EXPECT_STREQ(locString.getString(Aurora::kLanguageEnglish, Aurora::kLanguageGenderFemale).c_str(),
	             kUTF8String);

	EXPECT_STREQ(locString.getFirstString().c_str(), kUTF8String);

	EXPECT_STREQ(locString.getString().c_str(), kUTF8String);
}

GTEST_TEST_F(LocString, setString) {
	Aurora::LocString locString;

	locString.setString(Aurora::kLanguageEnglish, kUTF8String);

	EXPECT_TRUE(locString.hasString(Aurora::kLanguageEnglish, Aurora::kLanguageGenderMale  ));
	EXPECT_TRUE(locString.hasString(Aurora::kLanguageEnglish, Aurora::kLanguageGenderFemale));

	EXPECT_STREQ(locString.getString(Aurora::kLanguageEnglish, Aurora::kLanguageGenderMale  ).c_str(),
	             kUTF8String);
	EXPECT_STREQ(locString.getString(Aurora::kLanguageEnglish, Aurora::kLanguageGenderFemale).c_str(),
	             kUTF8String);

	EXPECT_STREQ(locString.getFirstString().c_str(), kUTF8String);

	EXPECT_STREQ(locString.getString().c_str(), kUTF8String);
}

GTEST_TEST_F(LocString, readString) {
	static const byte kData[] = { 0x09, 0x00, 0x00, 0x00, 0x46, 0xC3, 0xB6, 0xC3,
	                              0xB6, 0x62, 0xC3, 0xA4, 0x72 };
	Common::MemoryReadStream stream(kData);

	Aurora::LocString locString;
	locString.readString(1, stream);

	EXPECT_FALSE(locString.hasString(Aurora::kLanguageEnglish, Aurora::kLanguageGenderMale  ));
	EXPECT_TRUE (locString.hasString(Aurora::kLanguageEnglish, Aurora::kLanguageGenderFemale));

	EXPECT_STREQ(locString.getString(Aurora::kLanguageEnglish, Aurora::kLanguageGenderMale  ).c_str(),
	             "");
	EXPECT_STREQ(locString.getString(Aurora::kLanguageEnglish, Aurora::kLanguageGenderFemale).c_str(),
	             kUTF8String);

	EXPECT_STREQ(locString.getFirstString().c_str(), kUTF8String);

	EXPECT_STREQ(locString.getString().c_str(), kUTF8String);
}

GTEST_TEST_F(LocString, readLocSubString) {
	static const byte kData[] = { 0x01, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00,
	                              0x46, 0xC3, 0xB6, 0xC3, 0xB6, 0x62, 0xC3, 0xA4,
	                              0x72 };
	Common::MemoryReadStream stream(kData);

	Aurora::LocString locString;
	locString.readLocSubString(stream);

	EXPECT_FALSE(locString.hasString(Aurora::kLanguageEnglish, Aurora::kLanguageGenderMale  ));
	EXPECT_TRUE (locString.hasString(Aurora::kLanguageEnglish, Aurora::kLanguageGenderFemale));

	EXPECT_STREQ(locString.getString(Aurora::kLanguageEnglish, Aurora::kLanguageGenderMale  ).c_str(),
	             "");
	EXPECT_STREQ(locString.getString(Aurora::kLanguageEnglish, Aurora::kLanguageGenderFemale).c_str(),
	             kUTF8String);

	EXPECT_STREQ(locString.getFirstString().c_str(), kUTF8String);

	EXPECT_STREQ(locString.getString().c_str(), kUTF8String);
}

GTEST_TEST_F(LocString, readLocStringIDCount) {
	static const byte kData[] = { 0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00,
	                              0x46, 0xC3, 0xB6, 0xC3, 0xB6, 0x62, 0xC3, 0xA4,
	                              0x72, 0x30, 0x01, 0x00, 0x00, 0x00, 0x0A, 0x00,
	                              0x00, 0x00, 0x46, 0xC3, 0xB6, 0xC3, 0xB6, 0x62,
	                              0xC3, 0xA4, 0x72, 0x31 };
	Common::MemoryReadStream stream(kData);

	Aurora::LocString locString;
	locString.readLocString(stream, 23, 2);

	EXPECT_EQ(locString.getID(), 23);

	EXPECT_TRUE(locString.hasString(Aurora::kLanguageEnglish, Aurora::kLanguageGenderMale  ));
	EXPECT_TRUE(locString.hasString(Aurora::kLanguageEnglish, Aurora::kLanguageGenderFemale));

	EXPECT_STREQ(locString.getString(Aurora::kLanguageEnglish, Aurora::kLanguageGenderMale  ).c_str(),
	             kUTF8String0);
	EXPECT_STREQ(locString.getString(Aurora::kLanguageEnglish, Aurora::kLanguageGenderFemale).c_str(),
	             kUTF8String1);

	EXPECT_STREQ(locString.getFirstString().c_str(), kUTF8String0);

	EXPECT_STREQ(locString.getString().c_str(), kUTF8String0);
}

GTEST_TEST_F(LocString, readLocString) {
	static const byte kData[] = { 0x17, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
	                              0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00,
	                              0x46, 0xC3, 0xB6, 0xC3, 0xB6, 0x62, 0xC3, 0xA4,
	                              0x72, 0x30, 0x01, 0x00, 0x00, 0x00, 0x0A, 0x00,
	                              0x00, 0x00, 0x46, 0xC3, 0xB6, 0xC3, 0xB6, 0x62,
	                              0xC3, 0xA4, 0x72, 0x31 };
	Common::MemoryReadStream stream(kData);

	Aurora::LocString locString;
	locString.readLocString(stream);

	EXPECT_EQ(locString.getID(), 23);

	EXPECT_TRUE(locString.hasString(Aurora::kLanguageEnglish, Aurora::kLanguageGenderMale  ));
	EXPECT_TRUE(locString.hasString(Aurora::kLanguageEnglish, Aurora::kLanguageGenderFemale));

	EXPECT_STREQ(locString.getString(Aurora::kLanguageEnglish, Aurora::kLanguageGenderMale  ).c_str(),
	             kUTF8String0);
	EXPECT_STREQ(locString.getString(Aurora::kLanguageEnglish, Aurora::kLanguageGenderFemale).c_str(),
	             kUTF8String1);

	EXPECT_STREQ(locString.getFirstString().c_str(), kUTF8String0);

	EXPECT_STREQ(locString.getString().c_str(), kUTF8String0);
}

GTEST_TEST_F(LocString, readLocStringSetGender) {
	static const byte kData[] = { 0x17, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
	                              0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00,
	                              0x46, 0xC3, 0xB6, 0xC3, 0xB6, 0x62, 0xC3, 0xA4,
	                              0x72, 0x30, 0x01, 0x00, 0x00, 0x00, 0x0A, 0x00,
	                              0x00, 0x00, 0x46, 0xC3, 0xB6, 0xC3, 0xB6, 0x62,
	                              0xC3, 0xA4, 0x72, 0x31 };
	Common::MemoryReadStream stream(kData);

	Aurora::LocString locString;
	locString.readLocString(stream);

	LangMan.setCurrentLanguage(Aurora::kLanguageEnglish);

	LangMan.setCurrentGender(Aurora::kLanguageGenderMale);
	EXPECT_STREQ(locString.getString().c_str(), kUTF8String0);

	LangMan.setCurrentGender(Aurora::kLanguageGenderFemale);
	EXPECT_STREQ(locString.getString().c_str(), kUTF8String1);
}

GTEST_TEST_F(LocString, readLocStringBroken) {
	static const byte kData[] = { 0x99, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00 };
	Common::MemoryReadStream stream(kData);

	Aurora::LocString locString;

	EXPECT_THROW(locString.readLocString(stream), Common::Exception);
}

GTEST_TEST_F(LocString, wrongEncodingUTF8) {
	static const byte kData[] = { 0x09, 0x00, 0x00, 0x00, 0x46, 0xFF, 0xB6, 0xC3,
	                              0xB6, 0x62, 0xC3, 0xA4, 0x72 };
	Common::MemoryReadStream stream(kData);

	Aurora::LocString locString;
	EXPECT_THROW(locString.readString(1, stream), Common::Exception);
}

GTEST_TEST_F(LocString, wrongEncodingNonUTF8) {
	if (!Common::hasSupportEncoding(Common::kEncodingCP1252)) {
		const ::testing::TestInfo* const testInfo = ::testing::UnitTest::GetInstance()->current_test_info();

		std::fprintf(stderr, "Skipping %s.%s\n", testInfo->test_case_name(),  testInfo->name());
		return;
	}

	static const byte kData[] = { 0x04, 0x00, 0x00, 0x00, 0x81, 0x81, 0x81, 0x81 };
	Common::MemoryReadStream stream(kData);

	Aurora::LocString locString;
	locString.readString(2, stream);

	EXPECT_STREQ(locString.getString().c_str(), "[!?!]");
}

GTEST_TEST_F(LocString, unknownEncodingUTF8) {
	static const byte kData[] = { 0x09, 0x00, 0x00, 0x00, 0x46, 0xC3, 0xB6, 0xC3,
	                              0xB6, 0x62, 0xC3, 0xA4, 0x72 };
	Common::MemoryReadStream stream(kData);

	Aurora::LocString locString;
	locString.readString(4, stream);

	EXPECT_STREQ(locString.getString().c_str(), "[???]");
}
