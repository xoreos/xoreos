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
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file common/uuid.cpp
 *  Utility functions for generating unique IDs.
 */

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
