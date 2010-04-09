/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file aurora/erffile.cpp
 *  Handling BioWare's ERFs (encapsulated resource file).
 */

#include "common/stream.h"
#include "common/util.h"

#include "aurora/erffile.h"
#include "aurora/error.h"

static const uint32 kERFID     = MKID_BE('ERF ');
static const uint32 kMODID     = MKID_BE('MOD ');
static const uint32 kHAKID     = MKID_BE('HAK ');
static const uint32 kSAVID     = MKID_BE('SAV ');
static const uint32 kVersion1  = MKID_BE('V1.0');

namespace Aurora {

ERFFile::ERFFile() {
}

ERFFile::~ERFFile() {
}

void ERFFile::clear() {
	AuroraBase::clear();

	_description.clear();
	_resources.clear();
}

void ERFFile::load(Common::SeekableReadStream &erf) {
	clear();

	readHeader(erf);

	if ((_id != kERFID) && (_id != kMODID) && (_id != kHAKID) && (_id != kSAVID))
		throw Common::Exception("Not an ERF file");

	if (_version != kVersion1)
		throw Common::Exception("Unsupported ERF file version %08X", _version);

	uint32 langCount = erf.readUint32LE(); // Number of languages for the description
	erf.skip(4);                           // Number of bytes in the description
	uint32 resCount  = erf.readUint32LE(); // Number of resources in the ERF

	uint32 offDescription = erf.readUint32LE();
	uint32 offKeyList     = erf.readUint32LE();
	uint32 offResList     = erf.readUint32LE();

	erf.skip(4 + 4); // Build year and day

	uint32 descriptionID = erf.readUint32LE();

	erf.skip(116); // Reserved

	try {

		// Read description string(s)
		readDescription(erf, offDescription, descriptionID, langCount);

		_resources.resize(resCount);

		// Read name and type part of the resource list
		readKeyList(erf, offKeyList);

		// Read offset and size part of the resource list
		readResList(erf, offResList);

		if (erf.err())
			throw Common::Exception(kReadError);

	} catch (Common::Exception &e) {
		e.add("Failed reading ERF file");
		throw e;
	}

}

void ERFFile::readDescription(Common::SeekableReadStream &erf, uint32 offset, uint32 id, uint32 count) {
	if (!erf.seek(offset))
		throw Common::Exception(kSeekError);

	_description.readLocString(erf, id, count);
}

void ERFFile::readKeyList(Common::SeekableReadStream &erf, uint32 offset) {
	if (!erf.seek(offset))
		throw Common::Exception(kSeekError);

	for (ResourceList::iterator res = _resources.begin(); res != _resources.end(); ++res) {
		res->name = AuroraFile::readRawString(erf, 16);
		erf.skip(4); // Resource ID
		res->type = (FileType) erf.readUint16LE();
		erf.skip(2); // Reserved
	}
}

void ERFFile::readResList(Common::SeekableReadStream &erf, uint32 offset) {
	if (!erf.seek(offset))
		throw Common::Exception(kSeekError);

	for (ResourceList::iterator res = _resources.begin(); res != _resources.end(); ++res) {
		res->offset = erf.readUint32LE();
		res->size   = erf.readUint32LE();
	}
}

const LocString &ERFFile::getDescription() const {
	return _description;
}

const ERFFile::ResourceList &ERFFile::getResources() const {
	return _resources;
}

} // End of namespace Aurora
