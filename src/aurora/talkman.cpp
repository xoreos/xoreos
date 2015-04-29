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

#include "src/aurora/talkman.h"
#include "src/aurora/resman.h"

#include "src/common/error.h"
#include "src/common/ustring.h"
#include "src/common/stream.h"

DECLARE_SINGLETON(Aurora::TalkManager)

namespace Aurora {

TalkManager::TalkManager() : _gender(kLanguageGenderMale),
	_mainTableM(0), _mainTableF(0), _altTableM(0), _altTableF(0) {

}

TalkManager::~TalkManager() {
	clear();
}

void TalkManager::clear() {
	removeMainTable();
	removeAltTable();

	_encodings.clear();
}

void TalkManager::registerEncoding(uint32 languageID, Common::Encoding encoding) {
	_encodings[languageID] = encoding;
}

Common::Encoding TalkManager::getEncoding(uint32 languageID) const {
	EncodingMap::const_iterator e = _encodings.find(languageID);
	if (e == _encodings.end())
		return Common::kEncodingUTF8;

	return e->second;
}

uint32 TalkManager::getMainLanguageID() const {
	if (_mainTableM)
		return _mainTableM->getLanguageID();
	if (_mainTableF)
		return _mainTableF->getLanguageID();

	return (uint32) kLanguageInvalid;
}

void TalkManager::setGender(LanguageGender gender) {
	_gender = gender;
}

LanguageGender TalkManager::getGender() const {
	return _gender;
}

void TalkManager::addTable(const Common::UString &nameMale, const Common::UString &nameFemale,
                           uint32 languageID, TalkTable *&m, TalkTable *&f) {

	if (!nameMale.empty()) {
		Common::SeekableReadStream *tlkM = ResMan.getResource(nameMale, kFileTypeTLK);
		if (!tlkM)
			throw Common::Exception("No such talk table \"%s\"", nameMale.c_str());

		m = TalkTable::load(tlkM, languageID);
	}

	if (!nameFemale.empty()) {
		Common::SeekableReadStream *tlkF = ResMan.getResource(nameFemale, kFileTypeTLK);
		if (tlkF)
			f = TalkTable::load(tlkF, languageID);
	}
}

void TalkManager::addMainTable(const Common::UString &nameMale,
                               const Common::UString &nameFemale, uint32 languageID) {
	removeMainTable();

	try {
		addTable(nameMale, nameFemale, languageID, _mainTableM, _mainTableF);
	} catch (...) {
		removeMainTable();
		throw;
	}
}

void TalkManager::addAltTable(const Common::UString &nameMale,
                              const Common::UString &nameFemale, uint32 languageID) {
	removeAltTable();

	try {
		addTable(nameMale, nameFemale, languageID, _altTableM, _altTableF);
	} catch (...) {
		removeAltTable();
		throw;
	}
}

void TalkManager::removeMainTable() {
	delete _mainTableM;
	delete _mainTableF;

	_mainTableM = 0;
	_mainTableF = 0;
}

void TalkManager::removeAltTable() {
	delete _altTableM;
	delete _altTableF;

	_altTableM = 0;
	_altTableF = 0;
}

static const Common::UString kEmptyString = "";
const Common::UString &TalkManager::getString(uint32 strRef, LanguageGender gender) {
	if (gender == ((LanguageGender) -1))
		gender = _gender;

	if (strRef == kStrRefInvalid)
		return kEmptyString;

	const TalkTable *entry = getEntry(strRef, gender);
	if (!entry)
		return kEmptyString;

	return entry->getString(strRef);
}

const Common::UString &TalkManager::getSoundResRef(uint32 strRef, LanguageGender gender) {
	if (gender == ((LanguageGender) -1))
		gender = _gender;

	if (strRef == kStrRefInvalid)
		return kEmptyString;

	const TalkTable *entry = getEntry(strRef, gender);
	if (!entry)
		return kEmptyString;

	return entry->getSoundResRef(strRef);
}

const TalkTable *TalkManager::getEntry(uint32 strRef, LanguageGender gender) const {
	if (strRef == 0xFFFFFFFF)
		return 0;

	bool alt = (strRef & 0xFF000000) != 0;

	strRef &= 0x00FFFFFF;

	const TalkTable *entry = 0;
	if (alt) {
		if (!entry && (gender == kLanguageGenderFemale) && _altTableF && _altTableF->hasEntry(strRef))
			entry = _altTableF;

		if (!entry && _altTableM && _altTableM->hasEntry(strRef))
			entry = _altTableM;
	}

	if (!entry && (gender == kLanguageGenderFemale) && _mainTableF && _mainTableF->hasEntry(strRef))
		entry = _mainTableF;

	if (!entry && _mainTableM && _mainTableM->hasEntry(strRef))
		entry = _mainTableM;

	return entry;
}

} // End of namespace Aurora
