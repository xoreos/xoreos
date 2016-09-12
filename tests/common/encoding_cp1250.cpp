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
 *  Unit tests for Windows codepage 1250 encoding functions.
 */

#include "gtest/gtest.h"

#include "src/common/error.h"
#include "src/common/encoding.h"
#include "src/common/memreadstream.h"
#include "src/common/memwritestream.h"

#include "tests/common/encoding.h"

// The name and enum of the encoding we're testing
#define XOREOS_ENCODINGNAME EncodingCP1250
static const Common::Encoding kEncoding = Common::kEncodingCP1250;

// -- General encoding feature tests, can't be generalized --

GTEST_TEST(XOREOS_ENCODINGNAME, getBytesPerCodepoint) {
	testSupport(kEncoding);

	EXPECT_EQ(Common::getBytesPerCodepoint(kEncoding), 1);
}

GTEST_TEST(XOREOS_ENCODINGNAME, isValidCodePoint) {
	testSupport(kEncoding);

	EXPECT_TRUE(Common::isValidCodepoint(kEncoding, 0x20));
	EXPECT_FALSE(Common::isValidCodepoint(kEncoding, 0x81));
}

// -- Generalized encoding function tests --

// Example string with terminating 0
static const byte stringData0    [] = { 0xA3, 0xF6, 0xF6, 0xE8, 0xE4, 'r', '\0', };
// Example string with terminating 0 and garbage following
static const byte stringData0X   [] = { 0xA3, 0xF6, 0xF6, 0xE8, 0xE4, 'r', '\0', 'x' };
// Example string without terminating 0 and with garbage following
static const byte stringDataX    [] = { 0xA3, 0xF6, 0xF6, 0xE8, 0xE4, 'r', 'x' };
// Example string with line end and garbage following
static const byte stringDataLineX[] = { 0xA3, 0xF6, 0xF6, 0xE8, 0xE4, 'r', '\r', '\n', 'x' };

// Number of bytes in the example string without terminating 0 and without garbage
static const size_t stringBytes = 6;
// Number of characters in the example string without terminating 0 and without garbage
static const size_t stringChars = 6;

// The example string encoded as UTF-8 (Loocar, L with stroke, o and a with diaereses, c with caron)
static const Common::UString stringUString = Common::UString("\xc5""\x81""\xc3""\xb6""\xc3""\xb6""\xc4""\x8d""\xc3""\xa4""r");

// The actual tests live here
#include "tests/common/encoding_tests.h"
