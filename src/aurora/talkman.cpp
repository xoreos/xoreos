/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file aurora/talkman.cpp
 *  The global talk manager for Aurora strings.
 */

#include "aurora/talkman.h"
#include "aurora/resman.h"

#include "common/error.h"
#include "common/ustring.h"
#include "common/stream.h"

DECLARE_SINGLETON(Aurora::TalkManager)

namespace Aurora {

TalkManager::TalkManager() : _gender(kGenderMale), _mainTableM(0), _mainTableF(0), _altTableM(0), _altTableF(0) {
}

TalkManager::~TalkManager() {
	clear();
}

void TalkManager::clear() {
	removeMainTable();
	removeAltTable();
}

void TalkManager::setGender(Gender gender) {
	_gender = gender;
}

void TalkManager::addMainTable(const Common::UString &name) {
	removeMainTable();

	Common::SeekableReadStream *tlkM = ResMan.getResource(name, kFileTypeTLK);
	if (!tlkM)
		throw Common::Exception("No such talk table \"%s\"", name.c_str());

	Common::SeekableReadStream *tlkF = ResMan.getResource(name + "f", kFileTypeTLK);

	_mainTableM = new TalkTable;
	_mainTableM->load(*tlkM);

	if (tlkF) {
		_mainTableF = new TalkTable;
		_mainTableF->load(*tlkF);
	}
}

void TalkManager::addAltTable(const Common::UString &name) {
	removeAltTable();

	Common::SeekableReadStream *tlkM = ResMan.getResource(name, kFileTypeTLK);
	if (!tlkM)
		throw Common::Exception("No such talk table \"%s\"", name.c_str());

	Common::SeekableReadStream *tlkF = ResMan.getResource(name + "f", kFileTypeTLK);

	_altTableM = new TalkTable;
	_altTableM->load(*tlkM);

	if (tlkF) {
		_altTableF = new TalkTable;
		_altTableF->load(*tlkF);
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

const Common::UString &TalkManager::getString(uint32 strRef) {
	static const Common::UString kEmptyString = "";

	const TalkTable::Entry *entry = getEntry(strRef);
	if (!entry)
		return kEmptyString;

	return entry->text;
}

const Common::UString &TalkManager::getSoundResRef(uint32 strRef) {
	static const Common::UString kEmptyString = "";

	const TalkTable::Entry *entry = getEntry(strRef);
	if (!entry)
		return kEmptyString;

	return entry->soundResRef;
}

const TalkTable::Entry *TalkManager::getEntry(uint32 strRef) {
	if (strRef == 0xFFFFFFFF)
		return 0;

	bool alt = (strRef & 0xFF000000) != 0;

	strRef &= 0x00FFFFFF;

	const TalkTable::Entry *entry = 0;
	if (alt) {
		if ((_gender == kGenderFemale) && _altTableF)
			entry = _altTableF->getEntry(strRef);

		if (!entry && _altTableM)
			entry = _altTableM->getEntry(strRef);
	}

	if (!entry && (_gender == kGenderFemale) && _mainTableF)
		entry = _mainTableF->getEntry(strRef);

	if (!entry && _mainTableM)
		entry = _mainTableM->getEntry(strRef);

	return entry;
}

} // End of namespace Aurora
