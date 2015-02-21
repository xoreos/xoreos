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
 *  The global talk manager for Aurora strings.
 */

#ifndef AURORA_TALKMAN_H
#define AURORA_TALKMAN_H

#include <map>

#include "src/common/types.h"
#include "src/common/singleton.h"
#include "src/common/encoding.h"

#include "src/aurora/language.h"
#include "src/aurora/talktable.h"

namespace Common {
	class UString;
}

namespace Aurora {

/** The global Aurora talk manager, holding the current talk tables. */
class TalkManager : public Common::Singleton<TalkManager> {
public:
	TalkManager();
	~TalkManager();

	void clear();

	/** Use this encoding when reading strings in this (ungendered) language ID. */
	void registerEncoding(uint32 languageID, Common::Encoding encoding);
	/** Return the encoding to use when reading strings in this (ungendered) language ID. */
	Common::Encoding getEncoding(uint32 languageID) const;

	/** Return the language ID (ungendered) of the main talk table. */
	uint32 getMainLanguageID() const;
	/** Return the gender modulating the current language. */
	LanguageGender getGender() const;

	/** Set the gender modulating the current language. */
	void setGender(LanguageGender gender);

	void addMainTable(const Common::UString &name);
	void addAltTable(const Common::UString &name);

	void removeMainTable();
	void removeAltTable();

	const Common::UString &getString(uint32 strRef, LanguageGender gender = (LanguageGender) -1);
	const Common::UString &getSoundResRef(uint32 strRef, LanguageGender gender = (LanguageGender) -1);

private:
	typedef std::map<uint32, Common::Encoding> EncodingMap;


	LanguageGender _gender;

	TalkTable *_mainTableM;
	TalkTable *_mainTableF;

	TalkTable *_altTableM;
	TalkTable *_altTableF;

	EncodingMap _encodings;


	const TalkTable::Entry *getEntry(uint32 strRef, LanguageGender gender);

	void addTable(const Common::UString &name, TalkTable *&m, TalkTable *&f);
};

} // End of namespace Aurora

/** Shortcut for accessing the talk manager. */
#define TalkMan ::Aurora::TalkManager::instance()

#endif // AURORA_TALKMAN_H
