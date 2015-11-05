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
 *  Compiler-specific defines to declare a function non-returning.
 */

#ifndef COMMON_NORETURN_H
#define COMMON_NORETURN_H

#if defined(_MSC_VER)
	#define NORETURN_PRE __declspec(noreturn)
#elif defined(__GNUC__)
	#define NORETURN_POST __attribute__((__noreturn__))
#endif

#ifndef NORETURN_PRE
	#define NORETURN_PRE
#endif

#ifndef NORETURN_POST
	#define NORETURN_POST
#endif

#endif // COMMON_NORETURN_H
