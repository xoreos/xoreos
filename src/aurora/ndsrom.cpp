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

/** @file aurora/ndsrom.cpp
 *  Nintendo DS ROM parsing.
 */

// Based on http://dsibrew.org/wiki/NDS_Format

#include <boost/algorithm/string.hpp>

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
		throw;
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

		name.readFixedASCII(nds, nameLength);
		name.tolower();

		res.name  = TypeMan.setFileType(name, kFileTypeNone);
		res.type  = TypeMan.getFileType(name);
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
	gameName.readFixedASCII(stream, 12);
	if (gameName != "SONICCHRON") // Should be the only game we will accept.
		return false;

	if (!stream.seek(0x40))
		return false;

	return true;
}

const Archive::ResourceList &NDSFile::getResources() const {
	return _resources;
}

const NDSFile::IResource &NDSFile::getIResource(uint32 index) const {
	if (index >= _iResources.size())
		throw Common::Exception("Resource index out of range (%d/%d)", index, _iResources.size());

	return _iResources[index];
}

uint32 NDSFile::getResourceSize(uint32 index) const {
	return getIResource(index).size;
}

Common::SeekableReadStream *NDSFile::getResource(uint32 index) const {
	const IResource &res = getIResource(index);
	if (res.size == 0)
		return new Common::MemoryReadStream(0, 0);

	Common::File nds;
	open(nds);

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
