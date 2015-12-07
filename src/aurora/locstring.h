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

#include "src/aurora/language.h"

namespace Common {
	class SeekableReadStream;
}

namespace Aurora {

/** A localized string. */
class LocString {
public:
	LocString();
	~LocString();

	void clear();

	/** Is this localized string empty, without any strings whatsoever? */
	bool empty() const;

	/** Swap the contents of the LocString with this LocString's. */
	void swap(LocString &str);

	/** Return the string ID / StrRef. */
	uint32 getID() const;
	/** Set the string ID / StrRef. */
	void setID(uint32 id);

	/** Does the LocString have a string of this language? */
	bool hasString(Language language, LanguageGender gender = kLanguageGenderCurrent) const;

	/** Get the string of that language. */
	const Common::UString &getString(Language language, LanguageGender gender = kLanguageGenderCurrent) const;

	/** Set the string of that language. */
	void setString(Language language, LanguageGender gender, const Common::UString &str);
	/** Set the string of that language (for all genders). */
	void setString(Language language, const Common::UString &str);

	/** Get the string the StrRef points to. */
	const Common::UString &getStrRefString() const;

	/** Get the first available string. */
	const Common::UString &getFirstString() const;

	/** Try to get the most appropriate string. */
	const Common::UString &getString() const;

	/** Read a string out of a stream. */
	void readString(uint32 languageID, Common::SeekableReadStream &stream);
	/** Read a LocSubString (substring of a LocString in game data) out of a stream. */
	void readLocSubString(Common::SeekableReadStream &stream);
	/** Read a LocString out of a stream. */
	void readLocString(Common::SeekableReadStream &stream, uint32 id, uint32 count);
	/** Read a LocString out of a stream. */
	void readLocString(Common::SeekableReadStream &stream);

private:
	typedef std::map<uint32, Common::UString> StringMap;

	uint32 _id; ///< The string's ID / StrRef.

	StringMap _strings;


	bool hasString(uint32 languageID) const;

	const Common::UString &getString(uint32 languageID) const;

	void setString(uint32 languageID, const Common::UString &str);
};

} // End of namespace Aurora

#endif // AURORA_LOCSTRING_H
