/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file aurora/talktable.cpp
 *  Handling BioWare's TLKs (talk tables).
 */

#include "common/stream.h"
#include "common/util.h"

#include "aurora/talktable.h"
#include "aurora/error.h"

static const uint32 kTLKID     = MKTAG('T', 'L', 'K', ' ');
static const uint32 kVersion3  = MKTAG('V', '3', '.', '0');
static const uint32 kVersion4  = MKTAG('V', '4', '.', '0');

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
		throw;
	}

}

void TalkTable::readEntryTableV3() {
	for (EntryList::iterator entry = _entryList.begin(); entry != _entryList.end(); ++entry) {
		entry->flags          = _tlk->readUint32LE();
		entry->soundResRef.readFixedASCII(*_tlk, 16);
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
	entry.text.readFixedLatin9(*_tlk, MIN<uint32>(entry.length, _tlk->size() - _tlk->pos()));
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
