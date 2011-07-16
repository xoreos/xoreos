/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
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
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file aurora/herffile.cpp
 *  BioWare's HERF file parsing.
 */

#include "common/util.h"
#include "common/file.h"
#include "common/stream.h"

#include "aurora/herffile.h"
#include "aurora/error.h"
#include "aurora/util.h"
#include "aurora/resman.h"

namespace Aurora {

HERFFile::HERFFile(const Common::UString &fileName) : _fileName(fileName) {
	load();
}

HERFFile::~HERFFile() {
}

void HERFFile::clear() {
	_resources.clear();
}

void HERFFile::load() {
	Common::SeekableReadStream *herf = ResMan.getResource(setFileType(_fileName, kFileTypeNone), kFileTypeHERF);
	if (!herf)
		throw Common::Exception(Common::kOpenError);

	// Read in the resource table
	herf->skip(4);
	uint32 resCount = herf->readUint32LE();

	for (uint32 i = 0; i < resCount; i++) {
		uint32 nameHash = herf->readUint32LE();

		IResource &iResource = _iResources[nameHash];

		iResource.size = herf->readUint32LE();
		iResource.offset = herf->readUint32LE();

		if (iResource.offset >= (uint32)herf->size())
			throw Common::Exception("HERFFile::load(): Resource goes beyond end of file");
	}

	readNames();

	if (herf->err())
		throw Common::Exception(Common::kReadError);
}

void HERFFile::readNames() {
	// We need to find the erf.dict file inside the archive
	// Of course, we therefore need to know the name hash first
	// It is 0xEA828DD4
	Common::SeekableReadStream *dict = getResource(0xEA828DD4);

	if (!dict)
		throw Common::Exception("HERFFile::readNames(): No erf.dict file");

	dict->skip(8); // unknown

	while (dict->pos() < dict->size()) {
		Resource res;
		res.index = dict->readUint32LE();

		if (res.index == 0)
			break;

		Common::UString name;
		name.readFixedASCII(*dict, 128);
		name.tolower();

		res.name = setFileType(name, kFileTypeNone);
		res.type = getFileType(name);

		_resources.push_back(res);
	}

	delete dict;
}

const Archive::ResourceList &HERFFile::getResources() const {
	return _resources;
}

const HERFFile::IResource &HERFFile::getIResource(uint32 index) const {
	if (_iResources.count(index) == 0)
		throw Common::Exception("Resource hash not found 0x%08x", index);

	return _iResources.find(index)->second;
}

uint32 HERFFile::getResourceSize(uint32 index) const {
	return getIResource(index).size;
}

Common::SeekableReadStream *HERFFile::getResource(uint32 index) const {
	const IResource &res = getIResource(index);
	if (res.size == 0)
		return new Common::MemoryReadStream(0, 0);

	Common::SeekableReadStream *herf = ResMan.getResource(setFileType(_fileName, kFileTypeNone), kFileTypeHERF);
	if (!herf)
		throw Common::Exception(Common::kOpenError);

	if (!herf->seek(res.offset))
		throw Common::Exception(Common::kSeekError);

	Common::SeekableReadStream *resStream = herf->readStream(res.size);

	if (!resStream || (((uint32) resStream->size()) != res.size)) {
		delete resStream;
		throw Common::Exception(Common::kReadError);
	}

	return resStream;
}

} // End of namespace Aurora
