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
 *  Handling BioWare's localized strings.
 */

#ifndef AURORA_LOCSTRING_H
#define AURORA_LOCSTRING_H

#include <map>

#include "src/common/types.h"
#include "src/common/ustring.h"
#include "src/common/writestream.h"

#include "src/aurora/language.h"

namespace Common {
	class SeekableReadStream;
}

namespace Aurora {

/** A localized string. */
class LocString {
public:
	/* For "obscure" reasons, clang < 3.9.0 needs this to be a user-provided
	 * default constructor here, otherwise code that instantiates a default-
	 * constructed const LocString fails to compile.
	 *
	 * See https://stackoverflow.com/a/47368753 for details.
	 */
	LocString() { }
	~LocString() = default;

	void clear();

	/** Is this localized string empty, without any strings whatsoever? */
	bool empty() const;

	/** Swap the contents of the LocString with this LocString's. */
	void swap(LocString &str);

	/** Get the number of strings. */
	uint32_t getNumStrings() const;

	/** Return the string ID / StrRef. */
	uint32_t getID() const;
	/** Set the string ID / StrRef. */
	void setID(uint32_t id);

	/** Does the LocString have a string of this language? */
	bool hasString(Language language, LanguageGender gender = kLanguageGenderCurrent) const;

	/** Get the string of that language. */
	const Common::UString &getString(Language language, LanguageGender gender = kLanguageGenderCurrent) const;

	/** Set the string of that language. */
	void setString(Language language, LanguageGender gender, const Common::UString &str);
	/** Set the string of that language (for all genders). */
	void setString(Language language, const Common::UString &str);
	/** Set the string of that language (with a raw language ID). */
	void setStringRawLanguageID(uint32_t language, const Common::UString &str);

	/** Get the string the StrRef points to. */
	const Common::UString &getStrRefString() const;

	/** Get the first available string. */
	const Common::UString &getFirstString() const;

	/** Try to get the most appropriate string. */
	const Common::UString &getString() const;

	/** Read a string out of a stream. */
	void readString(uint32_t languageID, Common::SeekableReadStream &stream);
	/** Read a LocSubString (substring of a LocString in game data) out of a stream. */
	void readLocSubString(Common::SeekableReadStream &stream);
	/** Read a LocString out of a stream. */
	void readLocString(Common::SeekableReadStream &stream, uint32_t id, uint32_t count);
	/** Read a LocString out of a stream. */
	void readLocString(Common::SeekableReadStream &stream);

	/** Get the size, the string table will consume after being written. */
	uint32_t getWrittenSize(bool withNullTerminate = false) const;
	/** Write the LocString to a write stream. */
	void writeLocString(Common::WriteStream &stream, bool withNullTerminate = false) const;

	/** Equality operator for list search. */
	bool operator==(const LocString &rhs) const;
	/** Relational operator for map find. */
	bool operator<(const LocString &rhs) const;

private:
	typedef std::map<uint32_t, Common::UString> StringMap;

	uint32_t _id { kStrRefInvalid }; ///< The string's ID / StrRef.

	StringMap _strings;


	bool hasString(uint32_t languageID) const;

	const Common::UString &getString(uint32_t languageID) const;

	void setString(uint32_t languageID, const Common::UString &str);
};

} // End of namespace Aurora

#endif // AURORA_LOCSTRING_H
