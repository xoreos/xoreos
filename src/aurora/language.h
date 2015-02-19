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

/** @file aurora/language.h
 *  Types and functions related to language.
 */

#ifndef AURORA_LANGUAGE_H
#define AURORA_LANGUAGE_H

#include "aurora/types.h"

namespace Common {
	class UString;
}

namespace Aurora {

enum Language {
	kLanguageEnglish,
	kLanguageFrench,
	kLanguageGerman,
	kLanguageItalian,
	kLanguageSpanish,
	kLanguagePolish,
	kLanguageCzech,
	kLanguageHungarian,
	kLanguageRussian,
	kLanguageKorean,
	kLanguageChineseTraditional,
	kLanguageChineseSimplified,
	kLanguageJapanese,

	kLanguageMAX,
	kLanguageInvalid = 0xFFFFFFFF
};

enum LanguageGender {
	kLanguageGenderMale   = 0,
	kLanguageGenderFemale = 1
};

/** Return the human readable name of a language. */
Common::UString getLanguageName(Language language);


// Language IDs

/** Construct the internal language ID for an ungendered use of a language.
 *
 *  This is used by Aurora games in contexts where the gender of a player
 *  character is not relevant, for example when querying or setting the
 *  language the whole game runs.
 */
uint32 getLanguageID(GameID game, Language language);

/** Construct the internal language ID for an gendered use of a language.
 *
 *  This is used by Aurora games in contexts where the gender of a player
 *  character is relevant, for example when displaying dialogue strings
 *  that might change depending on whether the player character is male
 *  or female.
 */
uint32 getLanguageID(GameID game, Language language, LanguageGender gender);

/** Decode the internal language ID for an ungendered use of a language.
 *
 *  See getLanguageID() on what constitutes gendered and ungendered
 *  language use.
 */
Language getLanguage(GameID game, uint32 languageID);

/** Decode the internal language ID for a gendered use of a language.
 *
 *  See getLanguageID() on what constitutes gendered and ungendered
 *  language use.
 */
Language getLanguage(GameID game, uint32 languageID, LanguageGender &gender);

/** Convert an ungendered language ID to a gendered language ID. */
uint32 convertLanguageIDToGendered(uint32 languageID, LanguageGender gender);
/** Convert a gendered language ID to an ungendered language ID. */
uint32 convertLanguageIDToUngendered(uint32 languageID);

/** Return the gender of this gendered language ID. */
LanguageGender getLanguageGender(uint32 languageID);

/** Swap the gender of this gendered language ID. */
uint32 swapLanguageGender(uint32 languageID);

} // End of namespace Aurora

#endif // AURORA_LANGUAGE_H
