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

#include "aurora/biffile.h"
#include "aurora/aurorafile.h"

static const uint32 kBifID    = MKID_BE('BIFF');
static const uint32 kVersion1 = MKID_BE('V1  ');

namespace Aurora {

BifFile::BifFile() {
}

BifFile::~BifFile() {
}

void BifFile::clear() {
	_resources.clear();
}

bool BifFile::load(Common::SeekableReadStream &bif) {
	if (bif.readUint32BE() != kBifID) {
		warning("BifFile::load(): Not a KEY file");
		return false;
	}

	// TODO: Version 1.1. Found in The Witcher and not directly compatible with Version 1
	if (bif.readUint32BE() != kVersion1) {
		warning("BifFile::load(): Unsupported file version");
		return false;
	}

	uint32 varResCount = bif.readUint32LE();
	uint32 fixResCount = bif.readUint32LE();

	if (fixResCount != 0) {
		warning("BifFile::load(): TODO: Fixed resources");
		return false;
	}

	_resources.reserve(varResCount);

	uint32 offVarResTable = bif.readUint32LE();

	bif.seek(offVarResTable);
	if (!readVarResTable(bif, varResCount))
		return false;

	if (bif.err()) {
		warning("BifFile::load(): Read error");
		return false;
	}

	return true;
}

bool BifFile::readVarResTable(Common::SeekableReadStream &bif, uint32 varResCount) {
	for (uint32 i = 0; i < varResCount; i++) {
		Resource resource;

		bif.skip(4); // ID

		resource.offset = bif.readUint32LE();
		resource.size   = bif.readUint32LE();
		resource.type   = (FileType) bif.readUint32LE();

		_resources.push_back(resource);
	}

	return true;
}

const BifFile::ResourceList &BifFile::getResources() const {
	return _resources;
}

} // End of namespace Aurora
