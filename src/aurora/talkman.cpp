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
                           TalkTable *&m, TalkTable *&f) {

	if (!nameMale.empty()) {
		Common::SeekableReadStream *tlkM = ResMan.getResource(nameMale, kFileTypeTLK);
		if (!tlkM)
			throw Common::Exception("No such talk table \"%s\"", nameMale.c_str());

		m = new TalkTable(tlkM);
	}

	if (!nameFemale.empty()) {
		Common::SeekableReadStream *tlkF = ResMan.getResource(nameFemale, kFileTypeTLK);
		if (tlkF)
			f = new TalkTable(tlkF);
	}
}

void TalkManager::addMainTable(const Common::UString &nameMale, const Common::UString &nameFemale) {
	removeMainTable();

	try {
		addTable(nameMale, nameFemale, _mainTableM, _mainTableF);
	} catch (...) {
		removeMainTable();
		throw;
	}
}

void TalkManager::addAltTable(const Common::UString &nameMale, const Common::UString &nameFemale) {
	removeAltTable();

	try {
		addTable(nameMale, nameFemale, _altTableM, _altTableF);
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

const Common::UString &TalkManager::getString(uint32 strRef, LanguageGender gender) {
	if (gender == ((LanguageGender) -1))
		gender = _gender;

	static const Common::UString kEmptyString = "";
	if (strRef == kStrRefInvalid)
		return kEmptyString;

	const TalkTable::Entry *entry = getEntry(strRef, gender);
	if (!entry)
		return kEmptyString;

	return entry->text;
}

const Common::UString &TalkManager::getSoundResRef(uint32 strRef, LanguageGender gender) {
	if (gender == ((LanguageGender) -1))
		gender = _gender;

	static const Common::UString kEmptyString = "";
	if (strRef == kStrRefInvalid)
		return kEmptyString;

	const TalkTable::Entry *entry = getEntry(strRef, gender);
	if (!entry)
		return kEmptyString;

	return entry->soundResRef;
}

const TalkTable::Entry *TalkManager::getEntry(uint32 strRef, LanguageGender gender) {
	if (strRef == 0xFFFFFFFF)
		return 0;

	bool alt = (strRef & 0xFF000000) != 0;

	strRef &= 0x00FFFFFF;

	const TalkTable::Entry *entry = 0;
	if (alt) {
		if ((gender == kLanguageGenderFemale) && _altTableF)
			entry = _altTableF->getEntry(strRef);

		if (!entry && _altTableM)
			entry = _altTableM->getEntry(strRef);
	}

	if (!entry && (gender == kLanguageGenderFemale) && _mainTableF)
		entry = _mainTableF->getEntry(strRef);

	if (!entry && _mainTableM)
		entry = _mainTableM->getEntry(strRef);

	return entry;
}

} // End of namespace Aurora
