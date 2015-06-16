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

// Mostly copied verbatim from ScummVM's

/** @file
 *  Utility templates and functions.
 */

#ifndef COMMON_UTIL_H
#define COMMON_UTIL_H

#include "src/common/endianness.h"

#include <cmath>

#ifdef MIN
	#undef MIN
#endif

#ifdef MAX
	#undef MAX
#endif

#ifdef ARRAYSIZE
	#undef ARRAYSIZE
#endif

template<typename T> inline T ABS (T x)      { return (x>=0) ? x : -x; }
template<typename T> inline T MIN (T a, T b) { return (a<b)  ? a :  b; }
template<typename T> inline T MAX (T a, T b) { return (a>b)  ? a :  b; }
template<typename T> inline T CLIP (T v, T amin, T amax)
	{ if (v < amin) return amin; else if (v > amax) return amax; else return v; }

/**
 * Template method which swaps the values of its two parameters.
 */
template<typename T> inline void SWAP(T &a, T &b) { T tmp = a; a = b; b = tmp; }

static inline uint32 NEXTPOWER2(uint32 x) {
	if (x == 0)
		return 1;

	x--;

	x |= x >> 1; x |= x >> 2; x |= x >> 4; x |= x >> 8; x |= x >> 16;

	return x + 1;
}

static inline uint64 NEXTPOWER2(uint64 x) {
	if (x == 0)
		return 1;

	x--;

	x |= x >> 1; x |= x >> 2; x |= x >> 4; x |= x >> 8; x |= x >> 16; x |= x >> 32;

	return x + 1;
}

#ifdef HAVE_FABS
template<> inline double ABS(double x) { return fabs(x); }
#endif
#ifdef HAVE_FABSF
template<> inline float ABS(float x) { return fabsf(x); }
#endif

#ifdef HAVE_FMIN
template<> inline double MIN(double a, double b) { return fmin(a, b); }
#endif
#ifdef HAVE_FMINF
template<> inline float MIN(float a, float b) { return fminf(a, b); }
#endif

#ifdef HAVE_FMAX
template<> inline double MAX(double a, double b) { return fmax(a, b); }
#endif
#ifdef HAVE_FMAXF
template<> inline float MAX(float a, float b) { return fmaxf(a, b); }
#endif

/**
 * Macro which determines the number of entries in a fixed size array.
 */
#define ARRAYSIZE(x) (sizeof(x) / sizeof(x[0]))

/**
* Print a warning message to the text console (stderr).
* Automatically prepends the text "WARNING: " and appends
* an exclamation mark and a newline.
*/
void warning(const char *s, ...) GCC_PRINTF(1, 2);
/**
* Print a status message to the text console (stderr).
* Automatically appends a newline.
*/
void status(const char *s, ...) GCC_PRINTF(1, 2);
/**
* Print an info message to the text console (stdout).
* Automatically appends a newline.
*/
void info(const char *s, ...) GCC_PRINTF(1, 2);

void NORETURN_PRE error(const char *s, ...) GCC_PRINTF(1, 2) NORETURN_POST;

float  convertIEEEFloat(uint32 data);
double convertIEEEDouble(uint64 data);

uint32 convertIEEEFloat(float value);
uint64 convertIEEEDouble(double value);

/** Read a fixed-point value, in a format used by the Nintendo DS.
 *
 *  @param  value The integer representing the fixed-point value.
 *  @param  sign Is this a signed fixed-point value?
 *  @param  iBits Number of bits in the integer part.
 *  @param  fBits Number of bits in the fractional part.
 *  @return A floating-point representation of the fixed-point value.
 */
double readNintendoFixedPoint(uint32 value, bool sign, uint8 iBits, uint8 fBits);

/** Read a half-precision 16-bit IEEE float, converting it into a 32-bit iEEE float. */
float readIEEEFloat16(uint16 value);

#endif // COMMON_UTIL_H
