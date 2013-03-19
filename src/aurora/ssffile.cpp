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

/** @file aurora/ssffile.cpp
 *  Handling BioWare's SSFs (sound set file).
 */

#include "common/stream.h"
#include "common/util.h"

#include "aurora/error.h"
#include "aurora/resman.h"
#include "aurora/ssffile.h"

static const uint32 kSSFID     = MKTAG('S', 'S', 'F', ' ');
static const uint32 kVersion1  = MKTAG('V', '1', '.', '0');
static const uint32 kVersion11 = MKTAG('V', '1', '.', '1');

namespace Aurora {

SSFFile::SSFFile(Common::SeekableReadStream &ssf) {
	load(ssf);

	_emptySound.strRef = kStrRefInvalid;
}

SSFFile::SSFFile(const Common::UString &ssf) {
	Common::SeekableReadStream *res = ResMan.getResource(ssf, kFileTypeSSF);
	if (!res)
		throw Common::Exception("No such SSF \"%s\"", ssf.c_str());

	try {
		load(*res);
	} catch (...) {
		delete res;
		throw;
	}

	delete res;

	_emptySound.strRef = kStrRefInvalid;
}

SSFFile::~SSFFile() {
}

void SSFFile::load(Common::SeekableReadStream &ssf) {
	readHeader(ssf);

	if (_id != kSSFID)
		throw Common::Exception("Not a SSF file");

	if ((_version != kVersion1) && (_version != kVersion11))
		throw Common::Exception("Unsupported SSF file version %08X", _version);

	uint32 entryCount = 0;
	if (_version == kVersion1)
		entryCount = ssf.readUint32LE();

	uint32 offEntryTable = ssf.readUint32LE();

	if (_version == kVersion11)
		entryCount = (ssf.size() - offEntryTable) / 4;

	_sounds.resize(entryCount);

	try {

		readEntries(ssf, offEntryTable);

		if (ssf.err())
			throw Common::Exception(Common::kReadError);

	} catch (Common::Exception &e) {
		e.add("Failed reading SSF file");
		throw;
	}

}

void SSFFile::readEntries(Common::SeekableReadStream &ssf, uint32 offset) {
	if (!ssf.seek(offset))
		throw Common::Exception(Common::kSeekError);

	if      (_version == kVersion1)
		readEntries1(ssf);
	else if (_version == kVersion11)
		readEntries11(ssf);
}

void SSFFile::readEntries1(Common::SeekableReadStream &ssf) {
	// V1.0 begins with a list of offsets to the data entries.
	// Each data entry has a ResRef of a sound file and a StrRef of a text.

	uint32 count = _sounds.size();

	std::vector<uint32> offsets;

	offsets.resize(count);

	for (uint32 i = 0; i < count; i++)
		offsets[i] = ssf.readUint32LE();

	for (uint32 i = 0; i < count; i++) {
		if (!ssf.seek(offsets[i]))
			throw Common::Exception(Common::kSeekError);

		_sounds[i].fileName.readFixedASCII(ssf, 16);
		_sounds[i].strRef = ssf.readUint32LE();
	}
}

void SSFFile::readEntries11(Common::SeekableReadStream &ssf) {
	// V1.1 is just a list of StrRef

	for (SoundSet::iterator sound = _sounds.begin(); sound != _sounds.end(); ++sound)
		sound->strRef = ssf.readUint32LE();
}

const SSFFile::Sound &SSFFile::getSound(uint32 index) const {
	if (index >= _sounds.size())
		return _emptySound;

	return _sounds[index];
}

} // End of namespace Aurora
