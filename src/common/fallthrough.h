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
 *  Compiler-specific defines to mark an implicit switch-case fallthrough.
 */

#ifndef COMMON_FALLTHROUGH_H
#define COMMON_FALLTHROUGH_H

/* XOREOS_FALLTHROUGH is an annotation to suppress compiler warnings about switch
 * cases that fall through without a break or return statement. XOREOS_FALLTHROUGH
 * is only needed on cases that have code.
 *
 * Based on Mozilla's MOZ_FALLTHROUGH and Boost's BOOST_FALLTHROUGH.
 *
 * switch (foo) {
 *   case 1: // These cases have no code. No fallthrough annotations are needed.
 *   case 2:
 *   case 3: // This case has code, so a fallthrough annotation is needed!
 *     foo++;
 *     XOREOS_FALLTHROUGH;
 *   case 4:
 *     return foo;
 * }
 */
#ifndef __has_cpp_attribute
	#define __has_cpp_attribute(x) 0
#endif

#if __has_cpp_attribute(clang::fallthrough)
	#define XOREOS_FALLTHROUGH [[clang::fallthrough]]
#elif __has_cpp_attribute(gnu::fallthrough)
	#define XOREOS_FALLTHROUGH [[gnu::fallthrough]]
#elif defined(_MSC_VER)
	/*
	 * MSVC's __fallthrough annotations are checked by /analyze (Code Analysis):
	 * https://msdn.microsoft.com/en-us/library/ms235402%28VS.80%29.aspx
	 */
	#include <sal.h>
	#define XOREOS_FALLTHROUGH __fallthrough
#else
	#define XOREOS_FALLTHROUGH // Fallthrough
#endif

#if defined(__clang__)
	/* Macros to make clang ignore implicit fallthroughs in certain includes.
	 *
	 * This is needed for the SDL2 headers. Because sdl2-config puts the SDL2
	 * path into -I instead of -isystem, and SDL2 marks implicit fallthroughs
	 * with a comment instead of an attribute -- something which clang doesn't
	 * understand -- clang spits out a warning for the SDL2 headers.
	 *
	 * We don't want this, so we disable this warning for the SDL2 headers.
	 *
	 * Usage: wrap the include in these two macros.
	 */
	#define START_IGNORE_IMPLICIT_FALLTHROUGH _Pragma("clang diagnostic push") \
	                                          _Pragma("clang diagnostic ignored \"-Wimplicit-fallthrough\"")
	#define STOP_IGNORE_IMPLICIT_FALLTHROUGH _Pragma("clang diagnostic pop")
#else
	#define START_IGNORE_IMPLICIT_FALLTHROUGH
	#define STOP_IGNORE_IMPLICIT_FALLTHROUGH
#endif

#endif // COMMON_FALLTHROUGH_H
