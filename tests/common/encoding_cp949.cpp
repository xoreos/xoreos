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
 *  Unit tests for Windows codepage 949 encoding functions.
 */

#include "gtest/gtest.h"

#include "src/common/error.h"
#include "src/common/encoding.h"
#include "src/common/memreadstream.h"
#include "src/common/memwritestream.h"

#include "tests/common/encoding.h"

// The name and enum of the encoding we're testing
#define XOREOS_ENCODINGNAME EncodingCP949
static const Common::Encoding kEncoding = Common::kEncodingCP949;

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
static const byte stringData0    [] = { 0xC7, 0xCF, 0xB4, 0xC3, 0xB0, 0xFA, 0x20, 0xB9, 0xD9, 0xB6, 0xF7, 0xB0, 0xFA, 0x20, 0xBA, 0xB0, 0xB0, 0xFA, 0x20, 0xBD, 0xC3, '\0', };
// Example string with terminating 0 and garbage following
static const byte stringData0X   [] = { 0xC7, 0xCF, 0xB4, 0xC3, 0xB0, 0xFA, 0x20, 0xB9, 0xD9, 0xB6, 0xF7, 0xB0, 0xFA, 0x20, 0xBA, 0xB0, 0xB0, 0xFA, 0x20, 0xBD, 0xC3, '\0', 'x' };
// Example string without terminating 0 and with garbage following
static const byte stringDataX    [] = { 0xC7, 0xCF, 0xB4, 0xC3, 0xB0, 0xFA, 0x20, 0xB9, 0xD9, 0xB6, 0xF7, 0xB0, 0xFA, 0x20, 0xBA, 0xB0, 0xB0, 0xFA, 0x20, 0xBD, 0xC3, 'x' };
// Example string with line end and garbage following
static const byte stringDataLineX[] = { 0xC7, 0xCF, 0xB4, 0xC3, 0xB0, 0xFA, 0x20, 0xB9, 0xD9, 0xB6, 0xF7, 0xB0, 0xFA, 0x20, 0xBA, 0xB0, 0xB0, 0xFA, 0x20, 0xBD, 0xC3, '\r', '\n', 'x' };

// Number of bytes in the example string without terminating 0 and without garbage
static const size_t stringBytes = 21;
// Number of characters in the example string without terminating 0 and without garbage
static const size_t stringChars = 12;

// The example string encoded as UTF-8 ("Sky, Wind, Star and Poem", title of a collection of poems by Yun Dong-ju)
static const Common::UString stringUString = Common::UString("\xed""\x95""\x98""\xeb""\x8a""\x98""\xea""\xb3""\xbc""\x20""\xeb""\xb0""\x94""\xeb""\x9e""\x8c""\xea""\xb3""\xbc""\x20""\xeb""\xb3""\x84""\xea""\xb3""\xbc""\x20""\xec""\x8b""\x9c");

// The actual tests live here
#include "tests/common/encoding_tests.h"
