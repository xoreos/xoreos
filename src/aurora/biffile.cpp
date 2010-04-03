/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file aurora/biffile.cpp
 *  Handling BioWare's BIFs (resource data files).
 */

#include "common/stream.h"
#include "common/util.h"

#include "aurora/biffile.h"

static const uint32 kBIFID     = MKID_BE('BIFF');
static const uint32 kVersion1  = MKID_BE('V1  ');
static const uint32 kVersion11 = MKID_BE('V1.1');

namespace Aurora {

BIFFile::BIFFile() {
}

BIFFile::~BIFFile() {
}

void BIFFile::clear() {
	AuroraBase::clear();

	_resources.clear();
}

bool BIFFile::load(Common::SeekableReadStream &bif) {
	clear();

	readHeader(bif);

	if (_id != kBIFID) {
		warning("BIFFile::load(): Not a BIF file");
		return false;
	}

	if ((_version != kVersion1) && (_version != kVersion11)) {
		warning("BIFFile::load(): Unsupported file version");
		return false;
	}

	uint32 varResCount = bif.readUint32LE();
	uint32 fixResCount = bif.readUint32LE();

	if (fixResCount != 0) {
		warning("BIFFile::load(): TODO: Fixed resources");
		return false;
	}

	_resources.resize(varResCount);

	uint32 offVarResTable = bif.readUint32LE();
	if (!readVarResTable(bif, offVarResTable))
		return false;

	if (bif.err()) {
		warning("BIFFile::load(): Read error");
		return false;
	}

	return true;
}

bool BIFFile::readVarResTable(Common::SeekableReadStream &bif, uint32 offset) {
	if (!bif.seek(offset))
		return false;

	for (ResourceList::iterator res = _resources.begin(); res != _resources.end(); ++res) {
		bif.skip(4); // ID

		if (_version == kVersion11)
			bif.skip(4); // Flags

		res->offset = bif.readUint32LE();
		res->size   = bif.readUint32LE();
		res->type   = (FileType) bif.readUint32LE();
	}

	return true;
}

const BIFFile::ResourceList &BIFFile::getResources() const {
	return _resources;
}

} // End of namespace Aurora
