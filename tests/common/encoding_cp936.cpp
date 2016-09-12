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
 *  Unit tests for Windows codepage 936 encoding functions.
 */

#include "gtest/gtest.h"

#include "src/common/error.h"
#include "src/common/encoding.h"
#include "src/common/memreadstream.h"
#include "src/common/memwritestream.h"

#include "tests/common/encoding.h"

// The name and enum of the encoding we're testing
#define XOREOS_ENCODINGNAME EncodingCP936
static const Common::Encoding kEncoding = Common::kEncodingCP936;

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
static const byte stringData0    [] = { 0xD0, 0xB4, 0xCD, 0xAC, 0xCE, 0xF7, 0xCE, 0xAC, 0xB9, 0xB9, 0xB6, 0xCE, 0xCF, 0xB8, 0xBF, 0xB4, 0xA3, 0xAC, '\0', };
// Example string with terminating 0 and garbage following
static const byte stringData0X   [] = { 0xD0, 0xB4, 0xCD, 0xAC, 0xCE, 0xF7, 0xCE, 0xAC, 0xB9, 0xB9, 0xB6, 0xCE, 0xCF, 0xB8, 0xBF, 0xB4, 0xA3, 0xAC, '\0', 'x' };
// Example string without terminating 0 and with garbage following
static const byte stringDataX    [] = { 0xD0, 0xB4, 0xCD, 0xAC, 0xCE, 0xF7, 0xCE, 0xAC, 0xB9, 0xB9, 0xB6, 0xCE, 0xCF, 0xB8, 0xBF, 0xB4, 0xA3, 0xAC, 'x' };
// Example string with line end and garbage following
static const byte stringDataLineX[] = { 0xD0, 0xB4, 0xCD, 0xAC, 0xCE, 0xF7, 0xCE, 0xAC, 0xB9, 0xB9, 0xB6, 0xCE, 0xCF, 0xB8, 0xBF, 0xB4, 0xA3, 0xAC, '\r', '\n', 'x' };

// Number of bytes in the example string without terminating 0 and without garbage
static const size_t stringBytes = 18;
// Number of characters in the example string without terminating 0 and without garbage
static const size_t stringChars =  9;

// The example string encoded as UTF-8 (random words from a Lorem Ipsum generator)
static const Common::UString stringUString = Common::UString("\xe5""\x86""\x99""\xe5""\x90""\x8c""\xe8""\xa5""\xbf""\xe7""\xbb""\xb4""\xe6""\x9e""\x84""\xe6""\xae""\xb5""\xe7""\xbb""\x86""\xe7""\x9c""\x8b""\xef""\xbc""\x8c");

// The actual tests live here
#include "tests/common/encoding_tests.h"
