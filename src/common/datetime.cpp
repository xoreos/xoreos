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
 *  Utility functions for manipulating date and time.
 */

#include "src/common/datetime.h"
#include "src/common/error.h"

using boost::posix_time::ptime;
using boost::posix_time::second_clock;
using boost::posix_time::from_iso_string;
using boost::posix_time::not_a_date_time;

namespace Common {

DateTime::DateTime(TimeZone zone) : ptime(not_a_date_time) {
	switch (zone) {
		case kUTC:
			ptime::operator=(second_clock::universal_time());
			break;

		case kLocal:
			ptime::operator=(second_clock::local_time());
			break;

	default:
		throw Exception("Invalid time zone specifier (%u)", (uint) zone);
	}
}

DateTime::DateTime(const UString &value) : ptime(not_a_date_time) {
	try {
		ptime::operator=(from_iso_string(value.c_str()));
	} catch (std::exception &e) {
		Exception se(e);

		se.add("Failed to create DateTime from \"%s\"", value.c_str());
		throw se;
	}
}

UString DateTime::formatDateISO(uint32_t sep) const {
	const UString sepStr(sep, sep ? 1 : 0);

	return UString::format("%04d%s%02d%s%02d",
	                       (int) date().year() , sepStr.c_str(),
	                       (int) date().month(), sepStr.c_str(),
	                       (int) date().day());
}

UString DateTime::formatTimeISO(uint32_t sep) const {
	const UString sepStr(sep, sep ? 1 : 0);

	return UString::format("%02d%s%02d%s%02d",
	                       (int) time_of_day().hours()  , sepStr.c_str(),
	                       (int) time_of_day().minutes(), sepStr.c_str(),
	                       (int) time_of_day().seconds());
}

UString DateTime::formatDateTimeISO(uint32_t sep, uint32_t sepDate, uint32_t sepTime) const {
	const UString sepStr(sep, sep ? 1 : 0);

	return UString::format("%s%s%s", formatDateISO(sepDate).c_str(), sepStr.c_str(),
	                                 formatTimeISO(sepTime).c_str());
}

} // End of namespace Common
