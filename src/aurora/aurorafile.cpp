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
#include "common/util.h"

#include "aurora/aurorafile.h"

namespace Aurora {

std::string AuroraFile::readRawString(Common::SeekableReadStream &stream, uint32 length) {
	char buf[length + 1];

	if (stream.read(buf, length) != length)
		return "";

	buf[length] = '\0';

	return buf;
}

std::string AuroraFile::readRawString(Common::SeekableReadStream &stream, uint32 length, uint32 offset) {
	int32 pos = stream.pos();

	if (pos == -1)
		return "";

	if (!stream.seek(offset)) {
		stream.seek(pos);
		return "";
	}

	std::string str = readRawString(stream, length);

	stream.seek(pos);

	return str;
}

float AuroraFile::convertFloat(uint32 data) {
	// We just cast here because most systems have float in 754-1985 format anyway.
	// However, should we find another system that has this differently, we might
	// have to do something more here...

	return *reinterpret_cast<float *>(&data);
}

double AuroraFile::convertDouble(uint64 data) {
	// We just cast here because most systems have double in 754-1985 format anyway.
	// However, should we find another system that has this differently, we might
	// have to do something more here...

	return *reinterpret_cast<double *>(&data);
}

float AuroraFile::readFloat(Common::SeekableReadStream &stream) {
	return convertFloat(stream.readUint32LE());
}

double AuroraFile::readDouble(Common::SeekableReadStream &stream) {
	return convertDouble(stream.readUint64LE());
}

void AuroraFile::cleanupPath(std::string &path) {
	Common::replaceAll(path, '\\', '/');
}

} // End of namespace Aurora
