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
 *  Utility functions for generating unique IDs.
 */

#include <sstream>
#include <atomic>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "src/common/uuid.h"

namespace Common {

UString generateIDRandomString() {
	std::stringstream ss;

	ss << boost::uuids::random_generator()();

	return ss.str();
}

static std::atomic<uint32_t> idNumber(1);
uint32_t generateIDNumber() {
	return idNumber.fetch_add(1, std::memory_order_relaxed);
}

static UString uint64ToString(uint64_t i) {
	std::string str;
	str.reserve(20);

	do {
		str += "0123456789"[i % 10];
		i /= 10;
	} while (i);

	std::reverse(str.begin(), str.end());

	return str;
}

static std::atomic<uint64_t> idNumberString(1);
UString generateIDNumberString() {
	return uint64ToString(idNumberString.fetch_add(1, std::memory_order_relaxed));
}

} // End of namespace Common
