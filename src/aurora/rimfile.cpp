/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file aurora/rimfile.cpp
 *  Handling BioWare's RIMs (resource archives).
 */

#include "common/stream.h"
#include "common/util.h"

#include "aurora/rimfile.h"
#include "aurora/error.h"

static const uint32 kRIMID     = MKID_BE('RIM ');
static const uint32 kVersion1  = MKID_BE('V1.0');

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
		throw e;
	}

}

void RIMFile::readResList(Common::SeekableReadStream &rim, uint32 offset) {
	if (!rim.seek(offset))
		throw Common::Exception(Common::kSeekError);

	uint32 index = 0;
	ResourceList::iterator   res = _resources.begin();
	IResourceList::iterator iRes = _iResources.begin();
	for (; (res != _resources.end()) && (iRes != _iResources.end()); ++index, ++res, ++iRes) {
		res->name.readASCII(rim, 16);
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

Common::SeekableReadStream *RIMFile::getResource(uint32 index) const {
	if (index >= _iResources.size())
		throw Common::Exception("Resource index out of range (%d/%d)", index, _iResources.size());

	Common::File rim;
	open(rim);

	const IResource &res = _iResources[index];

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
