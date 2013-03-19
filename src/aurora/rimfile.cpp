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

/** @file aurora/rimfile.cpp
 *  Handling BioWare's RIMs (resource archives).
 */

#include "common/stream.h"
#include "common/util.h"

#include "aurora/rimfile.h"
#include "aurora/error.h"

static const uint32 kRIMID     = MKTAG('R', 'I', 'M', ' ');
static const uint32 kVersion1  = MKTAG('V', '1', '.', '0');

namespace Aurora {

RIMFile::RIMFile(const Common::UString &fileName) : _fileName(fileName) {
	load();
}

RIMFile::~RIMFile() {
}

void RIMFile::clear() {
	_resources.clear();
}

void RIMFile::load() {
	Common::File rim;
	open(rim);

	readHeader(rim);

	if (_id != kRIMID)
		throw Common::Exception("Not a RIM file");

	if (_version != kVersion1)
		throw Common::Exception("Unsupported RIM file version %08X", _version);

	rim.skip(4);                            // Reserved
	uint32 resCount   = rim.readUint32LE(); // Number of resources in the RIM
	uint32 offResList = rim.readUint32LE(); // Offset to the resource list

	rim.skip(116); // Reserved

	_resources.resize(resCount);
	_iResources.resize(resCount);

	try {

		// Read the resource list
		readResList(rim, offResList);

	if (rim.err())
		throw Common::Exception(Common::kReadError);

	} catch (Common::Exception &e) {
		e.add("Failed reading RIM file");
		throw;
	}

}

void RIMFile::readResList(Common::SeekableReadStream &rim, uint32 offset) {
	if (!rim.seek(offset))
		throw Common::Exception(Common::kSeekError);

	uint32 index = 0;
	ResourceList::iterator   res = _resources.begin();
	IResourceList::iterator iRes = _iResources.begin();
	for (; (res != _resources.end()) && (iRes != _iResources.end()); ++index, ++res, ++iRes) {
		res->name.readFixedASCII(rim, 16);
		res->type    = (FileType) rim.readUint16LE();
		res->index   = index;
		rim.skip(4 + 2); // Resource ID + Reserved
		iRes->offset = rim.readUint32LE();
		iRes->size   = rim.readUint32LE();
	}
}

const Archive::ResourceList &RIMFile::getResources() const {
	return _resources;
}

const RIMFile::IResource &RIMFile::getIResource(uint32 index) const {
	if (index >= _iResources.size())
		throw Common::Exception("Resource index out of range (%d/%d)", index, _iResources.size());

	return _iResources[index];
}

uint32 RIMFile::getResourceSize(uint32 index) const {
	return getIResource(index).size;
}

Common::SeekableReadStream *RIMFile::getResource(uint32 index) const {
	const IResource &res = getIResource(index);
	if (res.size == 0)
		return new Common::MemoryReadStream(0, 0);

	Common::File rim;
	open(rim);

	if (!rim.seek(res.offset))
		throw Common::Exception(Common::kSeekError);

	Common::SeekableReadStream *resStream = rim.readStream(res.size);

	if (!resStream || (((uint32) resStream->size()) != res.size)) {
		delete resStream;
		throw Common::Exception(Common::kReadError);
	}

	return resStream;
}

void RIMFile::open(Common::File &file) const {
	if (!file.open(_fileName))
		throw Common::Exception(Common::kOpenError);
}

} // End of namespace Aurora
