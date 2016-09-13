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
 *  Unit tests for our generic string hash functions.
 */

#include "gtest/gtest.h"

#include "src/common/hash.h"

static const char *kString = "Foobar";

GTEST_TEST(Hash, DJB2) {
	EXPECT_EQ(Common::hashString(kString, Common::kHashDJB2), 0xB33F4C9E);
}

GTEST_TEST(Hash, FNV32) {
	EXPECT_EQ(Common::hashString(kString, Common::kHashFNV32), 0xED18E8C2);
}

GTEST_TEST(Hash, FNV64) {
	EXPECT_EQ(Common::hashString(kString, Common::kHashFNV64), UINT64_C(0x744E9FFF32CA0A22));
}

GTEST_TEST(Hash, CRC32) {
	EXPECT_EQ(Common::hashString(kString, Common::kHashCRC32), 0x995A1AA3);
}

GTEST_TEST(Hash, DJB2Encoding) {
	EXPECT_EQ(Common::hashString(kString, Common::kHashDJB2, Common::kEncodingUTF16LE), 0xD11D54FE);
}

GTEST_TEST(Hash, FNV32Encoding) {
	EXPECT_EQ(Common::hashString(kString, Common::kHashFNV32, Common::kEncodingUTF16LE), 0xCE5005F0);
}

GTEST_TEST(Hash, FNV64Encoding) {
	EXPECT_EQ(Common::hashString(kString, Common::kHashFNV64, Common::kEncodingUTF16LE), UINT64_C(0xA73456F669A95770));
}

GTEST_TEST(Hash, CRC32Encoding) {
	EXPECT_EQ(Common::hashString(kString, Common::kHashCRC32, Common::kEncodingUTF16LE), 0x56031CD6);
}

GTEST_TEST(Hash, formatHash) {
	EXPECT_STREQ(Common::formatHash(UINT64_C(0x1234567890ABCDEF)).c_str(), "0x1234567890ABCDEF");
}
