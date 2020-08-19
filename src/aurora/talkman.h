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

#include <list>

#include "src/common/types.h"
#include "src/common/ustring.h"
#include "src/common/singleton.h"
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

	/** Add a talk table to the talk manager.
	 *
	 *  @param nameMale   Resource name of the male version.
	 *  @param nameFemale Resource name of the female version.
	 *  @param isAlt      Is this an "alternate" talk table instead of a main one?
	 *  @param priority   The priority this talk table has over other tables.
	 *  @param changeID   If given, record the collective changes done here.
	 */
	void addTable(const Common::UString &nameMale, const Common::UString &nameFemale,
                bool isAlt, uint32_t priority, Common::ChangeID *changeID = 0);

	/** Remove a talk table from the talk manager again. */
	void removeTable(Common::ChangeID &changeID);

	const Common::UString &getString     (uint32_t strRef, LanguageGender gender = kLanguageGenderCurrent);
	const Common::UString &getSoundResRef(uint32_t strRef, LanguageGender gender = kLanguageGenderCurrent);

private:
	struct Table {
		uint32_t id;
		uint32_t priority;

		TalkTable *tableMale;
		TalkTable *tableFemale;

		Table(TalkTable *tM, TalkTable *tF, uint32_t p, uint32_t i) :
			id(i), priority(p), tableMale(tM), tableFemale(tF) { }

		bool operator<(const Table &right) const { return priority < right.priority; }
	};

	class Change : public Common::ChangeContent {
	private:
		Change(uint32_t id, bool isAlt) : _id(id), _isAlt(isAlt) { }

		uint32_t _id;
		bool _isAlt;

		friend class TalkManager;

	public:
		~Change() { }

		Common::ChangeContent *clone() const { return new Change(_id, _isAlt); }
	};

	typedef std::list<Table> Tables;


	Tables _tablesMain;
	Tables _tablesAlt;


	void deleteTable(Table &table);

	const TalkTable *find(uint32_t strRef, LanguageGender gender) const;
	const TalkTable *find(const Tables &tables, uint32_t strRef, LanguageGender gender) const;
};

} // End of namespace Aurora

/** Shortcut for accessing the talk manager. */
#define TalkMan ::Aurora::TalkManager::instance()

#endif // AURORA_TALKMAN_H
