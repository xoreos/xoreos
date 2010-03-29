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

#include "aurora/keyfile.h"
#include "aurora/aurorafile.h"

static const uint32 kKeyID     = MKID_BE('KEY ');
static const uint32 kVersion1  = MKID_BE('V1  ');
static const uint32 kVersion11 = MKID_BE('V1.1');

namespace Aurora {

KeyFile::KeyFile() {
}

KeyFile::~KeyFile() {
}

void KeyFile::clear() {
	_bifs.clear();
	_resources.clear();
}

bool KeyFile::load(Common::SeekableReadStream &key) {
	if (key.readUint32BE() != kKeyID) {
		warning("KeyFile::load(): Not a KEY file");
		return false;
	}

	_version = key.readUint32BE();
	if (_version != kVersion1 && _version != kVersion11) {
		warning("KeyFile::load(): Unsupported file version");
		return false;
	}

	uint32 bifCount = key.readUint32LE();
	uint32 keyCount = key.readUint32LE();

	_bifs.reserve(bifCount);
	_resources.reserve(keyCount);

	// Version 1.1 has some NULL bytes here
	if (_version == kVersion11)
		key.skip(4);

	uint32 offFileTable     = key.readUint32LE();
	uint32 offKeyTable      = key.readUint32LE();

	key.skip( 8); // Build year and day
	key.skip(32); // Reserved

	key.seek(offFileTable);
	if (!readBifList(key, bifCount))
		return false;

	key.seek(offKeyTable);
	if (!readKeyList(key, keyCount))
		return false;

	if (key.err()) {
		warning("KeyFile::load(): Read error");
		return false;
	}

	return true;
}

bool KeyFile::readBifList(Common::SeekableReadStream &key, uint32 bifCount) {
	for (uint32 i = 0; i < bifCount; i++) {
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

		std::string name = AuroraFile::readRawString(key, nameSize, nameOffset);

		AuroraFile::cleanupPath(name);

		_bifs.push_back(name);
	}

	return true;
}

bool KeyFile::readKeyList(Common::SeekableReadStream &key, uint32 keyCount) {
	for (uint32 i = 0; i < keyCount; i++) {
		Resource resource;

		resource.name = AuroraFile::readRawString(key, 16);
		resource.type = (FileType) key.readUint16LE();

		uint32 id = key.readUint32LE();

		// The new flags field holds the bifIndex now. The rest contains fixed
		// resource info.
		if (_version == kVersion11) {
			uint32 flags = key.readUint32LE();
			resource.bifIndex = (flags & 0xFFF00000) >> 20;
		} else
			resource.bifIndex = id >> 20;

		// TODO: Fixed resources?
		resource.resIndex = id & 0xFFFFF;

		_resources.push_back(resource);
	}

	return true;
}

const KeyFile::BifList &KeyFile::getBifs() const {
	return _bifs;
}

const KeyFile::ResourceList &KeyFile::getResources() const {
	return _resources;
}

} // End of namespace Aurora
