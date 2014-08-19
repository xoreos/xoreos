/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
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
	printDataHex(stream);
}

} // End of namespace Common
