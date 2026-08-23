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
