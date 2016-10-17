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
 *  Low-level detection of architecture/system properties.
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

#ifndef COMMON_SYSTEM_H
#define COMMON_SYSTEM_H

#if defined(HAVE_CONFIG_H)
	#include "config.h"
#endif

#include "src/common/noreturn.h"

#if defined(_MSC_VER)

	#include <cstdarg>
	#include <cstdio>
	#include <cstdlib>

	#define snprintf c99_snprintf
	#define vsnprintf c99_vsnprintf

	#ifndef HAVE_STRTOLL
		#define strtoll _strtoi64
		#define HAVE_STRTOLL 1
	#endif

	#ifndef HAVE_STRTOULL
		#define strtoull _strtoui64
		#define HAVE_STRTOULL 1
	#endif

	#if !defined(XOREOS_LITTLE_ENDIAN) && !defined(XOREOS_BIG_ENDIAN)
		#define XOREOS_LITTLE_ENDIAN 1
	#endif

	#define FORCEINLINE __forceinline
	#define PLUGIN_EXPORT __declspec(dllexport)

	static FORCEINLINE int c99_vsnprintf(char *str, size_t size, const char *format, va_list ap) {
		int count = -1;

		if (size != 0)
			count = _vsnprintf_s(str, size, _TRUNCATE, format, ap);
		if (count == -1)
			count = _vscprintf(format, ap);

		return count;
	}

	static FORCEINLINE int c99_snprintf(char *str, size_t size, const char *format, ...) {
		int count;
		va_list ap;

		va_start(ap, format);
		count = c99_vsnprintf(str, size, format, ap);
		va_end(ap);

		return count;
	}

	#ifndef WIN32
		#define WIN32
	#endif

	#define IGNORE_UNUSED_VARIABLES __pragma(warning(disable : 4101))

#elif defined(__MINGW32__)

	#if !defined(XOREOS_LITTLE_ENDIAN) && !defined(XOREOS_BIG_ENDIAN)
		#define XOREOS_LITTLE_ENDIAN 1
	#endif

	#define PLUGIN_EXPORT __declspec(dllexport)

	#ifndef WIN32
		#define WIN32
	#endif

#elif defined(UNIX)

	#if !defined(XOREOS_LITTLE_ENDIAN) && !defined(XOREOS_BIG_ENDIAN)
		#if defined(HAVE_CONFIG_H)
			#if defined(WORDS_BIGENDIAN)
				#define XOREOS_BIG_ENDIAN 1
			#else
				#define XOREOS_LITTLE_ENDIAN 1
			#endif
		#endif
	#endif

#else
	#error No system type defined
#endif

//
// GCC specific stuff
//
#if defined(__GNUC__)
	#define PACKED_STRUCT __attribute__((__packed__))
	#define GCC_PRINTF(x,y) __attribute__((__format__(printf, x, y)))

	#if (__GNUC__ >= 3)
		// Macro to ignore several "unused variable" warnings produced by GCC
		#define IGNORE_UNUSED_VARIABLES _Pragma("GCC diagnostic ignored \"-Wunused-variable\"") \
		                                _Pragma("GCC diagnostic ignored \"-Wunused-but-set-variable\"")
	#endif

	#if !defined(FORCEINLINE) && (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1))
		#define FORCEINLINE inline __attribute__((__always_inline__))
	#endif

#else
	#define PACKED_STRUCT
	#define GCC_PRINTF(x,y)
#endif

#if defined(__cplusplus)
	#define UNUSED(x)
#else
	#if defined(__GNUC__)
		#define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
	#else
		#define UNUSED(x) UNUSED_ ## x
	#endif
#endif

#if defined(__clang__)
	// clang does not know the "unused-but-set-variable" (but claims to be GCC)
	#undef IGNORE_UNUSED_VARIABLES
	#define IGNORE_UNUSED_VARIABLES _Pragma("GCC diagnostic ignored \"-Wunused-variable\"")
#endif

//
// Fallbacks for various functions
//

#ifndef HAVE_STRTOF
	#define strtof c99_strtof

	static FORCEINLINE float c99_strtof(const char *nptr, char **endptr) {
		return (float) strtod(nptr, endptr);
	}

	#define HAVE_STRTOF 1
#endif

#ifndef HAVE_STRTOLL
	#define strtoll c99_strtoll

	#include <cctype>
	#include <cerrno>
	#include <climits>

	#ifndef LLONG_MAX
		#define LLONG_MAX   0x7FFFFFFFFFFFFFFFlL
	#endif

	#ifndef LLONG_MIN
		#define LLONG_MIN (-0x7FFFFFFFFFFFFFFFLL-1)
	#endif

	/** Convert a string to a long long.
	 *
	 *  Based on OpenBSD's strtoll() function, released under the terms of
	 *  the 3-clause BSD license.
	 *
	 *  Ignores 'locale' stuff.  Assumes that the upper and lower case
	 *  alphabets and digits are each contiguous.
	 */
	static long long c99_strtoll(const char *nptr, char **endptr, int base) {
		const char *s;
		long long acc, cutoff;
		int c;
		int neg, any, cutlim;

		/*
		 * Skip white space and pick up leading +/- sign if any.
		 * If base is 0, allow 0x for hex and 0 for octal, else
		 * assume decimal; if base is already 16, allow 0x.
		 */
		s = nptr;
		do {
			c = (unsigned char) *s++;
		} while (isspace(c));
		if (c == '-') {
			neg = 1;
			c = *s++;
		} else {
			neg = 0;
			if (c == '+')
				c = *s++;
		}
		if ((base == 0 || base == 16) &&
		    c == '0' && (*s == 'x' || *s == 'X')) {
			c = s[1];
			s += 2;
			base = 16;
		}
		if (base == 0)
			base = c == '0' ? 8 : 10;

		/*
		 * Compute the cutoff value between legal numbers and illegal
		 * numbers.  That is the largest legal value, divided by the
		 * base.  An input number that is greater than this value, if
		 * followed by a legal input character, is too big.  One that
		 * is equal to this value may be valid or not; the limit
		 * between valid and invalid numbers is then based on the last
		 * digit.  For instance, if the range for long longs is
		 * [-9223372036854775808..9223372036854775807] and the input base
		 * is 10, cutoff will be set to 922337203685477580 and cutlim to
		 * either 7 (neg==0) or 8 (neg==1), meaning that if we have
		 * accumulated a value > 922337203685477580, or equal but the
		 * next digit is > 7 (or 8), the number is too big, and we will
		 * return a range error.
		 *
		 * Set any if any `digits' consumed; make it negative to indicate
		 * overflow.
		 */
		cutoff = neg ? LLONG_MIN : LLONG_MAX;
		cutlim = cutoff % base;
		cutoff /= base;
		if (neg) {
			if (cutlim > 0) {
				cutlim -= base;
				cutoff += 1;
			}
			cutlim = -cutlim;
		}
		for (acc = 0, any = 0;; c = (unsigned char) *s++) {
			if (isdigit(c))
				c -= '0';
			else if (isalpha(c))
				c -= isupper(c) ? 'A' - 10 : 'a' - 10;
			else
				break;
			if (c >= base)
				break;
			if (any < 0)
				continue;
			if (neg) {
				if (acc < cutoff || (acc == cutoff && c > cutlim)) {
					any = -1;
					acc = LLONG_MIN;
					errno = ERANGE;
				} else {
					any = 1;
					acc *= base;
					acc -= c;
				}
			} else {
				if (acc > cutoff || (acc == cutoff && c > cutlim)) {
					any = -1;
					acc = LLONG_MAX;
					errno = ERANGE;
				} else {
					any = 1;
					acc *= base;
					acc += c;
				}
			}
		}
		if (endptr != 0)
			*endptr = any ? const_cast<char *>(s - 1) : const_cast<char *>(nptr);
		return (acc);
	}

	#define HAVE_STRTOLL 1
#endif

#ifndef HAVE_STRTOULL
	#define strtoull c99_strtoull

	#include <cctype>
	#include <cerrno>
	#include <climits>

	#ifndef ULLONG_MAX
		#define ULLONG_MAX 0xFFFFFFFFFFFFFFFFULL
	#endif

	/** Convert a string to an unsigned long long.
	 *
	 *  Based on OpenBSD's strtoull() function, released under the terms of
	 *  the 3-clause BSD license.
	 *
	 *  Ignores 'locale' stuff.  Assumes that the upper and lower case
	 *  alphabets and digits are each contiguous.
	 */
	static unsigned long long c99_strtoull(const char *nptr, char **endptr, int base) {
		const char *s;
		unsigned long long acc, cutoff;
		int c;
		int neg, any, cutlim;

		/*
		 * See c99_strtoll() for comments as to the logic used.
		 */
		s = nptr;
		do {
			c = (unsigned char) *s++;
		} while (isspace(c));
		if (c == '-') {
			neg = 1;
			c = *s++;
		} else {
			neg = 0;
			if (c == '+')
				c = *s++;
		}
		if ((base == 0 || base == 16) &&
		    c == '0' && (*s == 'x' || *s == 'X')) {
			c = s[1];
			s += 2;
			base = 16;
		}
		if (base == 0)
			base = c == '0' ? 8 : 10;

		cutoff = ULLONG_MAX / (unsigned long long)base;
		cutlim = ULLONG_MAX % (unsigned long long)base;
		for (acc = 0, any = 0;; c = (unsigned char) *s++) {
			if (isdigit(c))
				c -= '0';
			else if (isalpha(c))
				c -= isupper(c) ? 'A' - 10 : 'a' - 10;
			else
				break;
			if (c >= base)
				break;
			if (any < 0)
				continue;
			if (acc > cutoff || (acc == cutoff && c > cutlim)) {
				any = -1;
				acc = ULLONG_MAX;
				errno = ERANGE;
			} else {
				any = 1;
				acc *= static_cast<unsigned long long>(base);
				acc += c;
			}
		}
		if (neg && any > 0)
			acc = -acc;
		if (endptr != 0)
			*endptr = any ? const_cast<char *>(s - 1) : const_cast<char *>(nptr);
		return (acc);
	}

	#define HAVE_STRTOULL 1
#endif

// Compatibility macro for dealing with the explicit bool cast problem.
#if __cplusplus < 201103L
	#define XOREOS_EXPLICIT_OPERATOR_CONV
#else
	#define XOREOS_EXPLICIT_OPERATOR_CONV explicit
#endif

//
// Fallbacks / default values for various special macros
//
#ifndef FORCEINLINE
	#define FORCEINLINE inline
#endif

#ifndef STRINGBUFLEN
	#define STRINGBUFLEN 1024
#endif

#ifndef MAXPATHLEN
	#define MAXPATHLEN 256
#endif

#ifndef IGNORE_UNUSED_VARIABLES
	#define IGNORE_UNUSED_VARIABLES
#endif

#endif // COMMON_SYSTEM_H
