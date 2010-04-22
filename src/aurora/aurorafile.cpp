/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file aurora/aurorafile.cpp
 *  Utility class for handling special data structures found in BioWare's Aurora files.
 */

#include "common/stream.h"
#include "common/strutil.h"
#include "common/ustring.h"

#include "aurora/aurorafile.h"

namespace Aurora {

union floatConvert {
	uint32 dInt;
	float dFloat;
};

float AuroraFile::convertFloat(uint32 data) {
	// We just directly convert here because most systems have float in IEEE 754-1985
	// format anyway. However, should we find another system that has this differently,
	// we might have to do something more here...

	floatConvert conv;

	conv.dInt = data;

	return conv.dFloat;
}

union doubleConvert {
	uint64 dInt;
	double dDouble;
};

double AuroraFile::convertDouble(uint64 data) {
	// We just directly convert here because most systems have double in IEE 754-1985
	// format anyway. However, should we find another system that has this differently,
	// we might have to do something more here...

	doubleConvert conv;

	conv.dInt = data;

	return conv.dDouble;
}

float AuroraFile::readFloat(Common::SeekableReadStream &stream) {
	return convertFloat(stream.readUint32LE());
}

double AuroraFile::readDouble(Common::SeekableReadStream &stream) {
	return convertDouble(stream.readUint64LE());
}

void AuroraFile::cleanupPath(Common::UString &path) {
	path.replaceAll('\\', '/');
}


AuroraBase::AuroraBase() {
	clear();
}

void AuroraBase::clear() {
	_id      = 0;
	_version = 0;
	_utf16le = false;
}

uint32 AuroraBase::getID() const {
	return _id;
}

uint32 AuroraBase::getVersion() const {
	return _version;
}

bool AuroraBase::isUTF16LE() const {
	return _utf16le;
}

void AuroraBase::readHeader(Common::SeekableReadStream &stream) {
	_id      = stream.readUint32BE();
	_version = stream.readUint32BE();

	if (((_id & 0x00FF00FF) == 0) && ((_version & 0x00FF00FF) == 0)) {
		// There's 0-bytes in the ID and version, this looks like little-endian UTF-16

		_utf16le = true;

		_id = convertUTF16LE(_id, _version);

		uint32 version1 = stream.readUint32BE();
		uint32 version2 = stream.readUint32BE();

		_version = convertUTF16LE(version1, version2);
	} else
		_utf16le = false;
}

uint32 AuroraBase::convertUTF16LE(uint32 x1, uint32 x2) {
	// Take 8 byte and remove every second byte

	return ((x1 & 0xFF000000)      ) | ((x1 & 0x0000FF00) << 8) |
	       ((x2 & 0xFF000000) >> 16) | ((x2 & 0x0000FF00) >> 8);
}

} // End of namespace Aurora
