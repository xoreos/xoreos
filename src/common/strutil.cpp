/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file common/strutil.cpp
 *  Utility templates and functions for working with strings.
 */

#include <cstdio>
#include <cctype>

#include "common/strutil.h"
#include "common/util.h"
#include "common/error.h"
#include "common/stream.h"

namespace Common {

void replaceAll(std::string &str, char what, char with) {
	int pos = 0;

	while ((pos = str.find(what, pos)) != (int) std::string::npos)
		str[pos++] = with;
}

void printDataHex(Common::SeekableReadStream &stream) {
	uint32 pos  = stream.pos();
	uint32 size = stream.size() - pos;

	if (size == 0)
		return;

	uint32 offset = 0;
	byte rowData[16];

	while (size > 0) {
		uint32 n = MIN<uint32>(size, 16);

		std::fprintf(stderr, "%08X  ", offset);

		if (stream.read(rowData, n) != n)
			throw Common::Exception(Common::kReadError);

		for (uint32 i = 0; i < 2; i++) {
			for (uint32 j = 0; j < 8; j++) {
				uint32 m = i * 8 + j;

				if (m < n)
					std::fprintf(stderr, "%02X ", rowData[m]);
				else
					std::fprintf(stderr, "   ");
			}

			std::fprintf(stderr, " ");
		}

		std::fprintf(stderr, "|");

		for (uint32 i = 0; i < n; i++)
			std::fprintf(stderr, "%c", std::isprint(rowData[i]) ? rowData[i] : '.');

		std::fprintf(stderr, "|\n");

		size   -= n;
		offset += n;
	}

	if (!stream.seek(pos))
		throw Common::Exception(Common::kSeekError);
}

void printDataHex(const byte *data, uint32 size) {
	if (!data || (size == 0))
		return;

	Common::MemoryReadStream stream(data, size);
}

} // End of namespace Common
