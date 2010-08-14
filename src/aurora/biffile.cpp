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

#include "common/util.h"
#include "common/error.h"
#include "common/stream.h"
#include "common/file.h"

#include "aurora/biffile.h"
#include "aurora/keyfile.h"
#include "aurora/error.h"

static const uint32 kBIFID     = MKID_BE('BIFF');
static const uint32 kVersion1  = MKID_BE('V1  ');
static const uint32 kVersion11 = MKID_BE('V1.1');

namespace Aurora {

BIFFile::BIFFile(const Common::UString &fileName) : _fileName(fileName) {
	load();
}

BIFFile::~BIFFile() {
}

void BIFFile::clear() {
	_resources.clear();
}

void BIFFile::load() {
	Common::File bif;
	open(bif);

	readHeader(bif);

	if (_id != kBIFID)
		throw Common::Exception("Not a BIF file");

	if ((_version != kVersion1) && (_version != kVersion11))
		throw Common::Exception("Unsupported BIF file version %08X", _version);

	uint32 varResCount = bif.readUint32LE();
	uint32 fixResCount = bif.readUint32LE();

	if (fixResCount != 0)
		throw Common::Exception("TODO: Fixed BIF resources");

	_iResources.resize(varResCount);

	uint32 offVarResTable = bif.readUint32LE();

	try {

		readVarResTable(bif, offVarResTable);

		if (bif.err())
			throw Common::Exception(Common::kReadError);

	} catch (Common::Exception &e) {
		e.add("Failed reading BIF file");
		throw e;
	}

}

void BIFFile::readVarResTable(Common::SeekableReadStream &bif, uint32 offset) {
	if (!bif.seek(offset))
		throw Common::Exception(Common::kSeekError);

	for (IResourceList::iterator res = _iResources.begin(); res != _iResources.end(); ++res) {
		bif.skip(4); // ID

		if (_version == kVersion11)
			bif.skip(4); // Flags

		res->offset = bif.readUint32LE();
		res->size   = bif.readUint32LE();
		res->type   = (FileType) bif.readUint32LE();
	}
}

void BIFFile::mergeKEY(const KEYFile &key, uint32 bifIndex) {
	const KEYFile::ResourceList &keyResList = key.getResources();

	for (KEYFile::ResourceList::const_iterator keyRes = keyResList.begin(); keyRes != keyResList.end(); ++keyRes) {
		if (keyRes->bifIndex != bifIndex)
			continue;

		if (keyRes->resIndex >= _iResources.size())
			throw Common::Exception("Resource index out of range (%d/%d)", keyRes->resIndex, _iResources.size());

		if (keyRes->type != _iResources[keyRes->resIndex].type)
			throw Common::Exception("Type mismatch while merging KEY with BIF (%d, %d)",
					keyRes->type, _iResources[keyRes->resIndex].type);

		Resource res;

		res.name  = keyRes->name;
		res.type  = keyRes->type;
		res.index = keyRes->resIndex;

		_resources.push_back(res);
	}

}

const Archive::ResourceList &BIFFile::getResources() const {
	return _resources;
}

Common::SeekableReadStream *BIFFile::getResource(uint32 index) const {
	if (index >= _iResources.size())
		throw Common::Exception("Resource index out of range (%d/%d)", index, _iResources.size());

	Common::File bif;
	open(bif);

	const IResource &res = _iResources[index];
	if (res.size == 0)
		return new Common::MemoryReadStream(0, 0);

	if (!bif.seek(res.offset))
		throw Common::Exception(Common::kSeekError);

	Common::SeekableReadStream *resStream = bif.readStream(res.size);

	if (!resStream || (((uint32) resStream->size()) != res.size)) {
		delete resStream;
		throw Common::Exception(Common::kReadError);
	}

	return resStream;
}

void BIFFile::open(Common::File &file) const {
	if (!file.open(_fileName))
		throw Common::Exception(Common::kOpenError);
}

} // End of namespace Aurora
