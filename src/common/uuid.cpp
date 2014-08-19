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

/** @file common/uuid.cpp
 *  Utility functions for generating unique IDs.
 */


/* WORKAROUND: boost/atomic.hpp typedefs atomic<intptr_t> and atomic<uintptr_t>,
 *             but doesn't include stdint.h on purpose when compiling with gcc,
 *             only unistd.h. Unfortunately, unistd.h in glibc 2.19 only
 *             defines intptr_t and not uintptr_t, breaking compilation.
 *
 *             To fix this issue, we manually include both unistd.h and stdint.h
 *             (in that order), if they're available.
 */
#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif // HAVE_CONFIG_H
#ifdef HAVE_UNISTD_H
	#include <unistd.h>
#endif // HAVE_UNISTD_H
#ifdef HAVE_STDINT_H
	#include <stdint.h>
#endif // HAVE_STDINT_H


#include <sstream>

#include <boost/atomic.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "common/uuid.h"

namespace Common {

UString generateIDRandomString() {
	std::stringstream ss;

	ss << boost::uuids::random_generator()();

	return ss.str();
}

static boost::atomic<uint32> idNumber(1);
uint32 generateIDNumber() {
	return idNumber.fetch_add(1);
}

static UString uint64ToString(uint64 i) {
	std::string str;
	str.reserve(20);

	do {
		str += "0123456789"[i % 10];
		i /= 10;
	} while (i);

	std::reverse(str.begin(), str.end());

	return str;
}

static boost::atomic<uint64> idNumberString(1);
UString generateIDNumberString() {
	return uint64ToString(idNumberString.fetch_add(1));
}

} // End of namespace Common
