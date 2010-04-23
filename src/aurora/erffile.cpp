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
#include "aurora/util.h"

static const uint32 kERFID     = MKID_BE('ERF ');
static const uint32 kMODID     = MKID_BE('MOD ');
static const uint32 kHAKID     = MKID_BE('HAK ');
static const uint32 kSAVID     = MKID_BE('SAV ');
static const uint32 kVersion1  = MKID_BE('V1.0');
static const uint32 kVersion2  = MKID_BE('V2.0');

namespace Aurora {

ERFFile::ERFFile() : _langCount(0), _descriptionID(0), _offDescription(0), _offKeyList(0), _offResList(0) {
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

	if ((_version != kVersion1) && (_version != kVersion2))
		throw Common::Exception("Unsupported ERF file version %08X", _version);

	if ((_version == kVersion2) && !_utf16le)
		throw Common::Exception("ERF file version 2.0, but not UTF-16LE");

	try {

		readERFHeader(erf);
		readDescription(erf);
		readResources(erf);

		if (erf.err())
			throw Common::Exception(Common::kReadError);

	} catch (Common::Exception &e) {
		e.add("Failed reading ERF file");
		throw e;
	}

}

void ERFFile::readERFHeader(Common::SeekableReadStream &erf) {
	uint32 resCount = 0;

	if        (_version == kVersion1) {

		_langCount = erf.readUint32LE(); // Number of languages for the description
		erf.skip(4);                     // Number of bytes in the description
		resCount   = erf.readUint32LE(); // Number of resources in the ERF

		_offDescription = erf.readUint32LE();
		_offKeyList     = erf.readUint32LE();
		_offResList     = erf.readUint32LE();

		erf.skip(4 + 4); // Build year and day

		_descriptionID = erf.readUint32LE();

		erf.skip(116); // Reserved

	} else if (_version == kVersion2) {

		_langCount = 0;                  // No description in ERF V2.0
		resCount   = erf.readUint32LE(); // Number of resources in the ERF

		_descriptionID  = 0;    // No description in ERF V2.0
		_offDescription = 0;    // No description in ERF V2.0
		_offKeyList     = 0;    // No separate key list in ERF V2.0
		_offResList     = 0x20; // Resource list always starts at 0x20 in ERF V2.0

		erf.skip(4 + 4); // Build year and day
		erf.skip(4);     // Unknown, always 0xFFFFFFFF?
	}

	_resources.resize(resCount);
}

void ERFFile::readDescription(Common::SeekableReadStream &erf) {
	if (_version == kVersion1) {
		if (!erf.seek(_offDescription))
			throw Common::Exception(Common::kSeekError);

		_description.readLocString(erf, _descriptionID, _langCount);
	}
}

void ERFFile::readResources(Common::SeekableReadStream &erf) {
	if        (_version == kVersion1) {

		readV1KeyList(erf); // Read name and type part of the resource list
		readV1ResList(erf); // Read offset and size part of the resource list

	} else if (_version == kVersion2) {

		// Read the resource list
		readV2ResList(erf);

	}

}

void ERFFile::readV1KeyList(Common::SeekableReadStream &erf) {
	if (!erf.seek(_offKeyList))
		throw Common::Exception(Common::kSeekError);

	for (ResourceList::iterator res = _resources.begin(); res != _resources.end(); ++res) {
		res->name.readASCII(erf, 16);
		erf.skip(4); // Resource ID
		res->type = (FileType) erf.readUint16LE();
		erf.skip(2); // Reserved
	}
}

void ERFFile::readV1ResList(Common::SeekableReadStream &erf) {
	if (!erf.seek(_offResList))
		throw Common::Exception(Common::kSeekError);

	for (ResourceList::iterator res = _resources.begin(); res != _resources.end(); ++res) {
		res->offset = erf.readUint32LE();
		res->size   = erf.readUint32LE();
	}
}

void ERFFile::readV2ResList(Common::SeekableReadStream &erf) {
	if (!erf.seek(_offResList))
		throw Common::Exception(Common::kSeekError);

	for (ResourceList::iterator res = _resources.begin(); res != _resources.end(); ++res) {
		Common::UString name;

		name.readUTF16LE(erf, 32);

		res->name = setFileType(name, kFileTypeNone);
		res->type = getFileType(name);

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

Common::SeekableReadStream *ERFFile::getResource(Common::SeekableReadStream &stream,
		uint32 offset, uint32 size) {

	if (!stream.seek(offset))
		return 0;

	byte *data = new byte[size];

	if (stream.read(data, size) != size) {
		delete[] data;
		return 0;
	}

	return new Common::MemoryReadStream(data, size, DisposeAfterUse::YES);
}

} // End of namespace Aurora
