/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file aurora/talktable.cpp
 *  Handling BioWare's TLKs (talk tables).
 */

#include "common/stream.h"
#include "common/util.h"

#include "aurora/talktable.h"
#include "aurora/error.h"

static const uint32 kTLKID     = MKID_BE('TLK ');
static const uint32 kVersion3  = MKID_BE('V3.0');
static const uint32 kVersion4  = MKID_BE('V4.0');

namespace Aurora {

TalkTable::TalkTable(Common::SeekableReadStream *tlk) : _tlk(tlk), _stringsOffset(0) {
	assert(tlk);

	load();
}

TalkTable::~TalkTable() {
	delete _tlk;
}

void TalkTable::load() {
	readHeader(*_tlk);

	if (_id != kTLKID)
		throw Common::Exception("Not a TLK file");

	if (_version != kVersion3 && _version != kVersion4)
		throw Common::Exception("Unsupported TLK file version %08X", _version);

	_language = (Language) (_tlk->readUint32LE() * 2);

	uint32 stringCount = _tlk->readUint32LE();
	_entryList.resize(stringCount);

	// V4 added this field; it's right after the header in V3
	uint32 tableOffset = 20;
	if (_version == kVersion4)
		tableOffset = _tlk->readUint32LE();

	_stringsOffset = _tlk->readUint32LE();

	// Go to the table
	_tlk->seek(tableOffset);

	try {

		// Read in all the table data
		if (_version == kVersion3)
			readEntryTableV3();
		else
			readEntryTableV4();

		if (_tlk->err())
			throw Common::Exception(Common::kReadError);

	} catch (Common::Exception &e) {
		e.add("Failed reading TLK file");
		throw e;
	}

}

void TalkTable::readEntryTableV3() {
	for (EntryList::iterator entry = _entryList.begin(); entry != _entryList.end(); ++entry) {
		entry->flags          = _tlk->readUint32LE();
		entry->soundResRef.readASCII(*_tlk, 16);
		entry->volumeVariance = _tlk->readUint32LE();
		entry->pitchVariance  = _tlk->readUint32LE();
		entry->offset         = _tlk->readUint32LE() + _stringsOffset;
		entry->length         = _tlk->readUint32LE();
		entry->soundLength    = _tlk->readIEEEFloatLE();
	}
}

void TalkTable::readEntryTableV4() {
	for (EntryList::iterator entry = _entryList.begin(); entry != _entryList.end(); ++entry) {
		entry->soundID = _tlk->readUint32LE();
		entry->offset  = _tlk->readUint32LE();
		entry->length  = _tlk->readUint16LE();
		entry->flags   = kFlagTextPresent;
	}
}

void TalkTable::readString(Entry &entry) {
	if (!entry.text.empty() || (entry.length == 0) || !(entry.flags & kFlagTextPresent))
		// We already have the string
		return;

	assert(_tlk);

	if (!_tlk->seek(entry.offset))
		throw Common::Exception(Common::kSeekError);

	// TODO: Different encodings for different languages, probably
	entry.text.readLatin9(*_tlk, MIN<uint32>(entry.length, _tlk->size() - _tlk->pos()));
}

Language TalkTable::getLanguage() const {
	return _language;
}

const TalkTable::Entry *TalkTable::getEntry(uint32 strRef) {
	// If invalid or not loaded, return 0
	if (strRef >= _entryList.size())
		return 0;

	Entry &entry = _entryList[strRef];

	readString(entry);

	return &entry;
}

} // End of namespace Aurora
