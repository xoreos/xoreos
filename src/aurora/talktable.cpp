/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
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

namespace Aurora {

TalkTable::TalkTable() {
}

TalkTable::~TalkTable() {
	AuroraBase::clear();

	clear();
}

void TalkTable::clear() {
	_entryList.clear();
}

void TalkTable::load(Common::SeekableReadStream &tlk) {
	readHeader(tlk);

	if (_id != kTLKID)
		throw Common::Exception("Not a TLK file");

	if (_version != kVersion3)
		throw Common::Exception("Unsupported TLK file version %08X", _version);

	tlk.readUint32LE(); // Skip language
	uint32 stringCount = tlk.readUint32LE();
	uint32 tableOffset = tlk.readUint32LE();

	_entryList.resize(stringCount);

	try {

		// First, read in all the table data
		readEntryTable(tlk);

		// Now go and pick up all the strings
		readStrings(tlk, tableOffset);

		if (tlk.err())
			throw Common::Exception(Common::kReadError);

	} catch (Common::Exception &e) {
		e.add("Failed reading TLK file");
		throw e;
	}

}

void TalkTable::readEntryTable(Common::SeekableReadStream &tlk) {
	for (EntryList::iterator entry = _entryList.begin(); entry != _entryList.end(); ++entry) {
		entry->flags          = (EntryFlags) tlk.readUint32LE();
		entry->soundResRef    = AuroraFile::readRawString(tlk, 16);
		entry->volumeVariance = tlk.readUint32LE();
		entry->pitchVariance  = tlk.readUint32LE();
		entry->offset         = tlk.readUint32LE();
		entry->length         = tlk.readUint32LE();
		entry->soundLength    = AuroraFile::readFloat(tlk);
	}
}

void TalkTable::readStrings(Common::SeekableReadStream &tlk, uint32 dataOffset) {
	for (EntryList::iterator entry = _entryList.begin(); entry != _entryList.end(); ++entry) {
		if (!tlk.seek(dataOffset + entry->offset))
			throw Common::Exception(Common::kSeekError);

		entry->text = AuroraFile::readRawString(tlk, entry->length);
	}
}

const TalkTable::Entry *TalkTable::getEntry(uint32 strRef) const {
	// If invalid or not loaded, return 0
	if (strRef >= _entryList.size())
		return 0;

	return &_entryList[strRef];
}

} // End of namespace Aurora
