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

#include <memory>

#include "src/common/error.h"
#include "src/common/ustring.h"
#include "src/common/readstream.h"
#include "src/common/uuid.h"
#include "src/common/encoding.h"

#include "src/aurora/talkman.h"
#include "src/aurora/resman.h"
#include "src/aurora/talktable.h"

DECLARE_SINGLETON(Aurora::TalkManager)

namespace Aurora {

TalkManager::TalkManager() {
}

TalkManager::~TalkManager() {
	clear();
}

void TalkManager::clear() {
	for (Tables::iterator t = _tablesMain.begin(); t != _tablesMain.end(); ++t)
		deleteTable(*t);
	for (Tables::iterator t = _tablesAlt.begin(); t != _tablesAlt.end(); ++t)
		deleteTable(*t);

	_tablesMain.clear();
	_tablesAlt.clear();
}

static TalkTable *loadTable(const Common::UString &name, Common::Encoding encoding) {
	if (name.empty())
		return 0;

	Common::SeekableReadStream *tlk = ResMan.getResource(name, kFileTypeTLK);
	if (!tlk)
		return 0;

	return TalkTable::load(tlk, encoding);
}

static void loadTables(const Common::UString &nameM, const Common::UString &nameF,
                       TalkTable *&tableM, TalkTable *&tableF, Common::Encoding encoding) {

	std::unique_ptr<TalkTable> m(loadTable(nameM, encoding));
	std::unique_ptr<TalkTable> f(loadTable(nameF, encoding));

	tableM = m.release();
	tableF = f.release();
}

void TalkManager::addTable(const Common::UString &nameMale, const Common::UString &nameFemale,
                           bool isAlt, uint32_t priority, Common::ChangeID *changeID) {

	TalkTable *tableMale = 0, *tableFemale = 0;
	loadTables(nameMale, nameFemale, tableMale, tableFemale, LangMan.getCurrentEncoding());

	if (!tableMale && !tableFemale)
		throw Common::Exception("No such talk table \"%s\"/\"%s\"", nameMale.c_str(), nameFemale.c_str());

	Tables *tables = &_tablesMain;
	if (isAlt)
		tables = &_tablesAlt;

	const uint32_t id = Common::generateIDNumber();

	tables->push_back(Table(tableMale, tableFemale, priority, id));
	tables->sort();

	if (changeID)
		changeID->setContent(new Change(id, isAlt));
}

void TalkManager::deleteTable(Table &table) {
	delete table.tableMale;
	delete table.tableFemale;

	table.tableMale   = 0;
	table.tableFemale = 0;
}

void TalkManager::removeTable(Common::ChangeID &changeID) {
	Change *change = dynamic_cast<Change *>(changeID.getContent());
	if (!change)
		return;

	Tables *tables = &_tablesMain;
	if (change->_isAlt)
		tables = &_tablesAlt;

	for (Tables::iterator t = tables->begin(); t != tables->end(); ++t) {
		if (t->id == change->_id) {
			deleteTable(*t);

			tables->erase(t);
			break;
		}
	}

	changeID.clear();
}

static const Common::UString kEmptyString = "";
const Common::UString &TalkManager::getString(uint32_t strRef, LanguageGender gender) {
	if (gender == kLanguageGenderCurrent)
		gender = LangMan.getCurrentGender();

	if (strRef == kStrRefInvalid)
		return kEmptyString;

	const TalkTable *table = find(strRef, gender);
	if (!table)
		return kEmptyString;

	return table->getString(strRef);
}

const Common::UString &TalkManager::getSoundResRef(uint32_t strRef, LanguageGender gender) {
	if (gender == kLanguageGenderCurrent)
		gender = LangMan.getCurrentGender();

	if (strRef == kStrRefInvalid)
		return kEmptyString;

	const TalkTable *table = find(strRef, gender);
	if (!table)
		return kEmptyString;

	return table->getSoundResRef(strRef);
}

const TalkTable *TalkManager::find(const Tables &tables, uint32_t strRef, LanguageGender gender) const {
	/* Look for the strRef in decreasing priority.
	 *
	 * Only look through the female table if a female gendered string was requested,
	 * but fall back on the male table in either case.
	 */

	for (Tables::const_reverse_iterator t = tables.rbegin(); t != tables.rend(); ++t) {
		if (gender == kLanguageGenderFemale)
			if (t->tableFemale && t->tableFemale->hasEntry(strRef))
				return t->tableFemale;

		if (t->tableMale && t->tableMale->hasEntry(strRef))
			return t->tableMale;
	}

	return 0;
}

const TalkTable *TalkManager::find(uint32_t strRef, LanguageGender gender) const {
	bool isAlt = (strRef & 0xFF000000) != 0;

	strRef &= 0x00FFFFFF;

	const TalkTable *table = 0;

	/* If the flag for "use alternate tables" is set, search through those.
	 * If the flag is not set, or the strRef was not found, search in the main tables.
	 */

	if (isAlt)
		if ((table = find(_tablesAlt, strRef & 0x00FFFFFF, gender)) != 0)
			return table;

	// If the flag is not set, or the strRef was not found, look in the main tables
	if ((table = find(_tablesMain, strRef, gender)) != 0)
		return table;

	return 0;
}

} // End of namespace Aurora
