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
 *
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

// Mostly based on ScummVM's architecture/system checks

/** @file common/system.h
 *  Low-level detection of architecture/system properties.
 */

#ifndef COMMON_SYSTEM_H
#define COMMON_SYSTEM_H

#include <boost/config.hpp>

#define PLUGIN_EXPORT BOOST_SYMBOL_EXPORT
#define FORCEINLINE   BOOST_FORCEINLINE

#if defined(HAVE_CONFIG_H)
	#include "config.h"
#endif

#if defined(_MSC_VER)
	#define snprintf _snprintf
	#define NORETURN_PRE __declspec(noreturn)

	#ifndef WIN32
		#define WIN32
	#endif

#elif defined(__MINGW32__)

	#ifndef WIN32
		#define WIN32
	#endif
#endif

//
// GCC specific stuff
//
#if defined(__GNUC__)
	#define NORETURN_POST __attribute__((__noreturn__))
	#define PACKED_STRUCT __attribute__((__packed__))
	#define GCC_PRINTF(x,y) __attribute__((__format__(printf, x, y)))
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

#ifndef MAXPATHLEN
	#define MAXPATHLEN 256
#endif

#endif // COMMON_SYSTEM_H
