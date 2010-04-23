/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file aurora/ndsrom.cpp
 *  Nintendo DS ROM parsing.
 */

// Based on http://dsibrew.org/wiki/NDS_Format

#include "boost/algorithm/string.hpp"

#include "common/stream.h"
#include "common/util.h"

#include "aurora/ndsrom.h"
#include "aurora/error.h"
#include "aurora/util.h"

namespace Aurora {

NDSFile::NDSFile() {
}

NDSFile::~NDSFile() {
}

void NDSFile::clear() {
	_resources.clear();
}

void NDSFile::load(Common::SeekableReadStream &nds) {
	clear();

	if (!isNDS(nds))
		throw Common::Exception("Not a support NDS ROM file");

	nds.seek(0x40);

	uint32 fileNameTableOffset = nds.readUint32LE();
	uint32 fileNameTableLength = nds.readUint32LE();
	uint32 fatOffset           = nds.readUint32LE();
	//uint32 fatLength = nds.readUint32LE();

	try {

		readNames(nds, fileNameTableOffset, fileNameTableLength);
		readFAT(nds, fatOffset);

	if (nds.err())
		throw Common::Exception(Common::kReadError);

	} catch (Common::Exception &e) {
		e.add("Failed reading NDS file");
		throw e;
	}

}

void NDSFile::readNames(Common::SeekableReadStream &nds, uint32 offset, uint32 length) {
	if (!nds.seek(offset + 8))
		throw Common::Exception(Common::kSeekError);

	while (((uint32) nds.pos()) < (offset + length)) {
		Resource res;

		byte nameLength = nds.readByte();

		Common::UString name;

		name.readASCII(nds, nameLength);
		name.tolower();

		res.name = setFileType(name, kFileTypeNone);
		res.type = getFileType(name);

		_resources.push_back(res);
	}
}

void NDSFile::readFAT(Common::SeekableReadStream &nds, uint32 offset) {
	if (!nds.seek(offset))
		throw Common::Exception(Common::kSeekError);

	for (ResourceList::iterator res = _resources.begin(); res != _resources.end(); ++res) {
		res->offset = nds.readUint32LE();
		res->size   = nds.readUint32LE() - res->offset; // Value is the end offset
	}
}

const NDSFile::ResourceList &NDSFile::getResources() const {
	return _resources;
}

Common::SeekableReadStream *NDSFile::getResource(Common::SeekableReadStream &stream,
		uint32 offset, uint32 size) {

	if (!stream.seek(offset))
		return 0;

	return stream.readStream(size);
}

bool NDSFile::isNDS(Common::SeekableReadStream &stream) {
	if (!stream.seek(0))
		return false;

	Common::UString gameName;
	gameName.readASCII(stream, 12);
	if (gameName != "SONICCHRON") // Should be the only game we will accept.
		return false;

	if (!stream.seek(0x40))
		return false;

	return true;
}

} // End of namespace Aurora
