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

#include "aurora/aurorafile.h"

namespace Aurora {

std::string AuroraFile::readRawString(Common::SeekableReadStream &stream, uint32 length) {
	char buf[length];

	if (stream.read(buf, length) != length)
		return "";

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

} // End of namespace Aurora
