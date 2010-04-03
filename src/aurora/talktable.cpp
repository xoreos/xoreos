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

bool TalkTable::load(Common::SeekableReadStream &stream) {
	readHeader(stream);

	if (_id != kTLKID) {
		warning("TalkTable::load(): Not a TLK file");
		return false;
	}

	if (_version != kVersion3) {
		warning("TalkTable::load(): Unsupported file version");
		return false;
	}

	stream.readUint32LE(); // Skip language
	uint32 stringCount = stream.readUint32LE();
	uint32 tableOffset = stream.readUint32LE();

	_entryList.resize(stringCount);

	// First, read in all the table data
	for (uint32 i = 0; i < stringCount; i++) {
		_entryList[i].flags = (EntryFlags)stream.readUint32LE();
		_entryList[i].soundResRef = AuroraFile::readRawString(stream, 16);
		_entryList[i].volumeVariance = stream.readUint32LE();
		_entryList[i].pitchVariance = stream.readUint32LE();
		_entryList[i].offset = stream.readUint32LE();
		_entryList[i].length = stream.readUint32LE();
		_entryList[i].soundLength = AuroraFile::readFloat(stream);
	}

	// Now go and pick up all the strings
	for (uint32 i = 0; i < stringCount; i++) {
		stream.seek(_entryList[i].offset + tableOffset);
		_entryList[i].text = AuroraFile::readRawString(stream, _entryList[i].length);
	}

	if (stream.err()) {
		warning("TalkTable::load(): Read error");
		return false;
	}

	return true;
}

const TalkTable::Entry *TalkTable::getEntry(uint32 strRef) const {
	// If invalid or not loaded, return 0
	if (strRef >= _entryList.size())
		return 0;

	return &_entryList[strRef];
}

} // End of namespace Aurora
