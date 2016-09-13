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
 *  Unit tests for our MD5 digest implementation.
 */

#include <cstring>

#include <vector>

#include "gtest/gtest.h"

#include "src/common/md5.h"
#include "src/common/ustring.h"
#include "src/common/memreadstream.h"

static const char *kString = "Foobar";
static const byte  kData[] = { 0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF };

static const byte kDigestString[] = { 0x89, 0xD5, 0x73, 0x9B, 0xAA, 0xBB, 0xBE, 0x65,
                                      0xBE, 0x35, 0xCB, 0xE6, 0x1C, 0x88, 0xE0, 0x6D };
static const byte kDigestData  [] = { 0x79, 0x9D, 0x4F, 0x0B, 0xEC, 0x27, 0xAF, 0xF8,
                                      0x9D, 0xE3, 0x0B, 0x35, 0xE3, 0xCA, 0x43, 0x32 };

template<size_t N>
static void createVector(std::vector<byte> &v, const byte (&data)[N]) {
	v.resize(N);

	memcpy(&v[0], data, N);
}

template<size_t N>
static void compareData(const std::vector<byte> &v, const byte (&data)[N]) {
	ASSERT_EQ(v.size(), N);

	for (size_t i = 0; i < N; i++)
		EXPECT_EQ(v[i], data[i]) << "At index " << i;
}

GTEST_TEST(MD5, hashString) {
	std::vector<byte> digest;

	Common::hashMD5(kString, digest);

	compareData(digest, kDigestString);
}

GTEST_TEST(MD5, hashData) {
	std::vector<byte> digest;

	Common::hashMD5(kData, sizeof(kData), digest);

	compareData(digest, kDigestData);
}

GTEST_TEST(MD5, hashStream) {
	std::vector<byte> digest;

	Common::MemoryReadStream stream(kData);
	Common::hashMD5(stream, digest);

	compareData(digest, kDigestData);
}

GTEST_TEST(MD5, hashVector) {
	std::vector<byte> digest;

	std::vector<byte> data;
	createVector(data, kData);

	Common::hashMD5(data, digest);

	compareData(digest, kDigestData);
}

GTEST_TEST(MD5, compareString) {
	std::vector<byte> digest;
	createVector(digest, kDigestString);

	EXPECT_TRUE(Common::compareMD5Digest(kString, digest));
}

GTEST_TEST(MD5, compareData) {
	std::vector<byte> digest;
	createVector(digest, kDigestData);

	EXPECT_TRUE(Common::compareMD5Digest(kData, sizeof(kData), digest));
}

GTEST_TEST(MD5, compareStream) {
	std::vector<byte> digest;
	createVector(digest, kDigestData);

	Common::MemoryReadStream stream(kData);
	EXPECT_TRUE(Common::compareMD5Digest(stream, digest));
}

GTEST_TEST(MD5, compareVector) {
	std::vector<byte> digest;
	createVector(digest, kDigestData);

	std::vector<byte> data;
	createVector(data, kData);

	EXPECT_TRUE(Common::compareMD5Digest(data, digest));
}
