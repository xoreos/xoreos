/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * xoreos is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * xoreos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xoreos. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file
 *  Utility templates and functions.
 */

#include "src/common/util.h"
#include "src/common/debugman.h"

#include <cstdarg>
#include <cstdio>
#include <cstdlib>

void warning(const char *s, ...) {
	char buf[STRINGBUFLEN];
	va_list va;

	va_start(va, s);
	vsnprintf(buf, STRINGBUFLEN, s, va);
	va_end(va);

#ifndef DISABLE_TEXT_CONSOLE
	std::fputs("WARNING: ", stderr);
	std::fputs(buf, stderr);
	std::fputs("!\n", stderr);
#endif

	DebugMan.logString("WARNING: ");
	DebugMan.logString(buf);
	DebugMan.logString("!\n");
}

void status(const char *s, ...) {
	char buf[STRINGBUFLEN];
	va_list va;

	va_start(va, s);
	vsnprintf(buf, STRINGBUFLEN, s, va);
	va_end(va);

#ifndef DISABLE_TEXT_CONSOLE
	std::fputs(buf, stderr);
	std::fputs("\n", stderr);
#endif

	DebugMan.logString(buf);
	DebugMan.logString("\n");
}

void info(const char *s, ...) {
	char buf[STRINGBUFLEN];
	va_list va;

	va_start(va, s);
	vsnprintf(buf, STRINGBUFLEN, s, va);
	va_end(va);

#ifndef DISABLE_TEXT_CONSOLE
	std::fputs(buf, stdout);
	std::fputs("\n", stdout);
#endif

	DebugMan.logString(buf);
	DebugMan.logString("\n");
}

void NORETURN_PRE error(const char *s, ...) {
	char buf[STRINGBUFLEN];
	va_list va;

	va_start(va, s);
	vsnprintf(buf, STRINGBUFLEN, s, va);
	va_end(va);

#ifndef DISABLE_TEXT_CONSOLE
	std::fputs("ERROR: ", stderr);
	std::fputs(buf, stderr);
	std::fputs("!\n", stderr);
#endif

	DebugMan.logString("ERROR: ");
	DebugMan.logString(buf);
	DebugMan.logString("!\n");

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

double readNintendoFixedPoint(uint32 value, bool sign, uint8 iBits, uint8 fBits) {
	/* The Nintendo DS uses fixed point values of various formats. This method can
	 * convert them all into a usual floating point double. */

	// Masks for the integer, fractional and sign parts
	const uint32 fMask =  (1 <<          fBits)  - 1;
	const uint32 iMask = ((1 << (iBits + fBits)) - 1) - fMask;
	const uint32 sMask =   1 << (iBits + fBits);

	// Step of a fractional unit
	const uint32 fDiv  =  (1 <<          fBits);

	// The fractional and integer parts themselves
	int32 fPart =  value & fMask;
	int32 iPart = (value & iMask) >> fBits;

	// If this is a negative value, negate the integer part (which is a two's complement)
	if (sign && ((value & sMask) != 0))
		iPart = -((int32) ((~iPart & (iMask >> fBits)) + 1));

	return (double)iPart + ((double) fPart) / ((double) fDiv);
}
