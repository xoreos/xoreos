/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file aurora/keyfile.cpp
 *  Handling BioWare's KEYs (resource index files).
 */

#include "common/stream.h"
#include "common/util.h"

#include "aurora/keyfile.h"

static const uint32 kKEYID     = MKID_BE('KEY ');
static const uint32 kVersion1  = MKID_BE('V1  ');
static const uint32 kVersion11 = MKID_BE('V1.1');

namespace Aurora {

KEYFile::KEYFile() {
}

KEYFile::~KEYFile() {
}

void KEYFile::clear() {
	AuroraBase::clear();

	_bifs.clear();
	_resources.clear();
}

bool KEYFile::load(Common::SeekableReadStream &key) {
	clear();

	readHeader(key);

	if (_id != kKEYID) {
		warning("KEYFile::load(): Not a KEY file");
		return false;
	}

	if ((_version != kVersion1) && (_version != kVersion11)) {
		warning("KEYFile::load(): Unsupported file version");
		return false;
	}

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

	_bifs.resize(bifCount);

	if (!readBIFList(key, offFileTable))
		return false;

	_resources.resize(resCount);

	if (!readResList(key, offResTable))
		return false;

	if (key.err()) {
		warning("KEYFile::load(): Read error");
		return false;
	}

	return true;
}

bool KEYFile::readBIFList(Common::SeekableReadStream &key, uint32 offset) {
	if (!key.seek(offset))
		return false;

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

		*bif = AuroraFile::readRawString(key, nameSize, nameOffset);

		AuroraFile::cleanupPath(*bif);
	}

	return true;
}

bool KEYFile::readResList(Common::SeekableReadStream &key, uint32 offset) {
	if (!key.seek(offset))
		return false;

	for (ResourceList::iterator res = _resources.begin(); res != _resources.end(); ++res) {
		res->name = AuroraFile::readRawString(key, 16);
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

	return true;
}

const KEYFile::BIFList &KEYFile::getBIFs() const {
	return _bifs;
}

const KEYFile::ResourceList &KEYFile::getResources() const {
	return _resources;
}

} // End of namespace Aurora
