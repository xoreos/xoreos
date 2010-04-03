/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file aurora/ssffile.cpp
 *  Handling BioWare's SSFs (sound set file).
 */

#include "common/stream.h"
#include "common/util.h"

#include "aurora/ssffile.h"

static const uint32 kSSFID     = MKID_BE('SSF ');
static const uint32 kVersion1  = MKID_BE('V1.0');
static const uint32 kVersion11 = MKID_BE('V1.1');

namespace Aurora {

SSFFile::SSFFile() {
}

SSFFile::~SSFFile() {
}

void SSFFile::clear() {
	AuroraBase::clear();

	_soundSet.clear();
}

bool SSFFile::load(Common::SeekableReadStream &ssf) {
	clear();

	readHeader(ssf);

	if (_id != kSSFID) {
		warning("SSFFile::load(): Not a SSF file");
		return false;
	}

	if ((_version != kVersion1) && (_version != kVersion11)) {
		warning("SSFFile::load(): Unsupported file version");
		return false;
	}

	if (_version == kVersion11) {
		warning("SSFFile::load(): TODO: V1.1, found in KotOR");
		return false;
	}

	uint32 entryCount    = ssf.readUint32LE();
	uint32 offEntryTable = ssf.readUint32LE();

	_soundSet.resize(entryCount);

	if (!readEntries(ssf, offEntryTable))
		return false;

	if (ssf.err()) {
		warning("SSFFile::load(): Read error");
		return false;
	}

	return true;
}

bool SSFFile::readEntries(Common::SeekableReadStream &ssf, uint32 offset) {
	if (!ssf.seek(offset))
		return false;

	uint32 count = _soundSet.size();

	std::vector<uint32> offsets;

	offsets.resize(count);

	for (uint32 i = 0; i < count; i++)
		offsets[i] = ssf.readUint32LE();

	for (uint32 i = 0; i < count; i++) {
		if (!ssf.seek(offsets[i]))
			return false;

		_soundSet[i].fileName  = AuroraFile::readRawString(ssf, 16);
		_soundSet[i].stringRef = ssf.readUint32LE();
	}

	return true;
}

const SSFFile::SoundSet &SSFFile::getSounds() const {
	return _soundSet;
}

} // End of namespace Aurora
