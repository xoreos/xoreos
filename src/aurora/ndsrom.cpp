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

#include "common/util.h"
#include "common/file.h"
#include "common/stream.h"

#include "aurora/ndsrom.h"
#include "aurora/error.h"
#include "aurora/util.h"

namespace Aurora {

NDSFile::NDSFile(const Common::UString &fileName) : _fileName(fileName) {
	load();
}

NDSFile::~NDSFile() {
}

void NDSFile::clear() {
	_resources.clear();
}

void NDSFile::load() {
	Common::File nds;
	open(nds);

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

	uint32 index = 0;
	while (((uint32) nds.pos()) < (offset + length)) {
		Resource res;

		byte nameLength = nds.readByte();

		Common::UString name;

		name.readASCII(nds, nameLength);
		name.tolower();

		res.name  = setFileType(name, kFileTypeNone);
		res.type  = getFileType(name);
		res.index = index++;

		_resources.push_back(res);
	}
}

void NDSFile::readFAT(Common::SeekableReadStream &nds, uint32 offset) {
	if (!nds.seek(offset))
		throw Common::Exception(Common::kSeekError);

	_iResources.resize(_resources.size());
	for (IResourceList::iterator res = _iResources.begin(); res != _iResources.end(); ++res) {
		res->offset = nds.readUint32LE();
		res->size   = nds.readUint32LE() - res->offset; // Value is the end offset
	}
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

const Archive::ResourceList &NDSFile::getResources() const {
	return _resources;
}

Common::SeekableReadStream *NDSFile::getResource(uint32 index) const {
	if (index >= _iResources.size())
		throw Common::Exception("Resource index out of range (%d/%d)", index, _iResources.size());

	Common::File nds;
	open(nds);

	const IResource &res = _iResources[index];
	if (res.size == 0)
		return 0;

	if (!nds.seek(res.offset))
		throw Common::Exception(Common::kSeekError);

	Common::SeekableReadStream *resStream = nds.readStream(res.size);

	if (!resStream || (((uint32) resStream->size()) != res.size)) {
		delete resStream;
		throw Common::Exception(Common::kReadError);
	}

	return resStream;
}

void NDSFile::open(Common::File &file) const {
	if (!file.open(_fileName))
		throw Common::Exception(Common::kOpenError);
}

} // End of namespace Aurora
