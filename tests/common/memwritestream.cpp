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
 *  Unit tests for our memory write stream.
 */

#include "gtest/gtest.h"

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/writestream.h"
#include "src/common/memwritestream.h"
#include "src/common/memreadstream.h"

GTEST_TEST(MemoryWriteStream, write) {
	static const byte writeData[5] = { 0x12, 0x34, 0x56, 0x78, 0x90 };

	byte data[ARRAYSIZE(writeData) - 1] = { 0 };
	Common::MemoryWriteStream stream(data);

	EXPECT_EQ(stream.size(), ARRAYSIZE(writeData) - 1);
	EXPECT_EQ(stream.pos(), 0);

	const size_t writeCount = stream.write(writeData, ARRAYSIZE(writeData));
	EXPECT_EQ(writeCount, ARRAYSIZE(data));

	EXPECT_EQ(stream.size(), ARRAYSIZE(writeData) - 1);
	EXPECT_EQ(stream.pos(), ARRAYSIZE(writeData) - 1);

	for (size_t i = 0; i < ARRAYSIZE(data); i++)
		EXPECT_EQ(data[i], writeData[i]) << "At index " << i;
}

GTEST_TEST(MemoryWriteStream, writeStream) {
	static const byte writeData[5] = { 0x12, 0x34, 0x56, 0x78, 0x90 };
	Common::MemoryReadStream writeStream(writeData);

	byte data[ARRAYSIZE(writeData) - 1] = { 0 };
	Common::MemoryWriteStream stream(data);

	const size_t writeCount = stream.writeStream(writeStream);
	EXPECT_EQ(writeCount, ARRAYSIZE(data));

	for (size_t i = 0; i < ARRAYSIZE(data); i++)
		EXPECT_EQ(data[i], writeData[i]) << "At index " << i;
}

GTEST_TEST(MemoryWriteStream, writeByte) {
	byte data[1] = { 0 };
	Common::MemoryWriteStream stream(data);

	stream.writeByte(23);
	EXPECT_THROW(stream.writeByte(23), Common::Exception);

	EXPECT_EQ(data[0], 0x17);
}

GTEST_TEST(MemoryWriteStream, writeSByte) {
	byte data[1] = { 0 };
	Common::MemoryWriteStream stream(data);

	stream.writeSByte(-23);
	EXPECT_THROW(stream.writeSByte(-23), Common::Exception);

	EXPECT_EQ(data[0], 0xE9);
}

static void compareData(const byte *a, const byte *b, size_t n) {
	for (size_t i = 0; i < n; i++)
		EXPECT_EQ(a[i], b[i]) << "At index " << i;
}

GTEST_TEST(MemoryWriteStream, writeUint16LE) {
	static const uint16 compValue = 4660;
	static const byte compData[2] = { 0x34, 0x12 };

	byte data[ARRAYSIZE(compData)] = { 0 };
	Common::MemoryWriteStream stream(data);

	stream.writeUint16LE(compValue);
	EXPECT_THROW(stream.writeUint16LE(compValue), Common::Exception);

	compareData(data, compData, ARRAYSIZE(compData));
}

GTEST_TEST(MemoryWriteStream, writeSint16LE) {
	static const int16 compValue = -292;
	static const byte compData[2] = { 0xDC, 0xFE };

	byte data[ARRAYSIZE(compData)] = { 0 };
	Common::MemoryWriteStream stream(data);

	stream.writeSint16LE(compValue);
	EXPECT_THROW(stream.writeSint16LE(compValue), Common::Exception);

	compareData(data, compData, ARRAYSIZE(compData));
}

GTEST_TEST(MemoryWriteStream, writeUint16BE) {
	static const uint16 compValue = 4660;
	static const byte compData[2] = { 0x12, 0x34 };

	byte data[ARRAYSIZE(compData)] = { 0 };
	Common::MemoryWriteStream stream(data);

	stream.writeUint16BE(compValue);
	EXPECT_THROW(stream.writeUint16BE(compValue), Common::Exception);

	compareData(data, compData, ARRAYSIZE(compData));
}

GTEST_TEST(MemoryWriteStream, writeSint16BE) {
	static const int16 compValue = -292;
	static const byte compData[2] = { 0xFE, 0xDC };

	byte data[ARRAYSIZE(compData)] = { 0 };
	Common::MemoryWriteStream stream(data);

	stream.writeSint16BE(compValue);
	EXPECT_THROW(stream.writeSint16BE(compValue), Common::Exception);

	compareData(data, compData, ARRAYSIZE(compData));
}

GTEST_TEST(MemoryWriteStream, writeUint32LE) {
	static const uint32 compValue = 305419896;
	static const byte compData[4] = { 0x78, 0x56, 0x34, 0x12 };

	byte data[ARRAYSIZE(compData)] = { 0 };
	Common::MemoryWriteStream stream(data);

	stream.writeUint32LE(compValue);
	EXPECT_THROW(stream.writeUint32LE(compValue), Common::Exception);

	compareData(data, compData, ARRAYSIZE(compData));
}

GTEST_TEST(MemoryWriteStream, writeSint32LE) {
	static const int32 compValue = -19088887;
	static const byte compData[4] = { 0x09, 0xBA, 0xDC, 0xFE };

	byte data[ARRAYSIZE(compData)] = { 0 };
	Common::MemoryWriteStream stream(data);

	stream.writeSint32LE(compValue);
	EXPECT_THROW(stream.writeSint32LE(compValue), Common::Exception);

	compareData(data, compData, ARRAYSIZE(compData));
}

GTEST_TEST(MemoryWriteStream, writeUint32BE) {
	static const uint32 compValue = 305419896;
	static const byte compData[4] = { 0x12, 0x34, 0x56, 0x78 };

	byte data[ARRAYSIZE(compData)] = { 0 };
	Common::MemoryWriteStream stream(data);

	stream.writeUint32BE(compValue);
	EXPECT_THROW(stream.writeUint32BE(compValue), Common::Exception);

	compareData(data, compData, ARRAYSIZE(compData));
}

GTEST_TEST(MemoryWriteStream, writeSint32BE) {
	static const int32 compValue = -19088887;
	static const byte compData[4] = { 0xFE, 0xDC, 0xBA, 0x09 };

	byte data[ARRAYSIZE(compData)] = { 0 };
	Common::MemoryWriteStream stream(data);

	stream.writeSint32BE(compValue);
	EXPECT_THROW(stream.writeSint32BE(compValue), Common::Exception);

	compareData(data, compData, ARRAYSIZE(compData));
}

GTEST_TEST(MemoryWriteStream, writeUint64LE) {
	static const uint64 compValue = UINT64_C(1311768467294899695);
	static const byte compData[8] = { 0xEF, 0xCD, 0xAB, 0x90, 0x78, 0x56, 0x34, 0x12 };

	byte data[ARRAYSIZE(compData)] = { 0 };
	Common::MemoryWriteStream stream(data);

	stream.writeUint64LE(compValue);
	EXPECT_THROW(stream.writeUint64LE(compValue), Common::Exception);

	compareData(data, compData, ARRAYSIZE(compData));
}

GTEST_TEST(MemoryWriteStream, writeSint64LE) {
	static const int64 compValue = INT64_C(-81986143110479071);
	static const byte compData[8] = { 0x21, 0x43, 0x65, 0x87, 0x09, 0xBA, 0xDC, 0xFE };

	byte data[ARRAYSIZE(compData)] = { 0 };
	Common::MemoryWriteStream stream(data);

	stream.writeSint64LE(compValue);
	EXPECT_THROW(stream.writeSint64LE(compValue), Common::Exception);

	compareData(data, compData, ARRAYSIZE(compData));
}

GTEST_TEST(MemoryWriteStream, writeUint64BE) {
	static const uint64 compValue = UINT64_C(1311768467294899695);
	static const byte compData[8] = { 0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF };

	byte data[ARRAYSIZE(compData)] = { 0 };
	Common::MemoryWriteStream stream(data);

	stream.writeUint64BE(compValue);
	EXPECT_THROW(stream.writeUint64BE(compValue), Common::Exception);

	compareData(data, compData, ARRAYSIZE(compData));
}

GTEST_TEST(MemoryWriteStream, writeSint64BE) {
	static const int64 compValue = INT64_C(-81986143110479071);
	static const byte compData[8] = { 0xFE, 0xDC, 0xBA, 0x09, 0x87, 0x65, 0x43, 0x21 };

	byte data[ARRAYSIZE(compData)] = { 0 };
	Common::MemoryWriteStream stream(data);

	stream.writeSint64BE(compValue);
	EXPECT_THROW(stream.writeSint64BE(compValue), Common::Exception);

	compareData(data, compData, ARRAYSIZE(compData));
}

GTEST_TEST(MemoryWriteStream, writeIEEEFloatLE) {
	static const float compValue = 1.0f;
	static const byte compData[4] = { 0x00, 0x00, 0x80, 0x3F };

	byte data[ARRAYSIZE(compData)] = { 0 };
	Common::MemoryWriteStream stream(data);

	stream.writeIEEEFloatLE(compValue);
	EXPECT_THROW(stream.writeIEEEFloatLE(compValue), Common::Exception);

	compareData(data, compData, ARRAYSIZE(compData));
}

GTEST_TEST(MemoryWriteStream, writeIEEEFloatBE) {
	static const float compValue = 1.0f;
	static const byte compData[4] = { 0x3F, 0x80, 0x00, 0x00 };

	byte data[ARRAYSIZE(compData)] = { 0 };
	Common::MemoryWriteStream stream(data);

	stream.writeIEEEFloatBE(compValue);
	EXPECT_THROW(stream.writeIEEEFloatBE(compValue), Common::Exception);

	compareData(data, compData, ARRAYSIZE(compData));
}

GTEST_TEST(MemoryWriteStream, writeIEEEDoubleLE) {
	static const double compValue = 1.0;
	static const byte compData[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x3F };

	byte data[ARRAYSIZE(compData)] = { 0 };
	Common::MemoryWriteStream stream(data);

	stream.writeIEEEDoubleLE(compValue);
	EXPECT_THROW(stream.writeIEEEDoubleLE(compValue), Common::Exception);

	compareData(data, compData, ARRAYSIZE(compData));
}

GTEST_TEST(MemoryWriteStream, writeIEEEDoubleBE) {
	static const double compValue = 1.0;
	static const byte compData[8] = { 0x3F, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	byte data[ARRAYSIZE(compData)] = { 0 };
	Common::MemoryWriteStream stream(data);

	stream.writeIEEEDoubleBE(compValue);
	EXPECT_THROW(stream.writeIEEEDoubleBE(compValue), Common::Exception);

	compareData(data, compData, ARRAYSIZE(compData));
}

GTEST_TEST(MemoryWriteStream, writeString) {
	byte data[6] = { 0 };
	Common::MemoryWriteStream stream(data);

	stream.writeString("Foobar");
	EXPECT_THROW(stream.writeString("Foobar"), Common::Exception);

	EXPECT_EQ(data[0], 'F');
	EXPECT_EQ(data[1], 'o');
	EXPECT_EQ(data[2], 'o');
	EXPECT_EQ(data[3], 'b');
	EXPECT_EQ(data[4], 'a');
	EXPECT_EQ(data[5], 'r');
}

GTEST_TEST(MemoryWriteStreamDynamic, write) {
	static const byte data[8] = { 0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF };

	Common::MemoryWriteStreamDynamic stream(true);

	for (size_t i = 0; i < ARRAYSIZE(data); i++)
		stream.writeByte(data[i]);

	ASSERT_EQ(stream.size(), ARRAYSIZE(data));

	for (size_t i = 0; i < ARRAYSIZE(data); i++)
		EXPECT_EQ(stream.getData()[i], data[i]) << "At index " << i;
}
