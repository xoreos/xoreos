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

#include <cassert>
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

	assert((iBits + fBits + (sign ? 1 : 0)) <= 32);

	// Masks for the integer, fractional and sign parts
	const uint32 fMask =  (UINT64_C(1) <<          fBits)  - 1;
	const uint32 iMask = ((UINT64_C(1) << (iBits + fBits)) - 1) - fMask;
	const uint32 sMask =   UINT64_C(1) << (iBits + fBits);

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

/* .--- Convert IEEE float16 to IEEE float32, based on code by James Tursa ---.
 *
 * This function extends a 16-bit (half-precision) IEEE 754 floating-point
 * number into a 32-bit (full-precision) IEEE 754 float-point number.
 *
 * The special cases of -Inf, Inf and NaN are handled correctly.
 * Denormalized float16 numbers are adjusted to normalized float32 numbers.
 *
 * This code is heavily based on the halfp2singles() function found in
 * ieeehalfprecision.c by James Tursa, released under the terms of the
 * 2-Clause BSD license as part of the "IEEE 754r Half Precision floating
 * point converter" MATLAB package.
 * (<https://www.mathworks.com/matlabcentral/fileexchange/23173-ieee-754r-half-precision-floating-point-converter>)
 */
float readIEEEFloat16(uint16 value) {
	// Check for 0.0 / -0.0
	if ((value & 0x7FFF) == 0)
		return convertIEEEFloat(((uint32) value) << 16);

	uint16 vS = value & 0x8000; // float16 sign
	uint16 vE = value & 0x7C00; // float16 exponent
	uint16 vM = value & 0x03FF; // float16 mantissa

	const uint32 fS = ((uint32) vS) << 16; // float32 sign

	// Check for (-)Inf and NaN
	if (vE == 0x7C00) {
		// All exponent bits are set and the mantissa is 0: Inf / -Inf
		if (vM == 0)
			return convertIEEEFloat(0x7F800000 | fS);

		// All exponent bits are set and the mantissa is != 0: NaN
		return convertIEEEFloat(0xFFC00000);
	}

	// Not zero, infinity or NaN: this is a regular number

	// Unbias the float16 exponent, and bias the float32 exponent accordingly
	int32 fER = ((int32) (vE >> 10)) - 15 + 127;

	// Is this float normalized? If so, we can directly extend it
	if (vE != 0) {
		const uint32 fE = ((uint32) fER) << 23; // float32 exponent, rebiased
		const uint32 fM = ((uint32) vM ) << 13; // float32 mantissa

		// Combine sign, exponent and mantissa
		return convertIEEEFloat(fS | fE | fM);
	}

	// Denormalized float, we need to normalize it first

	// Shift the mantissa until it overflows, to find out the exponent adjustment
	int aE = -1;
	do {
		aE++;
		vM <<= 1;
	} while ((vM & 0x0400) == 0);

	// Adjust the exponent, and clip the shifted mantissa to valid range again
	fER -= aE;
	vM  &= 0x03FF;

	const uint32 fE = ((uint32) fER) << 23; // float32 exponent, rebiased and normalized
	const uint32 fM = ((uint32) vM ) << 13; // float32 mantissa, normalized

	// Combine sign, exponent and mantissa
	return convertIEEEFloat(fS | fE | fM);
}
// '--- Convert IEEE float16 to IEEE float32, based on code by James Tursa ---'
