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
 *  Unit tests for our LanguageManager.
 */

#include "gtest/gtest.h"

#include "src/common/util.h"
#include "src/common/ustring.h"
#include "src/common/memreadstream.h"

#include "src/aurora/language.h"

static const Aurora::LanguageManager::Declaration kLanguageDeclarations[] = {
	{ Aurora::kLanguageEnglish           ,   0, Common::kEncodingASCII  , Common::kEncodingCP1252 },
	{ Aurora::kLanguageFrench            ,   1, Common::kEncodingUTF16LE, Common::kEncodingCP1252 },
	{ Aurora::kLanguageGerman            ,   2, Common::kEncodingUTF16BE, Common::kEncodingCP1252 },
	{ Aurora::kLanguageItalian           ,   3, Common::kEncodingUTF8   , Common::kEncodingCP1252 },
	{ Aurora::kLanguageSpanish           ,   4, Common::kEncodingUTF8   , Common::kEncodingCP1252 },
	{ Aurora::kLanguagePolish            ,   5, Common::kEncodingUTF8   , Common::kEncodingCP1250 },
	{ Aurora::kLanguageKorean            , 128, Common::kEncodingCP949  , Common::kEncodingCP949  },
	{ Aurora::kLanguageChineseTraditional, 129, Common::kEncodingCP950  , Common::kEncodingCP950  },
	{ Aurora::kLanguageChineseSimplified , 130, Common::kEncodingCP936  , Common::kEncodingCP936  },
	{ Aurora::kLanguageJapanese          , 131, Common::kEncodingCP932  , Common::kEncodingCP932  }
};

static const char * const kLanguageNamesShort[] = {
	"en", "fr", "de", "it", "es", "pl", "ko", "zho_hk", "zho_cn", "jp"
};

static const char * const kLanguageNamesLong[] = {
	"English", "French", "German", "Italian", "Spanish", "Polish",
	"Korean", "Traditional Chinese", "Simplified Chinese", "Japanese"
};


class LanguageManager : public ::testing::Test {
protected:
	void SetUp() {
		LangMan.clear();
	}

	void TearDown() {
		Aurora::LanguageManager::destroy();
	}
};


GTEST_TEST_F(LanguageManager, addLanguage) {
	LangMan.addLanguage(kLanguageDeclarations[0].language, kLanguageDeclarations[0].id,
	                    kLanguageDeclarations[0].encoding);

	EXPECT_EQ(LangMan.getLanguage(kLanguageDeclarations[0].id), kLanguageDeclarations[0].language);
}

GTEST_TEST_F(LanguageManager, addLanguageEncodingLocString) {
	LangMan.addLanguage(kLanguageDeclarations[0].language, kLanguageDeclarations[0].id,
	                    kLanguageDeclarations[0].encoding, kLanguageDeclarations[0].encodingLocString);

	EXPECT_EQ(LangMan.getLanguage(kLanguageDeclarations[0].id), kLanguageDeclarations[0].language);
}

GTEST_TEST_F(LanguageManager, addLanguageDeclaration) {
	LangMan.addLanguage(kLanguageDeclarations[0]);

	EXPECT_EQ(LangMan.getLanguage(kLanguageDeclarations[0].id), kLanguageDeclarations[0].language);
}

GTEST_TEST_F(LanguageManager, addLanguages) {
	LangMan.addLanguages(kLanguageDeclarations, ARRAYSIZE(kLanguageDeclarations));

	for (size_t i = 0; i < ARRAYSIZE(kLanguageDeclarations); i++)
		EXPECT_EQ(LangMan.getLanguage(kLanguageDeclarations[i].id), kLanguageDeclarations[i].language) <<
			"At index " << i;
}

GTEST_TEST_F(LanguageManager, clear) {
	LangMan.addLanguages(kLanguageDeclarations, ARRAYSIZE(kLanguageDeclarations));
	LangMan.clear();

	for (size_t i = 0; i < ARRAYSIZE(kLanguageDeclarations); i++)
		EXPECT_EQ(LangMan.getLanguage(kLanguageDeclarations[i].id), Aurora::kLanguageInvalid) <<
			"At index " << i;
}

GTEST_TEST_F(LanguageManager, getLanguageIDUngendered) {
	LangMan.addLanguages(kLanguageDeclarations, ARRAYSIZE(kLanguageDeclarations));

	for (size_t i = 0; i < ARRAYSIZE(kLanguageDeclarations); i++)
		EXPECT_EQ(LangMan.getLanguageID(kLanguageDeclarations[i].language), kLanguageDeclarations[i].id) <<
			"At index " << i;
}

GTEST_TEST_F(LanguageManager, getLanguageIDGendered) {
	LangMan.addLanguages(kLanguageDeclarations, ARRAYSIZE(kLanguageDeclarations));

	for (size_t i = 0; i < ARRAYSIZE(kLanguageDeclarations); i++) {
		EXPECT_EQ(LangMan.getLanguageID(kLanguageDeclarations[i].language, Aurora::kLanguageGenderMale),
		          2 * kLanguageDeclarations[i].id + 0) << "At index " << i;
		EXPECT_EQ(LangMan.getLanguageID(kLanguageDeclarations[i].language, Aurora::kLanguageGenderFemale),
		          2 * kLanguageDeclarations[i].id + 1) << "At index " << i;
	}
}

GTEST_TEST_F(LanguageManager, getLanguageUngendered) {
	LangMan.addLanguages(kLanguageDeclarations, ARRAYSIZE(kLanguageDeclarations));

	for (size_t i = 0; i < ARRAYSIZE(kLanguageDeclarations); i++)
		EXPECT_EQ(LangMan.getLanguage(kLanguageDeclarations[i].id), kLanguageDeclarations[i].language) <<
			"At index " << i;
}

GTEST_TEST_F(LanguageManager, getLanguageGendered) {
	LangMan.addLanguages(kLanguageDeclarations, ARRAYSIZE(kLanguageDeclarations));

	for (size_t i = 0; i < ARRAYSIZE(kLanguageDeclarations); i++) {
		Aurora::LanguageGender gender0 = Aurora::kLanguageGenderMAX;
		EXPECT_EQ(LangMan.getLanguage(2 * kLanguageDeclarations[i].id + 0, gender0),
		          kLanguageDeclarations[i].language) << "At index " << i;

		EXPECT_EQ(gender0, Aurora::kLanguageGenderMale);

		Aurora::LanguageGender gender1 = Aurora::kLanguageGenderMAX;
		EXPECT_EQ(LangMan.getLanguage(2 * kLanguageDeclarations[i].id + 1, gender1),
		          kLanguageDeclarations[i].language) << "At index " << i;

		EXPECT_EQ(gender1, Aurora::kLanguageGenderFemale);
	}
}

GTEST_TEST_F(LanguageManager, getLanguageGenderedThrowaway) {
	LangMan.addLanguages(kLanguageDeclarations, ARRAYSIZE(kLanguageDeclarations));

	for (size_t i = 0; i < ARRAYSIZE(kLanguageDeclarations); i++) {
		EXPECT_EQ(LangMan.getLanguageGendered(2 * kLanguageDeclarations[i].id + 0),
		          kLanguageDeclarations[i].language) << "At index " << i;

		EXPECT_EQ(LangMan.getLanguageGendered(2 * kLanguageDeclarations[i].id + 1),
		          kLanguageDeclarations[i].language) << "At index " << i;
	}
}

GTEST_TEST_F(LanguageManager, getEncoding) {
	LangMan.addLanguages(kLanguageDeclarations, ARRAYSIZE(kLanguageDeclarations));

	for (size_t i = 0; i < ARRAYSIZE(kLanguageDeclarations); i++)
		EXPECT_EQ(LangMan.getEncoding(kLanguageDeclarations[i].language),
		          kLanguageDeclarations[i].encoding) << "At index " << i;
}

GTEST_TEST_F(LanguageManager, getEncodingLocString) {
	LangMan.addLanguages(kLanguageDeclarations, ARRAYSIZE(kLanguageDeclarations));

	for (size_t i = 0; i < ARRAYSIZE(kLanguageDeclarations); i++)
		EXPECT_EQ(LangMan.getEncodingLocString(kLanguageDeclarations[i].language),
		          kLanguageDeclarations[i].encodingLocString) << "At index " << i;
}

GTEST_TEST_F(LanguageManager, parseLanguage) {
	for (size_t i = 0; i < ARRAYSIZE(kLanguageDeclarations); i++)
		EXPECT_EQ(LangMan.parseLanguage(kLanguageNamesShort[i]), kLanguageDeclarations[i].language) <<
			"At index " << i;

	for (size_t i = 0; i < ARRAYSIZE(kLanguageDeclarations); i++)
		EXPECT_EQ(LangMan.parseLanguage(kLanguageNamesLong[i]), kLanguageDeclarations[i].language) <<
			"At index " << i;
}

GTEST_TEST_F(LanguageManager, convertLanguageIDToGendered) {
	EXPECT_EQ(LangMan.convertLanguageIDToGendered(23, Aurora::kLanguageGenderMale  ), 2 * 23 + 0);
	EXPECT_EQ(LangMan.convertLanguageIDToGendered(23, Aurora::kLanguageGenderFemale), 2 * 23 + 1);
}

GTEST_TEST_F(LanguageManager, convertLanguageIDToUngendered) {
	EXPECT_EQ(LangMan.convertLanguageIDToUngendered(2 * 23 + 0), 23);
	EXPECT_EQ(LangMan.convertLanguageIDToUngendered(2 * 23 + 1), 23);
}

GTEST_TEST_F(LanguageManager, getLanguageGender) {
	EXPECT_EQ(LangMan.getLanguageGender(2 * 23 + 0), Aurora::kLanguageGenderMale);
	EXPECT_EQ(LangMan.getLanguageGender(2 * 23 + 1), Aurora::kLanguageGenderFemale);
}

GTEST_TEST_F(LanguageManager, swapLanguageGender) {
	EXPECT_EQ(LangMan.swapLanguageGender(2 * 23 + 0), 2 * 23 + 1);
	EXPECT_EQ(LangMan.swapLanguageGender(2 * 23 + 1), 2 * 23 + 0);
}

GTEST_TEST_F(LanguageManager, setCurrentLanguageSingle) {
	LangMan.addLanguages(kLanguageDeclarations, ARRAYSIZE(kLanguageDeclarations));

	LangMan.setCurrentLanguage(kLanguageDeclarations[0].language);

	EXPECT_EQ(LangMan.getCurrentLanguageText() , kLanguageDeclarations[0].language);
	EXPECT_EQ(LangMan.getCurrentLanguageVoice(), kLanguageDeclarations[0].language);
}

GTEST_TEST_F(LanguageManager, setCurrentLanguageDouble) {
	LangMan.addLanguages(kLanguageDeclarations, ARRAYSIZE(kLanguageDeclarations));

	LangMan.setCurrentLanguage(kLanguageDeclarations[0].language);
	LangMan.setCurrentLanguage(kLanguageDeclarations[1].language, kLanguageDeclarations[2].language);

	EXPECT_EQ(LangMan.getCurrentLanguageText() , kLanguageDeclarations[1].language);
	EXPECT_EQ(LangMan.getCurrentLanguageVoice(), kLanguageDeclarations[2].language);
}

GTEST_TEST_F(LanguageManager, setCurrentLanguageText) {
	LangMan.addLanguages(kLanguageDeclarations, ARRAYSIZE(kLanguageDeclarations));

	LangMan.setCurrentLanguage    (kLanguageDeclarations[0].language);
	LangMan.setCurrentLanguageText(kLanguageDeclarations[1].language);

	EXPECT_EQ(LangMan.getCurrentLanguageText() , kLanguageDeclarations[1].language);
	EXPECT_EQ(LangMan.getCurrentLanguageVoice(), kLanguageDeclarations[0].language);
}

GTEST_TEST_F(LanguageManager, setCurrentLanguageVoice) {
	LangMan.addLanguages(kLanguageDeclarations, ARRAYSIZE(kLanguageDeclarations));

	LangMan.setCurrentLanguage     (kLanguageDeclarations[0].language);
	LangMan.setCurrentLanguageVoice(kLanguageDeclarations[1].language);

	EXPECT_EQ(LangMan.getCurrentLanguageText() , kLanguageDeclarations[0].language);
	EXPECT_EQ(LangMan.getCurrentLanguageVoice(), kLanguageDeclarations[1].language);
}

GTEST_TEST_F(LanguageManager, setCurrentGender) {
	LangMan.setCurrentGender(Aurora::kLanguageGenderMale);
	EXPECT_EQ(LangMan.getCurrentGender(), Aurora::kLanguageGenderMale);

	LangMan.setCurrentGender(Aurora::kLanguageGenderFemale);
	EXPECT_EQ(LangMan.getCurrentGender(), Aurora::kLanguageGenderFemale);
}

GTEST_TEST_F(LanguageManager, getCurrentEncoding) {
	LangMan.addLanguages(kLanguageDeclarations, ARRAYSIZE(kLanguageDeclarations));

	for (size_t i = 0; i < ARRAYSIZE(kLanguageDeclarations); i++) {
		LangMan.setCurrentLanguage(kLanguageDeclarations[i].language);

		EXPECT_EQ(LangMan.getCurrentEncoding(), kLanguageDeclarations[i].encoding) <<
			"At index " << i;
	}
}

GTEST_TEST_F(LanguageManager, getCurrentEncodingLocString) {
	LangMan.addLanguages(kLanguageDeclarations, ARRAYSIZE(kLanguageDeclarations));

	for (size_t i = 0; i < ARRAYSIZE(kLanguageDeclarations); i++) {
		LangMan.setCurrentLanguage(kLanguageDeclarations[i].language);

		EXPECT_EQ(LangMan.getCurrentEncodingLocString(), kLanguageDeclarations[i].encodingLocString) <<
			"At index " << i;
	}
}

GTEST_TEST_F(LanguageManager, Unknown) {
	EXPECT_EQ(LangMan.getLanguageID(Aurora::kLanguageEnglish), Aurora::kLanguageInvalid);
	EXPECT_EQ(LangMan.getLanguageID(Aurora::kLanguageEnglish,
	                                Aurora::kLanguageGenderMale), Aurora::kLanguageInvalid);

	EXPECT_EQ(LangMan.getLanguage(0), Aurora::kLanguageInvalid);
	EXPECT_EQ(LangMan.getLanguage(0), Aurora::kLanguageInvalid);

	Aurora::LanguageGender gender = Aurora::kLanguageGenderMAX;
	EXPECT_EQ(LangMan.getLanguage(0, gender), Aurora::kLanguageInvalid);

	EXPECT_EQ(LangMan.getLanguageGendered(0), Aurora::kLanguageInvalid);

	EXPECT_EQ(LangMan.getEncoding(Aurora::kLanguageEnglish), Common::kEncodingInvalid);
	EXPECT_EQ(LangMan.getEncodingLocString(Aurora::kLanguageEnglish), Common::kEncodingInvalid);

	EXPECT_EQ(LangMan.getCurrentLanguageText() , Aurora::kLanguageInvalid);
	EXPECT_EQ(LangMan.getCurrentLanguageVoice(), Aurora::kLanguageInvalid);

	EXPECT_EQ(LangMan.getCurrentEncoding(), Common::kEncodingInvalid);
	EXPECT_EQ(LangMan.getCurrentEncodingLocString(), Common::kEncodingInvalid);
}

GTEST_TEST_F(LanguageManager, preParseColorCodes) {
	static const byte kColorString    [] = "<c""\xAB""\xCD""\xEF"">Foobar</c>";
	static const byte kPreparsedString[] = "<cABCDEFFF>Foobar</c>";

	Common::MemoryReadStream stream(kColorString);

	Common::MemoryReadStream *preparsed = LangMan.preParseColorCodes(stream);
	ASSERT_NE(preparsed, static_cast<Common::MemoryReadStream *>(0));
	ASSERT_EQ(preparsed->size(), sizeof(kPreparsedString));

	for (size_t i = 0; i < sizeof(kPreparsedString); i++)
		EXPECT_EQ(preparsed->readChar(), kPreparsedString[i]) << "At index " << i;

	delete preparsed;
}
