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
 *  Unit tests for our bit stream writer.
 */

#include "gtest/gtest.h"

#include "src/common/util.h"
#include "src/common/memwritestream.h"
#include "src/common/bitstreamwriter.h"

static void writeBitStream(Common::BitStreamWriter &bitStream, const byte (&data)[10]) {
	for (size_t i = 0; i < 8; i++)
		bitStream.putBit(data[i]);

	bitStream.putBits(data[8], 4);
	bitStream.putBits(data[9], 4);

	bitStream.flush();
}

static void testBitStream(const byte *data, size_t dataSize, const byte *comp, size_t compSize) {
	ASSERT_EQ(dataSize, compSize);

	for (size_t i = 0; i < dataSize; i++)
		EXPECT_EQ(data[i], comp[i]) << "At index " << i;
}

GTEST_TEST(BitStreamWriter, BitStreamWriter8MSB) {
	static const byte data[10] = { 0, 0, 0, 1, 0, 0, 1, 0, 0x03, 0x04 };
	static const byte comp[2] = { 0x12, 0x34 };

	Common::MemoryWriteStreamDynamic stream(true);
	Common::BitStreamWriter8MSB bitStream(stream);

	writeBitStream(bitStream, data);
	testBitStream(stream.getData(), stream.size(), comp, ARRAYSIZE(comp));
}

GTEST_TEST(BitStreamWriter, BitStreamWriter8LSB) {
	static const byte data[10] = { 0, 1, 0, 0, 1, 0, 0, 0, 0x04, 0x03 };
	static const byte comp[2] = { 0x12, 0x34 };

	Common::MemoryWriteStreamDynamic stream(true);
	Common::BitStreamWriter8LSB bitStream(stream);

	writeBitStream(bitStream, data);
	testBitStream(stream.getData(), stream.size(), comp, ARRAYSIZE(comp));
}

GTEST_TEST(BitStreamWriter, BitStreamWriter16LEMSB) {
	static const byte data[10] = { 0, 0, 0, 1, 0, 0, 1, 0, 0x03, 0x04 };
	static const byte comp[2] = { 0x34, 0x12 };

	Common::MemoryWriteStreamDynamic stream(true);
	Common::BitStreamWriter16LEMSB bitStream(stream);

	writeBitStream(bitStream, data);
	testBitStream(stream.getData(), stream.size(), comp, ARRAYSIZE(comp));
}

GTEST_TEST(BitStreamWriter, BitStreamWriter16LELSB) {
	static const byte data[10] = { 0, 1, 0, 0, 1, 0, 0, 0, 0x04, 0x03 };
	static const byte comp[2] = { 0x12, 0x34 };

	Common::MemoryWriteStreamDynamic stream(true);
	Common::BitStreamWriter16LELSB bitStream(stream);

	writeBitStream(bitStream, data);
	testBitStream(stream.getData(), stream.size(), comp, ARRAYSIZE(comp));
}

GTEST_TEST(BitStreamWriter, BitStreamWriter16BEMSB) {
	static const byte data[10] = { 0, 0, 0, 1, 0, 0, 1, 0, 0x03, 0x04 };
	static const byte comp[2] = { 0x12, 0x34 };

	Common::MemoryWriteStreamDynamic stream(true);
	Common::BitStreamWriter16BEMSB bitStream(stream);

	writeBitStream(bitStream, data);
	testBitStream(stream.getData(), stream.size(), comp, ARRAYSIZE(comp));
}

GTEST_TEST(BitStreamWriter, BitStreamWriter16BELSB) {
	static const byte data[10] = { 0, 1, 0, 0, 1, 0, 0, 0, 0x04, 0x03 };
	static const byte comp[2] = { 0x34, 0x12 };

	Common::MemoryWriteStreamDynamic stream(true);
	Common::BitStreamWriter16BELSB bitStream(stream);

	writeBitStream(bitStream, data);
	testBitStream(stream.getData(), stream.size(), comp, ARRAYSIZE(comp));
}

GTEST_TEST(BitStreamWriter, BitStreamWriter32LEMSB) {
	static const byte data[10] = { 0, 0, 0, 1, 0, 0, 1, 0, 0x03, 0x04 };
	static const byte comp[4] = { 0x00, 0x00, 0x34, 0x12 };

	Common::MemoryWriteStreamDynamic stream(true);
	Common::BitStreamWriter32LEMSB bitStream(stream);

	writeBitStream(bitStream, data);
	testBitStream(stream.getData(), stream.size(), comp, ARRAYSIZE(comp));
}

GTEST_TEST(BitStreamWriter, BitStreamWriter32LELSB) {
	static const byte data[10] = { 0, 1, 0, 0, 1, 0, 0, 0, 0x04, 0x03 };
	static const byte comp[4] = { 0x12, 0x34, 0x00, 0x00 };

	Common::MemoryWriteStreamDynamic stream(true);
	Common::BitStreamWriter32LELSB bitStream(stream);

	writeBitStream(bitStream, data);
	testBitStream(stream.getData(), stream.size(), comp, ARRAYSIZE(comp));
}

GTEST_TEST(BitStreamWriter, BitStreamWriter32BEMSB) {
	static const byte data[10] = { 0, 0, 0, 1, 0, 0, 1, 0, 0x03, 0x04 };
	static const byte comp[4] = { 0x12, 0x34, 0x00, 0x00 };

	Common::MemoryWriteStreamDynamic stream(true);
	Common::BitStreamWriter32BEMSB bitStream(stream);

	writeBitStream(bitStream, data);
	testBitStream(stream.getData(), stream.size(), comp, ARRAYSIZE(comp));
}

GTEST_TEST(BitStreamWriter, BitStreamWriter32BELSB) {
	static const byte data[10] = { 0, 1, 0, 0, 1, 0, 0, 0, 0x04, 0x03 };
	static const byte comp[4] = { 0x00, 0x00, 0x34, 0x12 };

	Common::MemoryWriteStreamDynamic stream(true);
	Common::BitStreamWriter32BELSB bitStream(stream);

	writeBitStream(bitStream, data);
	testBitStream(stream.getData(), stream.size(), comp, ARRAYSIZE(comp));
}

GTEST_TEST(BitStreamWriter, BitStreamWriter64LEMSB) {
	static const byte data[10] = { 0, 0, 0, 1, 0, 0, 1, 0, 0x03, 0x04 };
	static const byte comp[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x34, 0x12 };

	Common::MemoryWriteStreamDynamic stream(true);
	Common::BitStreamWriter64LEMSB bitStream(stream);

	writeBitStream(bitStream, data);
	testBitStream(stream.getData(), stream.size(), comp, ARRAYSIZE(comp));
}

GTEST_TEST(BitStreamWriter, BitStreamWriter64LELSB) {
	static const byte data[10] = { 0, 1, 0, 0, 1, 0, 0, 0, 0x04, 0x03 };
	static const byte comp[8] = { 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	Common::MemoryWriteStreamDynamic stream(true);
	Common::BitStreamWriter64LELSB bitStream(stream);

	writeBitStream(bitStream, data);
	testBitStream(stream.getData(), stream.size(), comp, ARRAYSIZE(comp));
}

GTEST_TEST(BitStreamWriter, BitStreamWriter64BEMSB) {
	static const byte data[10] = { 0, 0, 0, 1, 0, 0, 1, 0, 0x03, 0x04 };
	static const byte comp[8] = { 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	Common::MemoryWriteStreamDynamic stream(true);
	Common::BitStreamWriter64BEMSB bitStream(stream);

	writeBitStream(bitStream, data);
	testBitStream(stream.getData(), stream.size(), comp, ARRAYSIZE(comp));
}

GTEST_TEST(BitStreamWriter, BitStreamWriter64BELSB) {
	static const byte data[10] = { 0, 1, 0, 0, 1, 0, 0, 0, 0x04, 0x03 };
	static const byte comp[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x34, 0x12 };

	Common::MemoryWriteStreamDynamic stream(true);
	Common::BitStreamWriter64BELSB bitStream(stream);

	writeBitStream(bitStream, data);
	testBitStream(stream.getData(), stream.size(), comp, ARRAYSIZE(comp));
}
