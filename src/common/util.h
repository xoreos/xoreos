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

/* Based on ScummVM (<http://scummvm.org>) code, which is released
 * under the terms of version 2 or later of the GNU General Public
 * License.
 *
 * The original copyright note in ScummVM reads as follows:
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
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

/** Is this integer value a power of 2? */
template<typename T> inline bool ISPOWER2(T x) { return x && !(x & (x - 1)); }

/** Round up to the next power of 2. */
static inline uint32_t NEXTPOWER2(uint32_t x) {
	if (x == 0)
		return 1;

	x--;

	x |= x >> 1; x |= x >> 2; x |= x >> 4; x |= x >> 8; x |= x >> 16;

	return x + 1;
}

/** Round up to the next power of 2. */
static inline uint64_t NEXTPOWER2(uint64_t x) {
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
 * Determine the number of entries in a fixed size array.
 */
template<typename T, std::size_t N>
constexpr std::size_t ARRAYSIZE(const T (&)[N]) {
	return N;
}

/** Print a warning message to both stderr and the global
 *  log file (if a global log file has been opened).
 *  See Common::DebugManager for details.
 *
 *  Automatically prepends the text "WARNING: " and appends
 *  an exclamation mark and a newline.
 */
void warning(const char *s, ...) GCC_PRINTF(1, 2);
/** Print a status message to both stderr and the global
 *  log file (if a global log file has been opened).
 *  See Common::DebugManager for details.
 *
 *  Automatically appends a newline.
 */
void status(const char *s, ...) GCC_PRINTF(1, 2);
/** Print an info message to both stdout and the global
 *  log file (if a global log file has been opened).
 *  See Common::DebugManager for details.
 *
 *  Automatically appends a newline.
 */
void info(const char *s, ...) GCC_PRINTF(1, 2);

/** Print an error message to both stderr and the global
 *  log file (if a global log file has been opened).
 *  See Common::DebugManager for details.
 *
 *  Automatically prepends the text "ERROR: " and appends
 *  an exclamation mark and a newline.
 *
 *  Additionally, the program will immediately quit with
 *  return code 1.
 */
[[noreturn]] void error(const char *s, ...) GCC_PRINTF(1, 2);

/** Convert a uint32_t holding the bit pattern of a 32-bit IEEE 754 single
 *  precision floating point value into a real, native float.
 *
 *  NOTE: Currently, this function assumes native floats are 32-bit IEEE
 *  754 single precision floating point values!
 */
float convertIEEEFloat(uint32_t data);
/** Convert a uint64_t holding the bit pattern of a 64-bit IEEE 754 double
 *  precision floating point value into a real, native double.
 *
 *  NOTE: Currently, this function assumes native doubles are 64-bit IEEE
 *  754 double precision floating point values!
 */
double convertIEEEDouble(uint64_t data);

/** Convert a native float into a uint32_t holding the bit pattern a 32-bit
 *  IEEE 754 single precision floating point value.
 *
 *  NOTE: Currently, this function assumes native floats are 32-bit IEEE
 *  754 single precision floating point values!
 */
uint32_t convertIEEEFloat(float value);
/** Convert a native double into a uint64_t holding the bit pattern a 64-bit
 *  IEEE 754 double precision floating point value.
 *
 *  NOTE: Currently, this function assumes native doubles are 64-bit IEEE
 *  754 double precision floating point values!
 */
uint64_t convertIEEEDouble(double value);

/** Read a fixed-point value, in a format used by the Nintendo DS.
 *
 *  @param  value The integer representing the fixed-point value.
 *  @param  sign Is this a signed fixed-point value?
 *  @param  iBits Number of bits in the integer part.
 *  @param  fBits Number of bits in the fractional part.
 *  @return A floating-point representation of the fixed-point value.
 */
double readNintendoFixedPoint(uint32_t value, bool sign, uint8_t iBits, uint8_t fBits);

/** Read a half-precision 16-bit IEEE float, converting it into a 32-bit iEEE float. */
float readIEEEFloat16(uint16_t value);

#endif // COMMON_UTIL_H
