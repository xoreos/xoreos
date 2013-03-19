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

/** @file aurora/keyfile.cpp
 *  Handling BioWare's KEYs (resource index files).
 */

#include "common/util.h"
#include "common/error.h"
#include "common/stream.h"
#include "common/file.h"

#include "aurora/keyfile.h"
#include "aurora/error.h"

static const uint32 kKEYID     = MKTAG('K', 'E', 'Y', ' ');
static const uint32 kVersion1  = MKTAG('V', '1', ' ', ' ');
static const uint32 kVersion11 = MKTAG('V', '1', '.', '1');

namespace Aurora {

KEYFile::KEYFile(const Common::UString &fileName) {
	Common::File key;
	if (!key.open(fileName))
		throw Common::Exception(Common::kOpenError);

	load(key);
}

KEYFile::~KEYFile() {
}

void KEYFile::load(Common::SeekableReadStream &key) {
	readHeader(key);

	if (_id != kKEYID)
		throw Common::Exception("Not a KEY file");

	if ((_version != kVersion1) && (_version != kVersion11))
		throw Common::Exception("Unsupported KEY file version %08X", _version);

	uint32 bifCount = key.readUint32LE();
	uint32 resCount = key.readUint32LE();

	_bifs.reserve(bifCount);
	_resources.reserve(resCount);

	// Version 1.1 has some NULL bytes here
	if (_version == kVersion11)
		key.skip(4);

	uint32 offFileTable     = key.readUint32LE();
	uint32 offResTable      = key.readUint32LE();

	key.skip( 8); // Build year and day
	key.skip(32); // Reserved

	try {

		_bifs.resize(bifCount);
		readBIFList(key, offFileTable);

		_resources.resize(resCount);
		readResList(key, offResTable);

		if (key.err())
			throw Common::Exception(Common::kReadError);

	} catch (Common::Exception &e) {
		e.add("Failed reading KEY file");
		throw;
	}

}

void KEYFile::readBIFList(Common::SeekableReadStream &key, uint32 offset) {
	if (!key.seek(offset))
		throw Common::Exception(Common::kSeekError);

	for (BIFList::iterator bif = _bifs.begin(); bif != _bifs.end(); ++bif) {
		key.skip(4); // File size of the bif

		uint32 nameOffset = key.readUint32LE();
		uint32 nameSize   = 0;

		// nameSize is expanded to 4 bytes in 1.1 and the location is dropped
		if (_version == kVersion11) {
			nameSize = key.readUint32LE();
		} else {
			nameSize = key.readUint16LE();
			key.skip(2); // Location of the bif (HD, CD, ...)
		}

		uint32 curPos = key.seekTo(nameOffset);
		bif->readFixedASCII(key, nameSize);
		key.seekTo(curPos);

		AuroraFile::cleanupPath(*bif);
	}
}

void KEYFile::readResList(Common::SeekableReadStream &key, uint32 offset) {
	if (!key.seek(offset))
		throw Common::Exception(Common::kSeekError);

	for (ResourceList::iterator res = _resources.begin(); res != _resources.end(); ++res) {
		res->name.readFixedASCII(key, 16);
		res->type = (FileType) key.readUint16LE();

		uint32 id = key.readUint32LE();

		// The new flags field holds the bifIndex now. The rest contains fixed
		// resource info.
		if (_version == kVersion11) {
			uint32 flags = key.readUint32LE();
			res->bifIndex = (flags & 0xFFF00000) >> 20;
		} else
			res->bifIndex = id >> 20;

		// TODO: Fixed resources?
		res->resIndex = id & 0xFFFFF;
	}
}

const KEYFile::BIFList &KEYFile::getBIFs() const {
	return _bifs;
}

const KEYFile::ResourceList &KEYFile::getResources() const {
	return _resources;
}

} // End of namespace Aurora
