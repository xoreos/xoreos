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
 *  Unit tests for our bit stream.
 */

#include "gtest/gtest.h"

#include "src/common/util.h"
#include "src/common/memreadstream.h"
#include "src/common/bitstream.h"

GTEST_TEST(BitStream, skip) {
	static const byte data[4] = { 0 };
	Common::MemoryReadStream stream(data);
	Common::BitStream8MSB bitStream(stream);

	EXPECT_EQ(bitStream.pos(), 0);
	EXPECT_EQ(bitStream.size(), 8 * ARRAYSIZE(data));

	bitStream.skip(1);
	bitStream.skip(2);
	bitStream.skip(3);

	EXPECT_EQ(bitStream.pos(), 6);

	bitStream.rewind();
	EXPECT_EQ(bitStream.pos(), 0);
}

static void readBitStream(Common::BitStream &bitStream, byte (&data)[11]) {
	for (size_t i = 0; i < 8; i++)
		data[i] = bitStream.getBit();

	data[8] = bitStream.getBits(4);
	data[9] = bitStream.getBits(4);

	uint32_t x = 1;
	bitStream.addBit(x, 1);

	data[10] = x;
}

static void testBitStream(Common::BitStream &bitStream, const byte (&compValues)[11]) {
	byte data[11];
	readBitStream(bitStream, data);

	for (size_t i = 0; i < ARRAYSIZE(compValues); i++)
		EXPECT_EQ(data[i], compValues[i]) << "At index " << i;
}

GTEST_TEST(BitStream, BitStream8MSB) {
	static const byte compValues[11] = { 0, 0, 0, 1, 0, 0, 1, 0, 0x03, 0x04, 0x02 };
	static const byte data[8] = { 0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF };
	Common::MemoryReadStream stream(data);
	Common::BitStream8MSB bitStream(stream);

	testBitStream(bitStream, compValues);
}

GTEST_TEST(BitStream, BitStream8LSB) {
	static const byte compValues[11] = { 0, 1, 0, 0, 1, 0, 0, 0, 0x04, 0x03, 0x01 };
	static const byte data[8] = { 0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF };
	Common::MemoryReadStream stream(data);
	Common::BitStream8LSB bitStream(stream);

	testBitStream(bitStream, compValues);
}

GTEST_TEST(BitStream, BitStream16LEMSB) {
	static const byte compValues[11] = { 0, 0, 1, 1, 0, 1, 0, 0, 0x01, 0x02, 0x02 };
	static const byte data[8] = { 0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF };
	Common::MemoryReadStream stream(data);
	Common::BitStream16LEMSB bitStream(stream);

	testBitStream(bitStream, compValues);
}

GTEST_TEST(BitStream, BitStream16LELSB) {
	static const byte compValues[11] = { 0, 1, 0, 0, 1, 0, 0, 0, 0x04, 0x03, 0x01 };
	static const byte data[8] = { 0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF };
	Common::MemoryReadStream stream(data);
	Common::BitStream16LELSB bitStream(stream);

	testBitStream(bitStream, compValues);
}

GTEST_TEST(BitStream, BitStream16BEMSB) {
	static const byte compValues[11] = { 0, 0, 0, 1, 0, 0, 1, 0, 0x03, 0x04, 0x02 };
	static const byte data[8] = { 0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF };
	Common::MemoryReadStream stream(data);
	Common::BitStream16BEMSB bitStream(stream);

	testBitStream(bitStream, compValues);
}

GTEST_TEST(BitStream, BitStream16BELSB) {
	static const byte compValues[11] = { 0, 0, 1, 0, 1, 1, 0, 0, 0x02, 0x01, 0x01 };
	static const byte data[8] = { 0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF };
	Common::MemoryReadStream stream(data);
	Common::BitStream16BELSB bitStream(stream);

	testBitStream(bitStream, compValues);
}

GTEST_TEST(BitStream, BitStream32LEMSB) {
	static const byte compValues[11] = { 0, 1, 1, 1, 1, 0, 0, 0, 0x05, 0x06, 0x02 };
	static const byte data[8] = { 0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF };
	Common::MemoryReadStream stream(data);
	Common::BitStream32LEMSB bitStream(stream);

	testBitStream(bitStream, compValues);
}

GTEST_TEST(BitStream, BitStream32LELSB) {
	static const byte compValues[11] = { 0, 1, 0, 0, 1, 0, 0, 0, 0x04, 0x03, 0x01 };
	static const byte data[8] = { 0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF };
	Common::MemoryReadStream stream(data);
	Common::BitStream32LELSB bitStream(stream);

	testBitStream(bitStream, compValues);
}

GTEST_TEST(BitStream, BitStream32BEMSB) {
	static const byte compValues[11] = { 0, 0, 0, 1, 0, 0, 1, 0, 0x03, 0x04, 0x02 };
	static const byte data[8] = { 0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF };
	Common::MemoryReadStream stream(data);
	Common::BitStream32BEMSB bitStream(stream);

	testBitStream(bitStream, compValues);
}

GTEST_TEST(BitStream, BitStream32BELSB) {
	static const byte compValues[11] = { 0, 0, 0, 1, 1, 1, 1, 0, 0x06, 0x05, 0x01 };
	static const byte data[8] = { 0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF };
	Common::MemoryReadStream stream(data);
	Common::BitStream32BELSB bitStream(stream);

	testBitStream(bitStream, compValues);
}

GTEST_TEST(BitStream, BitStream64LEMSB) {
	static const byte compValues[11] = { 1, 1, 1, 0, 1, 1, 1, 1, 0x0C, 0x0D, 0x03 };
	static const byte data[8] = { 0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF };
	Common::MemoryReadStream stream(data);
	Common::BitStream64LEMSB bitStream(stream);

	testBitStream(bitStream, compValues);
}

GTEST_TEST(BitStream, BitStream64LELSB) {
	static const byte compValues[11] = { 0, 1, 0, 0, 1, 0, 0, 0, 0x04, 0x03, 0x01 };
	static const byte data[8] = { 0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF };
	Common::MemoryReadStream stream(data);
	Common::BitStream64LELSB bitStream(stream);

	testBitStream(bitStream, compValues);
}

GTEST_TEST(BitStream, BitStream64BEMSB) {
	static const byte compValues[11] = { 0, 0, 0, 1, 0, 0, 1, 0, 0x03, 0x04, 0x02 };
	static const byte data[8] = { 0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF };
	Common::MemoryReadStream stream(data);
	Common::BitStream64BEMSB bitStream(stream);

	testBitStream(bitStream, compValues);
}

GTEST_TEST(BitStream, BitStream64BELSB) {
	static const byte compValues[11] = { 1, 1, 1, 1, 0, 1, 1, 1, 0x0D, 0x0C, 0x03 };
	static const byte data[8] = { 0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF };
	Common::MemoryReadStream stream(data);
	Common::BitStream64BELSB bitStream(stream);

	testBitStream(bitStream, compValues);
}
