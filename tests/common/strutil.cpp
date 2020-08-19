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
 *  Unit tests for our string and stream utilities.
 */

#include "gtest/gtest.h"

#include "src/common/error.h"
#include "src/common/ustring.h"
#include "src/common/strutil.h"
#include "src/common/memreadstream.h"

GTEST_TEST(StrUtil, composeBool) {
	EXPECT_STREQ(Common::composeString(true ).c_str(), "true" );
	EXPECT_STREQ(Common::composeString(false).c_str(), "false");
}

GTEST_TEST(StrUtil, composeInt8) {
	EXPECT_STREQ(Common::composeString((int8_t) -23).c_str(), "-23");
	EXPECT_STREQ(Common::composeString((int8_t)  23).c_str(),  "23");

	EXPECT_STREQ(Common::composeString((int8_t)   0).c_str(),   "0");
	EXPECT_STREQ(Common::composeString((int8_t) - 0).c_str(),   "0");

	EXPECT_STREQ(Common::composeString((int8_t) INT8_MIN).c_str(), "-128");
	EXPECT_STREQ(Common::composeString((int8_t) INT8_MAX).c_str(),  "127");
}

GTEST_TEST(StrUtil, composeInt16) {
	EXPECT_STREQ(Common::composeString((int16_t) -23).c_str(), "-23");
	EXPECT_STREQ(Common::composeString((int16_t)  23).c_str(),  "23");

	EXPECT_STREQ(Common::composeString((int16_t)   0).c_str(),   "0");
	EXPECT_STREQ(Common::composeString((int16_t) - 0).c_str(),   "0");

	EXPECT_STREQ(Common::composeString((int16_t) INT16_MIN).c_str(), "-32768");
	EXPECT_STREQ(Common::composeString((int16_t) INT16_MAX).c_str(),  "32767");
}

GTEST_TEST(StrUtil, composeInt32) {
	EXPECT_STREQ(Common::composeString((int32_t) -23).c_str(), "-23");
	EXPECT_STREQ(Common::composeString((int32_t)  23).c_str(),  "23");

	EXPECT_STREQ(Common::composeString((int32_t)   0).c_str(),   "0");
	EXPECT_STREQ(Common::composeString((int32_t) - 0).c_str(),   "0");

	EXPECT_STREQ(Common::composeString((int32_t) INT32_MIN).c_str(), "-2147483648");
	EXPECT_STREQ(Common::composeString((int32_t) INT32_MAX).c_str(),  "2147483647");
}

GTEST_TEST(StrUtil, composeInt64) {
	EXPECT_STREQ(Common::composeString((int64_t) -23).c_str(), "-23");
	EXPECT_STREQ(Common::composeString((int64_t)  23).c_str(),  "23");

	EXPECT_STREQ(Common::composeString((int64_t)   0).c_str(),   "0");
	EXPECT_STREQ(Common::composeString((int64_t) - 0).c_str(),   "0");

	EXPECT_STREQ(Common::composeString((int64_t) INT64_MIN).c_str(), "-9223372036854775808");
	EXPECT_STREQ(Common::composeString((int64_t) INT64_MAX).c_str(),  "9223372036854775807");
}

GTEST_TEST(StrUtil, composeUInt8) {
	EXPECT_STREQ(Common::composeString((uint8_t) 23).c_str(), "23");

	EXPECT_STREQ(Common::composeString((uint8_t)  0).c_str(),  "0");

	EXPECT_STREQ(Common::composeString((uint8_t) UINT8_MAX).c_str(), "255");
}

GTEST_TEST(StrUtil, composeUInt16) {
	EXPECT_STREQ(Common::composeString((uint16_t) 23).c_str(), "23");

	EXPECT_STREQ(Common::composeString((uint16_t)  0).c_str(),  "0");

	EXPECT_STREQ(Common::composeString((uint16_t) UINT16_MAX).c_str(), "65535");
}

GTEST_TEST(StrUtil, composeUInt32) {
	EXPECT_STREQ(Common::composeString((uint32_t) 23).c_str(), "23");

	EXPECT_STREQ(Common::composeString((uint32_t)  0).c_str(),  "0");

	EXPECT_STREQ(Common::composeString((uint32_t) UINT32_MAX).c_str(), "4294967295");
}

GTEST_TEST(StrUtil, composeUInt64) {
	EXPECT_STREQ(Common::composeString((uint64_t) 23).c_str(), "23");

	EXPECT_STREQ(Common::composeString((uint64_t)  0).c_str(),  "0");

	EXPECT_STREQ(Common::composeString((uint64_t) UINT64_MAX).c_str(), "18446744073709551615");
}

GTEST_TEST(StrUtil, composeSizeT) {
	EXPECT_STREQ(Common::composeString((size_t) 23).c_str(), "23");
	EXPECT_STREQ(Common::composeString((size_t)  0).c_str(),  "0");
}

GTEST_TEST(StrUtil, composeFloat) {
	EXPECT_EQ(strncmp(Common::composeString((float) -1.0).c_str(), "-1.0", 4), 0);
	EXPECT_EQ(strncmp(Common::composeString((float)  1.0).c_str(),  "1.0", 3), 0);

	EXPECT_EQ(strncmp(Common::composeString((float)  0.0).c_str(),  "0.0", 3), 0);
}

GTEST_TEST(StrUtil, composeDouble) {
	EXPECT_EQ(strncmp(Common::composeString((double) -1.0).c_str(), "-1.0", 4), 0);
	EXPECT_EQ(strncmp(Common::composeString((double)  1.0).c_str(),  "1.0", 3), 0);

	EXPECT_EQ(strncmp(Common::composeString((double)  0.0).c_str(),  "0.0", 3), 0);
}

GTEST_TEST(StrUtil, parseBool) {
	bool x = false;

	Common::parseString("true", x);
	EXPECT_EQ(x, true);
	Common::parseString("false", x);
	EXPECT_EQ(x, false);

	Common::parseString("yes", x);
	EXPECT_EQ(x, true);
	Common::parseString("no", x);
	EXPECT_EQ(x, false);

	Common::parseString("y", x);
	EXPECT_EQ(x, true);
	Common::parseString("n", x);
	EXPECT_EQ(x, false);

	Common::parseString("on", x);
	EXPECT_EQ(x, true);
	Common::parseString("off", x);
	EXPECT_EQ(x, false);

	Common::parseString("1", x);
	EXPECT_EQ(x, true);
	Common::parseString("0", x);
	EXPECT_EQ(x, false);

	Common::parseString("foobar", x);
	EXPECT_EQ(x, false);
}

GTEST_TEST(StrUtil, parseInt8 ) {
	int8_t  x = 0;

	Common::parseString("-23", x);
	EXPECT_EQ(x, -23);
	Common::parseString( "23", x);
	EXPECT_EQ(x,  23);

	Common::parseString("-0", x);
	EXPECT_EQ(x, 0);
	Common::parseString( "0", x);
	EXPECT_EQ(x, 0);

	Common::parseString("-128", x);
	EXPECT_EQ(x, INT8_MIN);
	Common::parseString( "127", x);
	EXPECT_EQ(x, INT8_MAX);

	EXPECT_THROW(Common::parseString("-129", x), Common::Exception);
	EXPECT_THROW(Common::parseString(" 128", x), Common::Exception);
}

GTEST_TEST(StrUtil, parseInt16) {
	int16_t x = 0;

	Common::parseString("-23", x);
	EXPECT_EQ(x, -23);
	Common::parseString( "23", x);
	EXPECT_EQ(x,  23);

	Common::parseString("-0", x);
	EXPECT_EQ(x, 0);
	Common::parseString( "0", x);
	EXPECT_EQ(x, 0);

	Common::parseString("-32768", x);
	EXPECT_EQ(x, INT16_MIN);
	Common::parseString( "32767", x);
	EXPECT_EQ(x, INT16_MAX);

	EXPECT_THROW(Common::parseString("-32769", x), Common::Exception);
	EXPECT_THROW(Common::parseString(" 32768", x), Common::Exception);
}

GTEST_TEST(StrUtil, parseInt32) {
	int32_t x = 0;

	Common::parseString("-23", x);
	EXPECT_EQ(x, -23);
	Common::parseString( "23", x);
	EXPECT_EQ(x,  23);

	Common::parseString("-0", x);
	EXPECT_EQ(x, 0);
	Common::parseString( "0", x);
	EXPECT_EQ(x, 0);

	Common::parseString("-2147483648", x);
	EXPECT_EQ(x, INT32_MIN);
	Common::parseString( "2147483647", x);
	EXPECT_EQ(x, INT32_MAX);

	EXPECT_THROW(Common::parseString("-2147483649", x), Common::Exception);
	EXPECT_THROW(Common::parseString(" 2147483648", x), Common::Exception);
}

GTEST_TEST(StrUtil, parseInt64) {
	int64_t x = 0;

	Common::parseString("-23", x);
	EXPECT_EQ(x, -23);
	Common::parseString( "23", x);
	EXPECT_EQ(x,  23);

	Common::parseString("-0", x);
	EXPECT_EQ(x, 0);
	Common::parseString( "0", x);
	EXPECT_EQ(x, 0);

	Common::parseString("-9223372036854775808", x);
	EXPECT_EQ(x, INT64_MIN);
	Common::parseString( "9223372036854775807", x);
	EXPECT_EQ(x, INT64_MAX);

	EXPECT_THROW(Common::parseString("-9223372036854775809", x), Common::Exception);
	EXPECT_THROW(Common::parseString(" 9223372036854775808", x), Common::Exception);
}

GTEST_TEST(StrUtil, parseUInt8 ) {
	uint8_t  x = 0;

	Common::parseString("23", x);
	EXPECT_EQ(x, 23);

	Common::parseString( "0", x);
	EXPECT_EQ(x, 0);

	Common::parseString("255", x);
	EXPECT_EQ(x, UINT8_MAX);

	EXPECT_THROW(Common::parseString("-23", x), Common::Exception);
	EXPECT_THROW(Common::parseString("256", x), Common::Exception);
}

GTEST_TEST(StrUtil, parseUInt16) {
	uint16_t x = 0;

	Common::parseString("23", x);
	EXPECT_EQ(x, 23);

	Common::parseString( "0", x);
	EXPECT_EQ(x, 0);

	Common::parseString("65535", x);
	EXPECT_EQ(x, UINT16_MAX);

	EXPECT_THROW(Common::parseString("-23", x), Common::Exception);
	EXPECT_THROW(Common::parseString("65536", x), Common::Exception);
}

GTEST_TEST(StrUtil, parseUInt32) {
	uint32_t x = 0;

	Common::parseString("23", x);
	EXPECT_EQ(x, 23);

	Common::parseString( "0", x);
	EXPECT_EQ(x, 0);

	Common::parseString("4294967295", x);
	EXPECT_EQ(x, UINT32_MAX);

	EXPECT_THROW(Common::parseString("-23", x), Common::Exception);
	EXPECT_THROW(Common::parseString("4294967296", x), Common::Exception);
}

GTEST_TEST(StrUtil, parseUInt64) {
	uint64_t x = 0;

	Common::parseString("23", x);
	EXPECT_EQ(x, 23);

	Common::parseString( "0", x);
	EXPECT_EQ(x, 0);

	Common::parseString("18446744073709551615", x);
	EXPECT_EQ(x, UINT64_MAX);

	EXPECT_THROW(Common::parseString("-23", x), Common::Exception);
	EXPECT_THROW(Common::parseString("18446744073709551616", x), Common::Exception);
}

GTEST_TEST(StrUtil, parseSizeT) {
	size_t x = 0;

	Common::parseString("23", x);
	EXPECT_EQ(x, 23);

	Common::parseString( "0", x);
	EXPECT_EQ(x, 0);

	EXPECT_THROW(Common::parseString("-23", x), Common::Exception);
}

GTEST_TEST(StrUtil, parseFloat) {
	float x = 0.0f;

	Common::parseString("-1.0", x);
	EXPECT_FLOAT_EQ(x, -1.0f);
	Common::parseString( "1.0", x);
	EXPECT_FLOAT_EQ(x,  1.0f);

	Common::parseString( "0.0", x);
	EXPECT_FLOAT_EQ(x, 0.0f);
}

GTEST_TEST(StrUtil, parseDouble) {
	double x = 0.0;

	Common::parseString("-1.0", x);
	EXPECT_DOUBLE_EQ(x, -1.0);
	Common::parseString( "1.0", x);
	EXPECT_DOUBLE_EQ(x,  1.0);

	Common::parseString( "0.0", x);
	EXPECT_DOUBLE_EQ(x, 0.0);
}

GTEST_TEST(StrUtil, searchBackwards) {
	static const byte kHaystack[] = { 'a','x',' ','a','b','c',' ','a','x','y',' ','a','z','x' };
	Common::MemoryReadStream haystack(kHaystack, sizeof(kHaystack));

	static const byte kNeedle1[] = { 'a','x' };
	static const byte kNeedle2[] = { 'n','o' };

	EXPECT_EQ(Common::searchBackwards(haystack, kNeedle1, sizeof(kNeedle1)), 7);
	EXPECT_EQ(Common::searchBackwards(haystack, kNeedle1, sizeof(kNeedle1), 3), SIZE_MAX);

	EXPECT_EQ(Common::searchBackwards(haystack, kNeedle2, sizeof(kNeedle2)), SIZE_MAX);

	EXPECT_EQ(Common::searchBackwards(haystack, 0, 0), SIZE_MAX);
}
