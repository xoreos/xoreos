/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

#include "common/stream.h"
#include "common/util.h"

#include "aurora/talktable.h"
#include "aurora/aurorafile.h"

static const uint32 kTLKID     = MKID_BE('TLK ');
static const uint32 kVersion3  = MKID_BE('V3.0');

namespace Aurora {

TalkTable::TalkTable() {
	_stream = 0;
}

TalkTable::~TalkTable() {
	clear();
}

void TalkTable::clear() {
	_entryList.clear();
	delete _stream; _stream = 0;
}

bool TalkTable::load(Common::SeekableReadStream &stream) {
	if (stream.readUint32BE() != kTLKID) {
		warning("TalkTable::load(): Not a TLK file");
		return false;
	}

	_version = stream.readUint32BE();
	if (_version != kVersion3) {
		warning("TalkTable::load(): Unsupported file version");
		return false;
	}

	_language = (Language)stream.readUint32LE();
	uint32 stringCount = stream.readUint32LE();
	uint32 tableOffset = stream.readUint32LE();

	_entryList.resize(stringCount);

	for (uint32 i = 0; i < stringCount; i++) {
		_entryList[i].flags = (EntryFlags)stream.readUint32LE();
		_entryList[i].soundResRef = AuroraFile::readRawString(stream, 16);
		_entryList[i].volumeVariance = stream.readUint32LE();
		_entryList[i].pitchVariance = stream.readUint32LE();
		_entryList[i].offset = stream.readUint32LE() + tableOffset;
		_entryList[i].length = stream.readUint32LE();
		_entryList[i].soundLength = AuroraFile::readFloat(stream);
	}

	if (stream.err()) {
		warning("TalkTable::load(): Read error");
		return false;
	}

	_stream = &stream;
	return true;
}

std::string TalkTable::getString(uint32 stringRef) {
	// If invalid or not loaded, return an empty string
	if (stringRef >= _entryList.size())
		return "";

	_stream->seek(_entryList[stringRef].offset);
	return AuroraFile::readRawString(*_stream, _entryList[stringRef].length);
}

const TalkTable::Entry *TalkTable::getEntry(uint32 stringRef) const {
	// If invalid or not loaded, return 0
	if (stringRef >= _entryList.size())
		return 0;

	return &_entryList[stringRef];
}

} // End of namespace Aurora
