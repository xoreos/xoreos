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

/** @file aurora/herffile.cpp
 *  BioWare's HERF file parsing.
 */

#include "common/util.h"
#include "common/file.h"
#include "common/stream.h"
#include "common/hash.h"

#include "aurora/herffile.h"
#include "aurora/error.h"
#include "aurora/util.h"
#include "aurora/resman.h"

namespace Aurora {

HERFFile::HERFFile(const Common::UString &fileName) : _fileName(fileName), _dictIndex(0xFFFFFFFF) {
	load();
}

HERFFile::~HERFFile() {
}

void HERFFile::clear() {
	_resources.clear();
}

void HERFFile::load() {
	Common::SeekableReadStream *herf = ResMan.getResource(TypeMan.setFileType(_fileName, kFileTypeNone), kFileTypeHERF);
	if (!herf)
		throw Common::Exception(Common::kOpenError);

	herf->skip(4);
	uint32 resCount = herf->readUint32LE();

	_resources.resize(resCount);
	_iResources.resize(resCount);

	try {

		readResList(*herf);

	if (herf->err())
		throw Common::Exception(Common::kReadError);

	} catch (Common::Exception &e) {
		delete herf;
		e.add("Failed reading HERF file");
		throw;
	}

	delete herf;
}

void HERFFile::readResList(Common::SeekableReadStream &herf) {
	const uint32 dictHash = Common::hashStringDJB2("erf.dict");

	uint32 index = 0;
	ResourceList::iterator   res = _resources.begin();
	IResourceList::iterator iRes = _iResources.begin();
	for (; (res != _resources.end()) && (iRes != _iResources.end()); ++index, ++res, ++iRes) {
		res->index = index;

		res->hash = herf.readUint32LE();

		iRes->size   = herf.readUint32LE();
		iRes->offset = herf.readUint32LE();

		if (iRes->offset >= (uint32)herf.size())
			throw Common::Exception("HERFFile::readResList(): Resource goes beyond end of file");

		if (res->hash == dictHash)
			_dictIndex = index;
	}
}

void HERFFile::getDictionary(std::list<uint32> &hashes, std::list<Common::UString> &names) const {
	hashes.clear();
	names.clear();

	if (_dictIndex == 0xFFFFFFFF)
		return;

	Common::SeekableReadStream *dict = getResource(_dictIndex);

	dict->skip(8); // unknown

	while (dict->pos() < dict->size()) {
		uint32 hash = dict->readUint32LE();
		if (hash == 0)
			break;

		hashes.push_back(hash);
		names.push_back("");

		names.back().readFixedASCII(*dict, 128);
		names.back().tolower();
	}

	delete dict;
}

const Archive::ResourceList &HERFFile::getResources() const {
	return _resources;
}

const HERFFile::IResource &HERFFile::getIResource(uint32 index) const {
	if (index >= _iResources.size())
		throw Common::Exception("Resource index out of range (%d/%d)", index, _iResources.size());

	return _iResources[index];
}

uint32 HERFFile::getResourceSize(uint32 index) const {
	return getIResource(index).size;
}

Common::SeekableReadStream *HERFFile::getResource(uint32 index) const {
	const IResource &res = getIResource(index);
	if (res.size == 0)
		return new Common::MemoryReadStream(0, 0);

	Common::SeekableReadStream *herf = ResMan.getResource(TypeMan.setFileType(_fileName, kFileTypeNone), kFileTypeHERF);
	if (!herf)
		throw Common::Exception(Common::kOpenError);

	if (!herf->seek(res.offset)) {
		delete herf;
		throw Common::Exception(Common::kSeekError);
	}

	Common::SeekableReadStream *resStream = herf->readStream(res.size);

	if (!resStream || (((uint32) resStream->size()) != res.size)) {
		delete herf;
		delete resStream;
		throw Common::Exception(Common::kReadError);
	}

	delete herf;
	return resStream;
}

Common::HashAlgo HERFFile::getNameHashAlgo() const {
	return Common::kHashDJB2;
}

} // End of namespace Aurora
