/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file common/util.cpp
 *  Utility templates and functions.
 */

#include "common/util.h"

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cctype>

#ifndef DISABLE_TEXT_CONSOLE

void warning(const char *s, ...) {
	char buf[STRINGBUFLEN];
	va_list va;

	va_start(va, s);
	std::vsnprintf(buf, STRINGBUFLEN, s, va);
	va_end(va);

	std::fputs("WARNING: ", stderr);
	std::fputs(buf, stderr);
	std::fputs("!\n", stderr);
}

void status(const char *s, ...) {
	char buf[STRINGBUFLEN];
	va_list va;

	va_start(va, s);
	std::vsnprintf(buf, STRINGBUFLEN, s, va);
	va_end(va);

	std::fputs(buf, stderr);
	std::fputs("\n", stderr);
}
#endif

void NORETURN_PRE error(const char *s, ...) {
	char buf[STRINGBUFLEN];
	va_list va;

	va_start(va, s);
	std::vsnprintf(buf, STRINGBUFLEN, s, va);
	va_end(va);

	std::fputs("ERROR: ", stderr);
	std::fputs(buf, stderr);
	std::fputs("!\n", stderr);

	std::exit(1);
}

void printDataHex(const byte *data, uint32 size) {
	if (!data || (size == 0))
		return;

	uint32 offset = 0;

	while (size > 0) {
		uint32 n = MIN<uint32>(size, 16);

		std::fprintf(stderr, "%08X  ", offset);

		for (uint32 i = 0; i < 2; i++) {
			for (uint32 j = 0; j < 8; j++) {
				uint32 m = i * 8 + j;

				if (m < n)
					std::fprintf(stderr, "%02X ", data[m]);
				else
					std::fprintf(stderr, "   ");
			}

			std::fprintf(stderr, " ");
		}

		std::fprintf(stderr, "|");

		for (uint32 i = 0; i < n; i++)
			std::fprintf(stderr, "%c", std::isprint(data[i]) ? data[i] : '.');

		std::fprintf(stderr, "|\n");

		data   += n;
		size   -= n;
		offset += n;
	}

}
