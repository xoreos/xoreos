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
 *  Unit tests for Windows codepage 950 encoding functions.
 */

#include "gtest/gtest.h"

#include "src/common/error.h"
#include "src/common/encoding.h"
#include "src/common/memreadstream.h"
#include "src/common/memwritestream.h"

#include "tests/common/encoding.h"

// The name and enum of the encoding we're testing
#define XOREOS_ENCODINGNAME EncodingCP950
static const Common::Encoding kEncoding = Common::kEncodingCP950;

// -- General encoding feature tests, can't be generalized --

GTEST_TEST(XOREOS_ENCODINGNAME, getBytesPerCodepoint) {
	testSupport(kEncoding);

	EXPECT_THROW(Common::getBytesPerCodepoint(kEncoding), Common::Exception);
}

GTEST_TEST(XOREOS_ENCODINGNAME, isValidCodePoint) {
	testSupport(kEncoding);

	EXPECT_TRUE(Common::isValidCodepoint(kEncoding, 0x20));
}

// -- Generalized encoding function tests --

// Example string with terminating 0
static const byte stringData0    [] = { 0xA4, 0xA3, 0xA8, 0xBA, 0xBC, 0x40, 0xA5, 0x7E, 0xB0, 0x5F, 0xAC, 0xA1, 0xBC, 0x57, 0xA9, 0xCE, 0xA1, 0x43, '\0', };
// Example string with terminating 0 and garbage following
static const byte stringData0X   [] = { 0xA4, 0xA3, 0xA8, 0xBA, 0xBC, 0x40, 0xA5, 0x7E, 0xB0, 0x5F, 0xAC, 0xA1, 0xBC, 0x57, 0xA9, 0xCE, 0xA1, 0x43, '\0', 'x' };
// Example string without terminating 0 and with garbage following
static const byte stringDataX    [] = { 0xA4, 0xA3, 0xA8, 0xBA, 0xBC, 0x40, 0xA5, 0x7E, 0xB0, 0x5F, 0xAC, 0xA1, 0xBC, 0x57, 0xA9, 0xCE, 0xA1, 0x43, 'x' };
// Example string with line end and garbage following
static const byte stringDataLineX[] = { 0xA4, 0xA3, 0xA8, 0xBA, 0xBC, 0x40, 0xA5, 0x7E, 0xB0, 0x5F, 0xAC, 0xA1, 0xBC, 0x57, 0xA9, 0xCE, 0xA1, 0x43, '\r', '\n', 'x' };

// Number of bytes in the example string without terminating 0 and without garbage
static const size_t stringBytes = 18;
// Number of characters in the example string without terminating 0 and without garbage
static const size_t stringChars =  9;

// The example string encoded as UTF-8 (random words from a Lorem Ipsum generator)
static const Common::UString stringUString = Common::UString("\xe4""\xb8""\x8d""\xe9""\x82""\xa3""\xe5""\x8a""\x87""\xe5""\xa4""\x96""\xe8""\xb5""\xb7""\xe6""\xb4""\xbb""\xe5""\xa2""\x9e""\xe6""\x88""\x96""\xe3""\x80""\x82");

// The actual tests live here
#include "tests/common/encoding_tests.h"
