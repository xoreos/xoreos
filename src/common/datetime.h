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

#ifndef COMMON_TIME_H
#define COMMON_TIME_H

#include <boost/date_time/posix_time/posix_time.hpp>

#include "src/common/types.h"
#include "src/common/ustring.h"

namespace Common {

/** A date/time object, storing a specific point in time. */
class DateTime : public boost::posix_time::ptime {
public:
	enum TimeZone {
		kUTC,  ///< Coordinated Universal Time (UTC).
		kLocal ///< The nondescript local time zone.
	};

	/** Create a DateTime object from the current date and time.
	 *
	 *  @param zone The time zone the current date and time should be expressed as.
	 *              Can be either Coordinated Universal Time (UTC), or the local
	 *              system time zone.
	 *
	 * Note: The time zone is not stored within the object. It is only used to
	 *       specify a base for the current time.
	 */
	DateTime(TimeZone zone);
	/** Create a DateTime object by parsing a string representation of a specific date and time.
	 *
	 *  @param value The time value to parse. It has to be in YYYYmmDDTHHMMSS format,
	 *               or %Y%m%dT%H%M%S in strftime() notation, i.e. ISO 8601 format,
	 *               with the date and the time separated by a 'T', but the individual
	 *               parts of the date and time not separated at all.
	 */
	DateTime(const UString &value);

	/** Return the year in the Gregorian calendar. */
	uint16_t getYear () const { return date().year(); }
	/** Return the month in the Gregorian calendar (1..12). */
	uint8_t  getMonth() const { return date().month(); }
	/** Return the day in the Gregorian calendar (1..31). */
	uint8_t  getDay  () const { return date().day(); }

	/** Return the hour in the 24-hour clock (0..23). */
	uint8_t getHour  () const { return time_of_day().hours(); }
	/** Return the minute (0..59). */
	uint8_t getMinute() const { return time_of_day().minutes(); }
	/** Return the second (0..60). */
	uint8_t getSecond() const { return time_of_day().seconds(); }

	/** Return the year, month (1..12) and day (1..31) in the Gregorian calendar. */
	void getDate(uint16_t &year, uint8_t &month, uint8_t &day) const {
		year  = getYear();
		month = getMonth();
		day   = getDay();
	}
	/** Return the hour in the 24-hour clock (0..23), minute (0..59) and second (0..60). */
	void getTime(uint8_t &hour, uint8_t &minute, uint8_t &second) const {
		hour   = getHour();
		minute = getMinute();
		second = getSecond();
	}
	/** Return the year, month (1..12), day (1..31), hour (0..23), minute (0..59) and second (0..60). */
	void getDateTime(uint16_t &year, uint8_t &month, uint8_t &day, uint8_t &hour, uint8_t &minute, uint8_t &second) const {
		getDate(year, month , day);
		getTime(hour, minute, second);
	}

	/** Return a string representation of the date in ISO 8601 format.
	 *
	 *  The format will be YYYY-mm-DD, with YYYY being a 4 digit year in the
	 *  Gregorian calendar, mm being a 2 digit month of the year (01..12) in the
	 *  Gregorian calendar, DD being a 2 digit day of the month (01..31) in the
	 *  Gregorian calendar, and - being a freely selectable separator
	 *  character.
	 *
	 *  If the separator character is 0, no separator at all is used.
	 *  The format of the string is then YYYYMMDD.
	 *
	 *  @param sep Use this character as the separator character.
	 */
	UString formatDateISO(uint32_t sep = 0) const;

	/** Return a string representation of the time in ISO 8601 format.
	 *
	 *  The format will be HH:MM:SS, with HH being a 2 digit hour in the
	 *  24-hour clock (00..23), MM being a 2 digit minute (00..59), SS
	 *  being a 2 digit second (00..60, includes a potential leap second),
	 *  and : being a freely selectable separator character.
	 *
	 *  If the separator character is 0, no separator at all is used.
	 *  The format of the string is then HHMMSS.
	 *
	 *  @param sep Use this character as the separator character.
	 */
	UString formatTimeISO(uint32_t sep = 0) const;

	/** Return a string representation of the date and time in ISO 8601 format.
	 *
	 *  The format will be YYYY-mm-DDTHH:MM:SS, with T being a freely selectable
	 *  separator character between the date and the time. For the formatting of
	 *  the date and time, please see formatDateISO() and formatTimeISO().
	 *
	 *  If the separator character is 0, no separator at all is used.
	 *  The format of the string is then YYYY-mm-DDHH:MM:SS.
	 *
	 *  @param sep     Use this character as the separator between date and time.
	 *  @param sepDate Use this character as the separator between the date elements.
	 *  @param sepTime Use this character as the separator between the time elements.
	 */
	UString formatDateTimeISO(uint32_t sep = 0, uint32_t sepDate = 0, uint32_t sepTime = 0) const;
};

} // End of namespace Common

#endif // COMMON_TIME_H
