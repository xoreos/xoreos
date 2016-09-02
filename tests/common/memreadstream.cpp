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
 *  Unit tests for our memory read stream.
 */

#include "gtest/gtest.h"

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/readstream.h"
#include "src/common/memreadstream.h"

GTEST_TEST(MemoryReadStream, size) {
	static const byte data[3] = { 0 };
	Common::MemoryReadStream stream(data);

	EXPECT_EQ(stream.size(), 3);
}

GTEST_TEST(MemoryReadStream, seek) {
	static const byte data[4] = { 0 };
	Common::MemoryReadStream stream(data);

	EXPECT_EQ(stream.pos(), 0);

	stream.seek(1);
	EXPECT_EQ(stream.pos(), 1);

	stream.seek(0);
	EXPECT_EQ(stream.pos(), 0);

	stream.seek(1, Common::MemoryReadStream::kOriginCurrent);
	stream.seek(1, Common::MemoryReadStream::kOriginCurrent);
	EXPECT_EQ(stream.pos(), 2);

	stream.skip(1);
	EXPECT_EQ(stream.pos(), 3);

	EXPECT_THROW(stream.seek(5), Common::Exception);
}

GTEST_TEST(MemoryReadStream, getData) {
	static const byte data[3] = { 0 };
	Common::MemoryReadStream stream(data);

	EXPECT_EQ(stream.getData(), data);
}

GTEST_TEST(MemoryReadStream, read) {
	static const byte data[3] = { 0 };
	Common::MemoryReadStream stream(data);

	EXPECT_FALSE(stream.eos());

	size_t readCount;
	byte readData[ARRAYSIZE(data)] = { 1 };

	readCount = stream.read(readData, ARRAYSIZE(data));

	EXPECT_EQ(readCount, ARRAYSIZE(data));
	EXPECT_FALSE(stream.eos());

	for (size_t i = 0; i < ARRAYSIZE(data); i++)
		EXPECT_EQ(readData[i], data[i]) << "At index " << i;

	readCount = stream.read(readData, ARRAYSIZE(data));

	EXPECT_EQ(readCount, 0);
	EXPECT_TRUE(stream.eos());
}

GTEST_TEST(MemoryReadStream, readStream) {
	static const byte data[3] = { 0x12, 0x34, 0x56 };
	Common::MemoryReadStream stream(data);

	Common::MemoryReadStream *streamRead = stream.readStream(ARRAYSIZE(data));

	EXPECT_EQ(streamRead->size(), ARRAYSIZE(data));
	for (size_t i = 0; i < ARRAYSIZE(data); i++)
		EXPECT_EQ(streamRead->readByte(), data[i]) << "At index " << i;

	delete streamRead;

	stream.seek(0);

	EXPECT_THROW(stream.readStream(ARRAYSIZE(data) + 1), Common::Exception);
}

GTEST_TEST(MemoryReadStream, readChar) {
	static const byte data[3] = { 0x12, 0x34, 0x56 };
	Common::MemoryReadStream stream(data);

	for (size_t i = 0; i < ARRAYSIZE(data); i++)
		EXPECT_EQ(stream.readChar(), data[i]) << "At index " << i;

	EXPECT_EQ(stream.readChar(), Common::MemoryReadStream::kEOF);
}

GTEST_TEST(MemoryReadStream, readByte) {
	static const byte data[1] = { 0x17 };
	Common::MemoryReadStream stream(data);

	EXPECT_EQ(stream.readByte(), 23);
	EXPECT_THROW(stream.readByte(), Common::Exception);
}

GTEST_TEST(MemoryReadStream, readSByte) {
	static const byte data[1] = { 0xE9 };
	Common::MemoryReadStream stream(data);

	EXPECT_EQ(stream.readSByte(), -23);
	EXPECT_THROW(stream.readSByte(), Common::Exception);
}

GTEST_TEST(MemoryReadStream, readUint16LE) {
	static const byte data[2] = { 0x34, 0x12 };
	Common::MemoryReadStream stream(data);

	EXPECT_EQ(stream.readUint16LE(), 4660);
	EXPECT_THROW(stream.readUint16LE(), Common::Exception);
}

GTEST_TEST(MemoryReadStream, readSint16LE) {
	static const byte data[2] = { 0xDC, 0xFE };
	Common::MemoryReadStream stream(data);

	EXPECT_EQ(stream.readSint16LE(), -292);
	EXPECT_THROW(stream.readSint16LE(), Common::Exception);
}

GTEST_TEST(MemoryReadStream, readUint16BE) {
	static const byte data[2] = { 0x12, 0x34 };
	Common::MemoryReadStream stream(data);

	EXPECT_EQ(stream.readUint16BE(), 4660);
	EXPECT_THROW(stream.readUint16BE(), Common::Exception);
}

GTEST_TEST(MemoryReadStream, readSint16BE) {
	static const byte data[2] = { 0xFE, 0xDC };
	Common::MemoryReadStream stream(data);

	EXPECT_EQ(stream.readSint16BE(), -292);
	EXPECT_THROW(stream.readSint16BE(), Common::Exception);
}

GTEST_TEST(MemoryReadStream, readUint32LE) {
	static const byte data[4] = { 0x78, 0x56, 0x34, 0x12 };
	Common::MemoryReadStream stream(data);

	EXPECT_EQ(stream.readUint32LE(), 305419896);
	EXPECT_THROW(stream.readUint32LE(), Common::Exception);
}

GTEST_TEST(MemoryReadStream, readSint32LE) {
	static const byte data[4] = { 0x09, 0xBA, 0xDC, 0xFE };
	Common::MemoryReadStream stream(data);

	EXPECT_EQ(stream.readSint32LE(), -19088887);
	EXPECT_THROW(stream.readSint32LE(), Common::Exception);
}

GTEST_TEST(MemoryReadStream, readUint32BE) {
	static const byte data[4] = { 0x12, 0x34, 0x56, 0x78 };
	Common::MemoryReadStream stream(data);

	EXPECT_EQ(stream.readUint32BE(), 305419896);
	EXPECT_THROW(stream.readUint32BE(), Common::Exception);
}

GTEST_TEST(MemoryReadStream, readSint32BE) {
	static const byte data[4] = { 0xFE, 0xDC, 0xBA, 0x09 };
	Common::MemoryReadStream stream(data);

	EXPECT_EQ(stream.readSint32BE(), -19088887);
	EXPECT_THROW(stream.readSint32BE(), Common::Exception);
}

GTEST_TEST(MemoryReadStream, readUint64LE) {
	static const byte data[8] = { 0xEF, 0xCD, 0xAB, 0x90, 0x78, 0x56, 0x34, 0x12 };
	Common::MemoryReadStream stream(data);

	EXPECT_EQ(stream.readUint64LE(), UINT64_C(1311768467294899695));
	EXPECT_THROW(stream.readUint64LE(), Common::Exception);
}

GTEST_TEST(MemoryReadStream, readSint64LE) {
	static const byte data[8] = { 0x21, 0x43, 0x65, 0x87, 0x09, 0xBA, 0xDC, 0xFE };
	Common::MemoryReadStream stream(data);

	EXPECT_EQ(stream.readSint64LE(), INT64_C(-81986143110479071));
	EXPECT_THROW(stream.readSint64LE(), Common::Exception);
}

GTEST_TEST(MemoryReadStream, readUint64BE) {
	static const byte data[8] = { 0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF };
	Common::MemoryReadStream stream(data);

	EXPECT_EQ(stream.readUint64BE(), UINT64_C(1311768467294899695));
	EXPECT_THROW(stream.readUint64BE(), Common::Exception);
}

GTEST_TEST(MemoryReadStream, readSint64BE) {
	static const byte data[8] = { 0xFE, 0xDC, 0xBA, 0x09, 0x87, 0x65, 0x43, 0x21 };
	Common::MemoryReadStream stream(data);

	EXPECT_EQ(stream.readSint64BE(), INT64_C(-81986143110479071));
	EXPECT_THROW(stream.readSint64BE(), Common::Exception);
}

GTEST_TEST(MemoryReadStream, readIEEEFloatLE) {
	static const byte data[4] = { 0x00, 0x00, 0x80, 0x3F };
	Common::MemoryReadStream stream(data);

	EXPECT_EQ(stream.readIEEEFloatLE(), 1.0f);
	EXPECT_THROW(stream.readIEEEFloatLE(), Common::Exception);
}

GTEST_TEST(MemoryReadStream, readIEEEFloatBE) {
	static const byte data[4] = { 0x3F, 0x80, 0x00, 0x00 };
	Common::MemoryReadStream stream(data);

	EXPECT_EQ(stream.readIEEEFloatBE(), 1.0f);
	EXPECT_THROW(stream.readIEEEFloatBE(), Common::Exception);
}

GTEST_TEST(MemoryReadStream, readIEEEDoubleLE) {
	static const byte data[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x3F };
	Common::MemoryReadStream stream(data);

	EXPECT_EQ(stream.readIEEEDoubleLE(), 1.0);
	EXPECT_THROW(stream.readIEEEDoubleLE(), Common::Exception);
}

GTEST_TEST(MemoryReadStream, readIEEEDoubleBE) {
	static const byte data[8] = { 0x3F, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	Common::MemoryReadStream stream(data);

	EXPECT_EQ(stream.readIEEEDoubleBE(), 1.0);
	EXPECT_THROW(stream.readIEEEDoubleBE(), Common::Exception);
}

GTEST_TEST(MemoryReadStreamEndian, streamEndianLE) {
	static const byte data[4] = { 0x78, 0x56, 0x34, 0x12 };
	Common::MemoryReadStreamEndian stream(data, sizeof(data), false);

	EXPECT_EQ(stream.readUint32(), 305419896);
	EXPECT_THROW(stream.readUint32(), Common::Exception);
}

GTEST_TEST(MemoryReadStreamEndian, streamEndianBE) {
	static const byte data[4] = { 0x12, 0x34, 0x56, 0x78 };
	Common::MemoryReadStreamEndian stream(data, sizeof(data), true);

	EXPECT_EQ(stream.readUint32(), 305419896);
	EXPECT_THROW(stream.readUint32(), Common::Exception);
}

GTEST_TEST(SubReadStream, fromMem) {
	static const byte data[5] = { 0x12, 0x34, 0x56, 0x78, 0x90 };
	Common::MemoryReadStream stream(data);

	stream.seek(1);
	Common::SubReadStream subStream(&stream, 3);

	byte readData[4] = { 0 };
	const size_t readCount = subStream.read(readData, 4);

	EXPECT_EQ(readCount, 3);
	EXPECT_TRUE(subStream.eos());

	EXPECT_EQ(readData[0], data[1]);
	EXPECT_EQ(readData[1], data[2]);
	EXPECT_EQ(readData[2], data[3]);
}

GTEST_TEST(SeekableSubReadStream, fromMem) {
	static const byte data[5] = { 0x12, 0x34, 0x56, 0x78, 0x90 };
	Common::MemoryReadStream stream(data);

	Common::SeekableSubReadStream subStream(&stream, 1, 4);

	byte readData[4] = { 0 };
	const size_t readCount = subStream.read(readData, 4);

	EXPECT_EQ(readCount, 3);
	EXPECT_TRUE(subStream.eos());

	EXPECT_EQ(readData[0], data[1]);
	EXPECT_EQ(readData[1], data[2]);
	EXPECT_EQ(readData[2], data[3]);

	subStream.seek(0);

	EXPECT_EQ(subStream.pos(), 0);
	EXPECT_FALSE(subStream.eos());
}

GTEST_TEST(SeekableSubReadStreamEndian, streamEndianLE) {
	static const byte data[4] = { 0x78, 0x56, 0x34, 0x12 };
	Common::MemoryReadStream stream(data);

	Common::SeekableSubReadStreamEndian subStream(&stream, 0, stream.size(), false);
	EXPECT_EQ(subStream.readUint32(), 305419896);
	EXPECT_THROW(subStream.readUint32(), Common::Exception);
}

GTEST_TEST(SeekableSubReadStreamEndian, streamEndianBE) {
	static const byte data[4] = { 0x12, 0x34, 0x56, 0x78 };
	Common::MemoryReadStream stream(data);

	Common::SeekableSubReadStreamEndian subStream(&stream, 0, stream.size(), true);
	EXPECT_EQ(subStream.readUint32(), 305419896);
	EXPECT_THROW(subStream.readUint32(), Common::Exception);
}
