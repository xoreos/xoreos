/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

// Mostly based on ScummVM's architecture/system checks

#ifndef COMMON_SYSTEM_H
#define COMMON_SYSTEM_H

#if defined(HAVE_CONFIG_H)
	#include "config.h"
#endif

#if defined(_MSC_VER)

	#define scumm_stricmp stricmp
	#define scumm_strnicmp _strnicmp
	#define snprintf _snprintf

	#define EOS_LITTLE_ENDIAN

	#define FORCEINLINE __forceinline
	#define NORETURN_PRE __declspec(noreturn)
	#define PLUGIN_EXPORT __declspec(dllexport)

#elif defined(__MINGW32__)

	#define scumm_stricmp stricmp
	#define scumm_strnicmp strnicmp

	#define EOS_LITTLE_ENDIAN

	#define PLUGIN_EXPORT __declspec(dllexport)

#elif defined(UNIX)

	#define scumm_stricmp strcasecmp
	#define scumm_strnicmp strncasecmp

	#if defined(HAVE_CONFIG_H)
		#if defined(WORDS_BIGENDIAN)
			#define EOS_BIG_ENDIAN
		#else
			#define EOS_LITTLE_ENDIAN
		#endif
	#endif

#else
	#error No system type defined
#endif

//
// GCC specific stuff
//
#if defined(__GNUC__)
	#define NORETURN_POST __attribute__((__noreturn__))
	#define PACKED_STRUCT __attribute__((__packed__))
	#define GCC_PRINTF(x,y) __attribute__((__format__(printf, x, y)))

	#if !defined(FORCEINLINE) && (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1))
		#define FORCEINLINE inline __attribute__((__always_inline__))
	#endif
#else
	#define PACKED_STRUCT
	#define GCC_PRINTF(x,y)
#endif

//
// Fallbacks / default values for various special macros
//
#ifndef FORCEINLINE
#define FORCEINLINE inline
#endif

#ifndef NORETURN_PRE
#define NORETURN_PRE
#endif

#ifndef NORETURN_POST
#define NORETURN_POST
#endif

#ifndef STRINGBUFLEN
#define STRINGBUFLEN 1024
#endif

#ifndef PI
#define PI 3.14159265358979323846
#endif

#ifndef MAXPATHLEN
#define MAXPATHLEN 256
#endif

#endif // COMMON_SYSTEM_H
