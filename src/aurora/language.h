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
 *  Types and functions related to language.
 */

#ifndef AURORA_LANGUAGE_H
#define AURORA_LANGUAGE_H

#include <map>

#include "src/common/types.h"
#include "src/common/singleton.h"
#include "src/common/encoding.h"

#include "src/aurora/types.h"

namespace Common {
	class UString;

	class SeekableReadStream;
	class MemoryReadStream;
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
	kLanguageDebug   = 0xFFFFFFFD, ///< Pseudo value for debug strings.
	kLanguageChinese = 0xFFFFFFFE, ///< Pseudo value that means either traditional or simplified Chinese.
	kLanguageInvalid = 0xFFFFFFFF
};

enum LanguageGender {
	kLanguageGenderMale   = 0,
	kLanguageGenderFemale = 1,

	kLanguageGenderMAX,
	kLanguageGenderCurrent = 0xFF ///< Pseudo value that means the current language gender.
};

/** The global language manager.
 *
 *  The language manager holds all known supported languages of a game, as well
 *  as the currently selected language(s) and gender (see below for details).
 *
 *  A language consists of these parts:
 *  - A value of the Language type, describing what exact language this is.
 *  - A numerical language ID by which the language is referenced in game data
 *    files. This is the ungendered version of the language ID (see below for
 *    details).
 *  - The encoding to use when reading or writing text strings of this language.
 *  - The encoding to use when reading or writing LocStrings of this language.
 *    This can, but does not have to, be the same encoding as other strings of
 *    this language.
 *
 *  Since the encoding can be different when reading LocStrings, two different
 *  functions exist to query the encoding of language strings: one for
 *  LocStrings and one for other string.s
 *
 *  Depending on the game, two different languages might be selected at the
 *  same time, differentiating between use: the current language for text and
 *  the current language for speech. Therefore, there are also two functions to
 *  query the current language: one for the current text language, one for the
 *  current speech language.
 *
 *  A language ID exists in two variants: gendered and ungendered. Some data
 *  files specify ungendered language IDs, some gendered. A gendered language
 *  ID is used to describe strings that may exist in two versions: one for
 *  male player characters and one for female player characters. The ungendered
 *  language ID used when describing a language as such.
 *
 *  The ungendered language IDs of a game are usually a simple running index.
 *  The gendered variant of an ungendered language ID x is 2 * x + 0 for male
 *  and 2 * x + 1 for female. The LanguageManager has functions to convert
 *  between gendered language IDs and ungendered language IDs plus Gender value.
 *
 *  Likewise, the LanguageManager holds the value of the current gender of
 *  the player character.
 */
class LanguageManager : public Common::Singleton<LanguageManager> {
public:
	struct Declaration {
		Language language;

		uint32 id;
		Common::Encoding encoding;
		Common::Encoding encodingLocString;
	};

	LanguageManager();
	~LanguageManager();

	// .--- Managed language methods
	/** Clear all managed languages from the LanguageManager. */
	void clear();

	/** Add a supported language for the current game to the LanguageManager,
	 *  together with its internal (ungendered) language ID and usual encoding. */
	void addLanguage(Language language, uint32 id, Common::Encoding encoding);
	/** Add a supported language for the current game to the LanguageManager,
	 *  together with its internal (ungendered) language ID, the usual encoding,
	 *  and the encoding when reading an embedded LocString string. */
	void addLanguage(Language language, uint32 id, Common::Encoding encoding,
	                 Common::Encoding encodingLocString);
	/** Add a supported language for the current game to the LanguageManager,
	 *  together with its internal (ungendered) language ID and usual encoding. */
	void addLanguage(const Declaration &languageDeclaration);
	/** Add several supported language for the current game to the LanguageManager,
	 *  together with their internal (ungendered) language ID and usual encoding. */
	void addLanguages(const Declaration *languageDeclarations, size_t count);

	/** Construct the internal language ID for an ungendered use of a language.
	 *
	 *  This is used by Aurora games in contexts where the gender of a player
	 *  character is not relevant, for example when querying or setting the
	 *  language the whole game runs.
	 */
	uint32 getLanguageID(Language language) const;

	/** Construct the internal language ID for an gendered use of a language.
	 *
	 *  This is used by Aurora games in contexts where the gender of a player
	 *  character is relevant, for example when displaying dialogue strings
	 *  that might change depending on whether the player character is male
	 *  or female.
	 */
	uint32 getLanguageID(Language language, LanguageGender gender) const;

	/** Decode the internal language ID for an ungendered use of a language.
	 *
	 *  See getLanguageID() on what constitutes gendered and ungendered
	 *  language use.
	 */
	Language getLanguage(uint32 languageID) const;

	/** Decode the internal language ID for a gendered use of a language.
	 *
	 *  See getLanguageID() on what constitutes gendered and ungendered
	 *  language use.
	 */
	Language getLanguage(uint32 languageID, LanguageGender &gender) const;

	/** Decode the internal language ID for a gendered use of a language
	 *  (and ignore the language gender).
	 *
	 *  See getLanguageID() on what constitutes gendered and ungendered
	 *  language use.
	 */
	Language getLanguageGendered(uint32 languageID) const;

	/** Return the encoding used for the given language. */
	Common::Encoding getEncoding(Language language) const;
	/** Return the encoding used for the given language, for reading an embedded LocString string. */
	Common::Encoding getEncodingLocString(Language language) const;
	// '---

	// .--- Current language
	/** Change the current language, setting both text and voice to the same language. */
	void setCurrentLanguage(Language language);
	/** Change the current language, separately for text and voice. */
	void setCurrentLanguage(Language languageText, Language languageVoice);
	/** Change the current language, for text only. */
	void setCurrentLanguageText(Language language);
	/** Change the current language, for voice only. */
	void setCurrentLanguageVoice(Language language);
	/** Change the gender modulating the current language. */
	void setCurrentGender(LanguageGender gender);

	/** Return the current language for text. */
	Language getCurrentLanguageText() const;
	/** Return the current language for voices. */
	Language getCurrentLanguageVoice() const;
	/** Return the gender modulating the current language. */
	LanguageGender getCurrentGender() const;

	/** Return the encoding for the current text language. */
	Common::Encoding getCurrentEncoding() const;
	/** Return the encoding for the current text language, for reading an embedded LocString string. */
	Common::Encoding getCurrentEncodingLocString() const;
	// '---

	// .--- Static language utility methods
	/** Return the human readable name of a language. */
	static Common::UString getLanguageName(Language language);

	/** Parse this string into a language. */
	static Language parseLanguage(const Common::UString &str);

	/** Convert an ungendered language ID to a gendered language ID. */
	static uint32 convertLanguageIDToGendered(uint32 languageID, LanguageGender gender);
	/** Convert a gendered language ID to an ungendered language ID. */
	static uint32 convertLanguageIDToUngendered(uint32 languageID);

	/** Return the gender of this gendered language ID. */
	static LanguageGender getLanguageGender(uint32 languageID);

	/** Swap the gender of this gendered language ID. */
	static uint32 swapLanguageGender(uint32 languageID);

	/** Pre-parse and fix color codes found in UI and dialogue strings in Aurora games.
	 *
	 *  Aurora games (or Neverwinter Nights at least) allow for color codes of the
	 *  form "<c???>" (and closing with "</c>"). The ??? are direct byte values taken
	 *  as red, green and blue values, breaking the usual text encoding.
	 *
	 *  To fix that, we pre-parse those strings, converting "<c???>" into "<cXXXXXX>",
	 *  where XXXXXX are the byte values in hexadecimal notation.
	 *
	 *  Note that this function will only work for encodings where ASCII values are
	 *  directly encoded as ASCII. Namely, ASCII, Latin9, UTF-8, Windows CP-1250,
	 *  Windows CP-1252 and many of the variable-byte Windows codepages used for
	 *  Asian languages. This function will *not* work for UTF-16 and UTF-32.
	 *
	 *  Also note that we're hoping that "<c???>" won't collide with valid characters
	 *  inside multibyte sequences. This is reasonable likely to never happen.
	 */
	static Common::MemoryReadStream *preParseColorCodes(Common::SeekableReadStream &stream);
	// '---

private:
	typedef std::map<uint32  , Declaration> LanguageByID;
	typedef std::map<Language, Declaration> LanguageByLanguage;

	LanguageByID _langByID;
	LanguageByLanguage _langByLang;

	Language _currentLanguageText;
	Language _currentLanguageVoice;
	LanguageGender _currentGender;

	const Declaration *find(Language language) const;
	const Declaration *find(uint32 id) const;
};

} // End of namespace Aurora

/** Shortcut for accessing the language manager. */
#define LangMan ::Aurora::LanguageManager::instance()

#endif // AURORA_LANGUAGE_H
