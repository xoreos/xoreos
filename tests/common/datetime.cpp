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
 *  Unit tests for the DateTime class.
 */

#include "gtest/gtest.h"

#include "src/common/error.h"
#include "src/common/datetime.h"

GTEST_TEST(DateTime, invalid) {
	EXPECT_THROW(Common::DateTime("Foobar"), Common::Exception);
}

GTEST_TEST(DateTime, valid) {
	const Common::DateTime dateTime("20160825T235506");

	EXPECT_EQ(dateTime.getYear  (), 2016);
	EXPECT_EQ(dateTime.getMonth (),    8);
	EXPECT_EQ(dateTime.getDay   (),   25);
	EXPECT_EQ(dateTime.getHour  (),   23);
	EXPECT_EQ(dateTime.getMinute(),   55);
	EXPECT_EQ(dateTime.getSecond(),    6);
}

GTEST_TEST(DateTime, format) {
	const Common::DateTime dateTime("20160825T235506");

	EXPECT_STREQ(dateTime.formatDateTimeISO('T', '-', ':').c_str(), "2016-08-25T23:55:06");
}
