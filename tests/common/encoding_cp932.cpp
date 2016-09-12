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
 *  Unit tests for Windows codepage 932 encoding functions.
 */

#include "gtest/gtest.h"

#include "src/common/error.h"
#include "src/common/encoding.h"
#include "src/common/memreadstream.h"
#include "src/common/memwritestream.h"

#include "tests/common/encoding.h"

// The name and enum of the encoding we're testing
#define XOREOS_ENCODINGNAME EncodingCP932
static const Common::Encoding kEncoding = Common::kEncodingCP932;

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
static const byte stringData0    [] = { 0x8B, 0x5F, 0x89, 0x80, 0x90, 0xB8, 0x8E, 0xC9, 0x82, 0xCC, 0x8F, 0xE0, 0x82, 0xCC, 0xE3, 0xDF, 0x81, 0x41, '\0', };
// Example string with terminating 0 and garbage following
static const byte stringData0X   [] = { 0x8B, 0x5F, 0x89, 0x80, 0x90, 0xB8, 0x8E, 0xC9, 0x82, 0xCC, 0x8F, 0xE0, 0x82, 0xCC, 0xE3, 0xDF, 0x81, 0x41, '\0', 'x' };
// Example string without terminating 0 and with garbage following
static const byte stringDataX    [] = { 0x8B, 0x5F, 0x89, 0x80, 0x90, 0xB8, 0x8E, 0xC9, 0x82, 0xCC, 0x8F, 0xE0, 0x82, 0xCC, 0xE3, 0xDF, 0x81, 0x41, 'x' };
// Example string with line end and garbage following
static const byte stringDataLineX[] = { 0x8B, 0x5F, 0x89, 0x80, 0x90, 0xB8, 0x8E, 0xC9, 0x82, 0xCC, 0x8F, 0xE0, 0x82, 0xCC, 0xE3, 0xDF, 0x81, 0x41, '\r', '\n', 'x' };

// Number of bytes in the example string without terminating 0 and without garbage
static const size_t stringBytes = 18;
// Number of characters in the example string without terminating 0 and without garbage
static const size_t stringChars =  9;

// The example string encoded as UTF-8 (part of the opening to the The Tale of the Heike)
static const Common::UString stringUString = Common::UString("\xe7""\xa5""\x87""\xe5""\x9c""\x92""\xe7""\xb2""\xbe""\xe8""\x88""\x8e""\xe3""\x81""\xae""\xe9""\x90""\x98""\xe3""\x81""\xae""\xe8""\x81""\xb2""\xe3""\x80""\x81");

// The actual tests live here
#include "tests/common/encoding_tests.h"
