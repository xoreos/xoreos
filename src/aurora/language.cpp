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

/** @file aurora/language.cpp
 *  Types and functions related to language.
 */

#include "common/ustring.h"

#include "aurora/language.h"

namespace Aurora {

uint32 getLanguageID(GameID game, Language language) {
	switch (game) {
		// The Witcher supports different languages than other Aurora games
		case kGameIDWitcher:
			switch (language) {
				case kLanguageEnglish:
					return  3;
				case kLanguageFrench:
					return 11;
				case kLanguageGerman:
					return 10;
				case kLanguageItalian:
					return 13;
				case kLanguageSpanish:
					return 12;
				case kLanguagePolish:
					return  5;
				case kLanguageCzech:
					return 15;
				case kLanguageHungarian:
					return 16;
				case kLanguageRussian:
					return 14;
				case kLanguageKorean:
					return 20;
				case kLanguageChineseTraditional:
					return 21;
				case kLanguageChineseSimplified:
					return 22;
				default:
					break;
			}
			break;

		default:
			switch (language) {
				case kLanguageEnglish:
					return   0;
				case kLanguageFrench:
					return   1;
				case kLanguageGerman:
					return   2;
				case kLanguageItalian:
					return   3;
				case kLanguageSpanish:
					return   4;
				case kLanguagePolish:
					return   5;
				case kLanguageKorean:
					return 128;
				case kLanguageChineseTraditional:
					return 129;
				case kLanguageChineseSimplified:
					return 130;
				case kLanguageJapanese:
					return 131;
				default:
					break;
			}
		break;
	}

	return 0;
}

uint32 getLanguageID(GameID game, Language language, LanguageGender gender) {
	return convertLanguageIDToGendered(getLanguageID(game, language), gender);
}

Language getLanguage(GameID game, uint32 languageID) {
	switch (game) {
		// The Witcher supports different languages than other Aurora games
		case kGameIDWitcher:
			switch (languageID) {
				case  3:
					return kLanguageEnglish;
				case  5:
					return kLanguagePolish;
				case 10:
					return kLanguageGerman;
				case 11:
					return kLanguageFrench;
				case 12:
					return kLanguageSpanish;
				case 13:
					return kLanguageItalian;
				case 14:
					return kLanguageRussian;
				case 15:
					return kLanguageCzech;
				case 16:
					return kLanguageHungarian;
				case 20:
					return kLanguageKorean;
				case 21:
					return kLanguageChineseTraditional;
				case 22:
					return kLanguageChineseSimplified;
				default:
					break;
			}
			break;

		default:
			switch (languageID) {
				case   0:
					return kLanguageEnglish;
				case   1:
					return kLanguageFrench;
				case   2:
					return kLanguageGerman;
				case   3:
					return kLanguageItalian;
				case   4:
					return kLanguageSpanish;
				case   5:
					return kLanguagePolish;
				case 128:
					return kLanguageKorean;
				case 129:
					return kLanguageChineseTraditional;
				case 130:
					return kLanguageChineseSimplified;
				case 131:
					return kLanguageJapanese;
				default:
					break;
			}
		break;
	}

	return kLanguageInvalid;
}

Language getLanguage(GameID game, uint32 languageID, LanguageGender &gender) {
	gender = getLanguageGender(languageID);

	return getLanguage(game, convertLanguageIDToUngendered(languageID));
}

uint32 convertLanguageIDToGendered(uint32 languageID, LanguageGender gender) {
	// In gendered language use, the ID is:
	// - ID * 2 + 0  for male
	// - ID * 2 + 1  for female

	return languageID * 2 + ((uint) gender);
}

uint32 convertLanguageIDToUngendered(uint32 languageID) {
	return languageID / 2;
}

LanguageGender getLanguageGender(uint32 languageID) {
	return (LanguageGender) (languageID % 2);
}

uint32 swapLanguageGender(uint32 languageID) {
	return languageID ^ 1;
}

Common::UString getLanguageName(Language language) {
	if (((uint32) language >= kLanguageMAX))
		return "Invalid";

	static const char *names[] = {
		"English", "French", "German", "Italian", "Spanish", "Polish", "Czech", "Hungarian",
		"Russian", "Korean", "Traditional Chinese", "Simplified Chinese", "Japanese"
	};

	return names[language];
}

} // End of namespace Aurora
