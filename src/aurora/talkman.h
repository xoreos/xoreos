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
#include "src/common/ustring.h"
#include "src/common/singleton.h"
#include "src/common/encoding.h"
#include "src/common/changeid.h"

#include "src/aurora/language.h"

namespace Aurora {

class TalkTable;

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

	/** Return the current language. */
	Language getLanguage() const;
	/** Return the current (ungendered) language ID. */
	uint32 getLanguageID() const;
	/** Return the gender modulating the current language. */
	LanguageGender getGender() const;

	/** Set the current language together with its (ungendered) language ID. */
	void setLanguage(Language language, uint32 languageID);
	/** Set the gender modulating the current language. */
	void setGender(LanguageGender gender);

	/** Add a talk table to the talk manager.
	 *
	 *  @param nameMale   Resource name of the male version.
	 *  @param nameFemale Resource name of the female version.
	 *  @param isAlt      Is this an "alternate" talk table instead of a main one?
	 *  @param priority   The priority this talk table has over other tables.
	 *  @param changeID   If given, record the collective changes done here.
	 */
	void addTable(const Common::UString &nameMale, const Common::UString &nameFemale,
                bool isAlt, uint32 priority, Common::ChangeID *changeID = 0);

	/** Remove a talk table from the talk manager again. */
	void removeTable(Common::ChangeID &changeID);

	const Common::UString &getString     (uint32 strRef, LanguageGender gender = (LanguageGender) -1);
	const Common::UString &getSoundResRef(uint32 strRef, LanguageGender gender = (LanguageGender) -1);

private:
	struct Table {
		uint32 id;
		uint32 priority;

		TalkTable *tableMale;
		TalkTable *tableFemale;

		Table(TalkTable *tM, TalkTable *tF, uint32 p, uint32 i) :
			id(i), priority(p), tableMale(tM), tableFemale(tF) { }

		bool operator<(const Table &right) const { return priority < right.priority; }
	};

	class Change : public Common::ChangeContent {
	private:
		Change(uint32 id, bool isAlt) : _id(id), _isAlt(isAlt) { }

		uint32 _id;
		bool _isAlt;

		friend class TalkManager;

	public:
		~Change() { }

		Common::ChangeContent *clone() const { return new Change(_id, _isAlt); }
	};

	typedef std::list<Table> Tables;
	typedef std::map<uint32, Common::Encoding> EncodingMap;


	EncodingMap _encodings;

	Language       _language;
	uint32         _languageID;
	LanguageGender _gender;

	Tables _tablesMain;
	Tables _tablesAlt;


	void deleteTable(Table &table);

	const TalkTable *find(uint32 strRef, LanguageGender gender) const;
	const TalkTable *find(const Tables &tables, uint32 strRef, LanguageGender gender) const;
};

} // End of namespace Aurora

/** Shortcut for accessing the talk manager. */
#define TalkMan ::Aurora::TalkManager::instance()

#endif // AURORA_TALKMAN_H
