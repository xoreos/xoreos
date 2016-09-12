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
 *  Unit tests for Windows codepage 1251 encoding functions.
 */

#include "gtest/gtest.h"

#include "src/common/error.h"
#include "src/common/encoding.h"
#include "src/common/memreadstream.h"
#include "src/common/memwritestream.h"

#include "tests/common/encoding.h"

// The name and enum of the encoding we're testing
#define XOREOS_ENCODINGNAME EncodingCP1251
static const Common::Encoding kEncoding = Common::kEncodingCP1251;

// -- General encoding feature tests, can't be generalized --

GTEST_TEST(XOREOS_ENCODINGNAME, getBytesPerCodepoint) {
	testSupport(kEncoding);

	EXPECT_EQ(Common::getBytesPerCodepoint(kEncoding), 1);
}

GTEST_TEST(XOREOS_ENCODINGNAME, isValidCodePoint) {
	testSupport(kEncoding);

	EXPECT_TRUE(Common::isValidCodepoint(kEncoding, 0x20));
	EXPECT_FALSE(Common::isValidCodepoint(kEncoding, 0x98));
}

// -- Generalized encoding function tests --

// Example string with terminating 0
static const byte stringData0    [] = { 0xD4, 0xEE, 0xEE, 0xE1, 0xE0, 0xF0, '\0' };
// Example string with terminating 0 and garbage following
static const byte stringData0X   [] = { 0xD4, 0xEE, 0xEE, 0xE1, 0xE0, 0xF0, '\0', 'x' };
// Example string without terminating 0 and with garbage following
static const byte stringDataX    [] = { 0xD4, 0xEE, 0xEE, 0xE1, 0xE0, 0xF0, 'x' };
// Example string with line end and garbage following
static const byte stringDataLineX[] = { 0xD4, 0xEE, 0xEE, 0xE1, 0xE0, 0xF0, '\r', '\n', 'x' };

// Number of bytes in the example string without terminating 0 and without garbage
static const size_t stringBytes = 6;
// Number of characters in the example string without terminating 0 and without garbage
static const size_t stringChars = 6;

// The example string encoded as UTF-8 (Foobar, transcripted into the Cyrillic alphabet)
static const Common::UString stringUString = Common::UString("\xd0""\xa4""\xd0""\xbe""\xd0""\xbe""\xd0""\xb1""\xd0""\xb0""\xd1""\x80");

// The actual tests live here
#include "tests/common/encoding_tests.h"
