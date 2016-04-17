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
 *  Handling BioWare's SSFs (sound set file).
 */

/* See BioWare's own specs released for Neverwinter Nights modding
 * (<https://github.com/xoreos/xoreos-docs/tree/master/specs/bioware>)
 */

#include "src/common/readstream.h"
#include "src/common/util.h"
#include "src/common/strutil.h"
#include "src/common/error.h"
#include "src/common/encoding.h"

#include "src/aurora/resman.h"
#include "src/aurora/ssffile.h"

static const uint32 kSSFID     = MKTAG('S', 'S', 'F', ' ');
static const uint32 kVersion10 = MKTAG('V', '1', '.', '0');
static const uint32 kVersion11 = MKTAG('V', '1', '.', '1');

namespace Aurora {

SSFFile::Sound::Sound(const Common::UString &f, uint32 s) : soundFile(f), strRef(s) {
}


SSFFile::SSFFile(Common::SeekableReadStream &ssf) {
	load(ssf);
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
}

SSFFile::~SSFFile() {
}

void SSFFile::load(Common::SeekableReadStream &ssf) {
	try {
		size_t entryCount, offEntryTable;
		Version version = readSSFHeader(ssf, entryCount, offEntryTable);

		_sounds.resize(entryCount);

		readEntries(ssf, version, offEntryTable);

	} catch (Common::Exception &e) {
		e.add("Failed reading SSF file");
		throw;
	}
}

SSFFile::Version SSFFile::readSSFHeader(Common::SeekableReadStream &ssf,
                                        size_t &entryCount, size_t &offEntryTable) {
	readHeader(ssf);

	if (_id != kSSFID)
		throw Common::Exception("Not a SSF file (%s)", Common::debugTag(_id).c_str());

	if ((_version != kVersion10) && (_version != kVersion11))
		throw Common::Exception("Unsupported SSF file version %s", Common::debugTag(_version).c_str());

	entryCount    = ssf.readUint32LE();
	offEntryTable = ssf.readUint32LE();

	// Plain old version V1.0 used in NWN (and NWN2)
	if (_version == kVersion10)
		return kVersion10_NWN;

	// NWN2's V1.1
	if ((offEntryTable < ssf.size()) && ((ssf.size() - offEntryTable) >= ((4 + 32 + 4) * entryCount)))
		return kVersion11_NWN2;

	offEntryTable = entryCount;
	entryCount    = (ssf.size() - offEntryTable) / 4;

	// Sanity check
	if ((offEntryTable > ssf.size()) || (((ssf.size() - offEntryTable) % 4) != 0))
		throw Common::Exception("Invalid SSF header (%u, %u)", (uint32) ssf.size(), (uint32) offEntryTable);

	// KotOR's V1.1
	return kVersion11_KotOR;
}

void SSFFile::readEntries(Common::SeekableReadStream &ssf, Version version, size_t offset) {
	ssf.seek(offset);

	switch (version) {
		case kVersion10_NWN:
			readEntriesNWN(ssf, 16);
			break;

		case kVersion11_NWN2:
			readEntriesNWN(ssf, 32);
			break;

		case kVersion11_KotOR:
			readEntriesKotOR(ssf);
			break;

		default:
			break;
	}
}

void SSFFile::readEntriesNWN(Common::SeekableReadStream &ssf, size_t soundFileLen) {
	/* The NWN/NWN2 versions of an SSF file (V1.0 and V1.1) begin with a list of
	   offsets to the data entries. Each of these contains a ResRef of a sound
	   file and a StrRef of a text. */

	size_t count = _sounds.size();

	std::vector<uint32> offsets;

	offsets.resize(count);

	for (size_t i = 0; i < count; i++)
		offsets[i] = ssf.readUint32LE();

	for (size_t i = 0; i < count; i++) {
		ssf.seek(offsets[i]);

		_sounds[i].soundFile = Common::readStringFixed(ssf, Common::kEncodingASCII, soundFileLen);
		_sounds[i].strRef    = ssf.readUint32LE();
	}
}

void SSFFile::readEntriesKotOR(Common::SeekableReadStream &ssf) {
	/* The KotOR/KotOR2 version of an SSF file (V1.1) is just a list of StrRefs. */

	for (SoundSet::iterator sound = _sounds.begin(); sound != _sounds.end(); ++sound)
		sound->strRef = ssf.readUint32LE();
}

size_t SSFFile::getSoundCount() const {
	return _sounds.size();
}

const Common::UString &SSFFile::getSoundFile(size_t index) const {
	static const Common::UString kEmptyString = "";

	if (index >= _sounds.size())
		return kEmptyString;

	return _sounds[index].soundFile;
}

uint32 SSFFile::getStrRef(size_t index) const {
	if (index >= _sounds.size())
		return kStrRefInvalid;

	return _sounds[index].strRef;
}

void SSFFile::getSound(size_t index, Common::UString &soundFile, uint32 &strRef) const {
	if (index >= _sounds.size()) {
		soundFile.clear();
		strRef = kStrRefInvalid;

		return;
	}

	soundFile = _sounds[index].soundFile;
	strRef    = _sounds[index].strRef;
}

void SSFFile::setSoundFile(size_t index, const Common::UString &soundFile) {
	if (_sounds.size() <= index)
		_sounds.resize(index + 1);

	_sounds[index].soundFile = soundFile;
}

void SSFFile::setStrRef(size_t index, uint32 strRef) {
	if (_sounds.size() <= index)
		_sounds.resize(index + 1);

	_sounds[index].strRef = strRef;
}

void SSFFile::setSound(size_t index, const Common::UString &soundFile, uint32 strRef) {
	if (_sounds.size() <= index)
		_sounds.resize(index + 1);

	_sounds[index].soundFile = soundFile;
	_sounds[index].strRef    = strRef;
}

} // End of namespace Aurora
