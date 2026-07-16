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
 *  Project-wide assertion macros.
 *
 *  Usage:
 *      Is_True(ptr != 0, ("some pointer was null: %s", name));
 *      Is_False(x < 0, ("negative x: %d", x));
 *
 *  Message syntax uses doubled parens so the macro can hold a variable
 *  argument list without requiring the call site to use varargs tricks.
 *  The message token expands directly onto the existing xoreos warning()
 *  function, i.e. `warning msg` becomes `warning("fmt", arg)`.
 *
 *  Behavior is gated on NDEBUG:
 *    - Debug builds   -> print FILE:LINE in func, print message, abort().
 *    - Release builds  -> downgrade to warning() so unexpected-but-recoverable
 *                         conditions (e.g. EE-specific format extensions) do
 *                         not crash the engine.
 */

#ifndef COMMON_ASSERTION_H
#define COMMON_ASSERTION_H

#include <cstdlib>

#include "src/common/util.h"

#if defined(NDEBUG)
#define Is_True(cond, msg) \
	do { \
		if (!(cond)) { \
			::warning("Is_True(%s) failed at %s:%d in %s", #cond, __FILE__, __LINE__, __func__); \
			::warning msg; \
		} \
	} while (0)
#else
#define Is_True(cond, msg) \
	do { \
		if (!(cond)) { \
			::warning("Is_True(%s) failed at %s:%d in %s", #cond, __FILE__, __LINE__, __func__); \
			::warning msg; \
			std::abort(); \
		} \
	} while (0)
#endif

#define Is_False(cond, msg) Is_True(!(cond), msg)

#endif // COMMON_ASSERTION_H
