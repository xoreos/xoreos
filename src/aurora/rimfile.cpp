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

RIMFile::RIMFile() {
}

RIMFile::~RIMFile() {
}

void RIMFile::clear() {
	AuroraBase::clear();

	_resources.clear();
}

void RIMFile::load(Common::SeekableReadStream &rim) {
	clear();

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

	for (ResourceList::iterator res = _resources.begin(); res != _resources.end(); ++res) {
		res->name.readASCII(rim, 16);
		res->type   = (FileType) rim.readUint16LE();
		rim.skip(4 + 2); // Resource ID + Reserved
		res->offset = rim.readUint32LE();
		res->size   = rim.readUint32LE();
	}
}

const RIMFile::ResourceList &RIMFile::getResources() const {
	return _resources;
}

} // End of namespace Aurora
