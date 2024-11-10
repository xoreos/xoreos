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

#include "src/common/fallthrough.h"

#if defined(_MSC_VER)

	#include <cstdarg>
	#include <cstdio>
	#include <cstdlib>

	#define FORCEINLINE __forceinline
	#define PLUGIN_EXPORT __declspec(dllexport)

	#if _MSC_VER < 1900
		#define snprintf c99_snprintf
		#define vsnprintf c99_vsnprintf

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
	#endif

	#if !defined(XOREOS_LITTLE_ENDIAN) && !defined(XOREOS_BIG_ENDIAN)
		#define XOREOS_LITTLE_ENDIAN 1
	#endif

	#ifndef WIN32
		#define WIN32
	#endif

	#define DIAGNOSTICS_PUSH __pragma(warning(push))
	#define DIAGNOSTICS_POP __pragma(warning(pop))

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

	#define DIAGNOSTICS_PUSH _Pragma("GCC diagnostic push")
	#define DIAGNOSTICS_POP _Pragma("GCC diagnostic pop")

	#if (__GNUC__ >= 3)
		// Macro to ignore several "unused variable" warnings produced by GCC
		#define IGNORE_UNUSED_VARIABLES _Pragma("GCC diagnostic ignored \"-Wunused-variable\"") \
		                                _Pragma("GCC diagnostic ignored \"-Wunused-but-set-variable\"")
	#endif
	#if (__GNUC__ >= 8)
		#define IGNORE_FUNCTION_CAST _Pragma("GCC diagnostic ignored \"-Wcast-function-type\"")
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

#ifndef DIAGNOSTICS_PUSH
	#define DIAGNOSTICS_PUSH
#endif

#ifndef DIAGNOSTICS_POP
	#define DIAGNOSTICS_POP
#endif

#ifndef IGNORE_UNUSED_VARIABLES
	#define IGNORE_UNUSED_VARIABLES
#endif

#ifndef IGNORE_FUNCTION_CAST
	#define IGNORE_FUNCTION_CAST
#endif

#endif // COMMON_SYSTEM_H
