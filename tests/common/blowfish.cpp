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
 *  Unit tests for our Blowfish implementation.
 */

#include <vector>

#include "gtest/gtest.h"

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/memreadstream.h"
#include "src/common/blowfish.h"

static const byte kClearText[]  = { 'F', 'o', 'o', 'b', 'a', 'r', ' ', 'B', 'a', 'r', 'f', 'o', 'o' };
static const byte kKey[]        = { 'T', 'h', 'i', 's', ' ', 'a', ' ', 'K', 'e', 'y', '!' };
static const byte kCypherText[] = { 0x2B, 0x9B, 0x10, 0x5F, 0xF2, 0x5B, 0x58, 0xB6,
                                    0x20, 0xA8, 0xC2, 0xF8, 0x98, 0x1E, 0xA4, 0xE8 };

static void createKey(std::vector<byte> &key) {
	key.resize(ARRAYSIZE(kKey));

	for (size_t i = 0; i < ARRAYSIZE(kKey); i++)
		key[i] = kKey[i];
}

GTEST_TEST(Blowfish, encrypt) {
	Common::MemoryReadStream clearText(kClearText);

	std::vector<byte> key;
	createKey(key);

	Common::MemoryReadStream *cipherText = Common::encryptBlowfishEBC(clearText, key);
	ASSERT_EQ(cipherText->size(), ARRAYSIZE(kCypherText));

	for (size_t i = 0; i < ARRAYSIZE(kCypherText); i++)
		EXPECT_EQ(cipherText->readByte(), kCypherText[i]) << "At index " << i;

	delete cipherText;
}

GTEST_TEST(Blowfish, decrypt) {
	Common::MemoryReadStream cipherText(kCypherText);

	std::vector<byte> key;
	createKey(key);

	Common::MemoryReadStream *clearText = Common::decryptBlowfishEBC(cipherText, key);
	ASSERT_GE(clearText->size(), ARRAYSIZE(kClearText));

	for (size_t i = 0; i < ARRAYSIZE(kClearText); i++)
		EXPECT_EQ(clearText->readByte(), kClearText[i]) << "At index " << i;

	delete clearText;
}

GTEST_TEST(Blowfish, misalign) {
	Common::MemoryReadStream cipherText(kCypherText, 7);

	std::vector<byte> key;
	createKey(key);

	EXPECT_THROW(Common::decryptBlowfishEBC(cipherText, key), Common::Exception);
}
