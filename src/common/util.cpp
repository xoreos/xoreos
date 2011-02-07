/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
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


	// We just directly convert here because most systems have float in IEEE 754-1985
	// format anyway. However, should we find another system that has this differently,
	// we might have to do something more here...
union floatConvert {
	uint32 dInt;
	float dFloat;
};

float convertIEEEFloat(uint32 data) {

	floatConvert conv;

	conv.dInt = data;

	return conv.dFloat;
}

uint32 convertIEEEFloat(float value) {
	floatConvert conv;

	conv.dFloat = value;

	return conv.dInt;
}


	// We just directly convert here because most systems have double in IEEE 754-1985
	// format anyway. However, should we find another system that has this differently,
	// we might have to do something more here...

union doubleConvert {
	uint64 dInt;
	double dDouble;
};

double convertIEEEDouble(uint64 data) {
	doubleConvert conv;

	conv.dInt = data;

	return conv.dDouble;
}

uint64 convertIEEEDouble(double value) {
	doubleConvert conv;

	conv.dDouble = value;

	return conv.dInt;
}
