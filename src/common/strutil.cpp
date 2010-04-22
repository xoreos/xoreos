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

#include <cstdarg>
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

std::string sprintf(const char *s, ...) {
	char buf[STRINGBUFLEN];
	va_list va;

	va_start(va, s);
	std::vsnprintf(buf, STRINGBUFLEN, s, va);
	va_end(va);

	return buf;
}

void printDataHex(SeekableReadStream &stream) {
	uint32 pos  = stream.pos();
	uint32 size = stream.size() - pos;

	if (size == 0)
		return;

	uint32 offset = 0;
	byte rowData[16];

	while (size > 0) {
		// At max 16 bytes printed per row
		uint32 n = MIN<uint32>(size, 16);
		if (stream.read(rowData, n) != n)
			throw Exception(kReadError);

		// Print an offset
		std::fprintf(stderr, "%08X  ", offset);

		// 2 "blobs" of each 8 bytes per row
		for (uint32 i = 0; i < 2; i++) {
			for (uint32 j = 0; j < 8; j++) {
				uint32 m = i * 8 + j;

				if (m < n)
					// Print the data
					std::fprintf(stderr, "%02X ", rowData[m]);
				else
					// Last row, data count not aligned to 16
					std::fprintf(stderr, "   ");
			}

			// Separate the blobs by an extra space
			std::fprintf(stderr, " ");
		}

		std::fprintf(stderr, "|");

		// If the data byte is a printable character, print it. If not, substitute a '.'
		for (uint32 i = 0; i < n; i++)
			std::fprintf(stderr, "%c", std::isprint(rowData[i]) ? rowData[i] : '.');

		std::fprintf(stderr, "|\n");

		size   -= n;
		offset += n;
	}

	// Seek back
	if (!stream.seek(pos))
		throw Exception(kSeekError);
}

void printDataHex(const byte *data, uint32 size) {
	if (!data || (size == 0))
		return;

	MemoryReadStream stream(data, size);
}

} // End of namespace Common
