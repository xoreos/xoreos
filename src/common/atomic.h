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
 *  Helper header to include boost::atomic.
 *
 *  boost/atomic.hpp typedefs atomic<intptr_t> and atomic<uintptr_t>,
 *  but doesn't include stdint.h on purpose when compiling with gcc,
 *  only unistd.h. Unfortunately, unistd.h in glibc 2.19 only
 *  defines intptr_t and not uintptr_t, breaking compilation.
 *
 *  To fix this issue, we manually include both unistd.h and stdint.h
 *  (in that order), if they're available.
 *
 *  This strict order requirement of unistd.h and stdint.h also means
 *  that this header needs to be included relatively early, ideally
 *  at the top.
 */

#ifndef COMMON_ATOMIC_H
#define COMMON_ATOMIC_H

#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif // HAVE_CONFIG_H
#ifdef HAVE_UNISTD_H
	#include <unistd.h>
#endif // HAVE_UNISTD_H
#ifdef HAVE_STDINT_H
	#include <stdint.h>
#endif // HAVE_STDINT_H

#include <boost/atomic.hpp>

#endif // COMMON_ATOMIC_H
