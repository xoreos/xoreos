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
 *  SerializationStream unit tests.
 */

#include "gtest/gtest.h"

#include "src/common/serializationstream.h"
#include "src/common/memwritestream.h"
#include "src/common/memreadstream.h"

GTEST_TEST(SerializationStream, readAndWriteData) {
	Common::MemoryWriteStreamDynamic writeStream(true);

	Common::SerializationWriteStream serialWriteStream(writeStream);

	uint64_t test64 = UINT64_C(0x1122334455667788);
	serialWriteStream.readOrWriteUint64LE(UINT64_C(0x1122334455667788));
	serialWriteStream.readOrWriteUint64LE(test64);
	serialWriteStream.readOrWriteUint64BE(UINT64_C(0x1122334455667788));
	serialWriteStream.readOrWriteUint64BE(test64);

	uint32_t test32 = 0x11223344;
	serialWriteStream.readOrWriteUint32LE(0x11223344);
	serialWriteStream.readOrWriteUint32LE(test32);
	serialWriteStream.readOrWriteUint32BE(0x11223344);
	serialWriteStream.readOrWriteUint32BE(test32);

	uint16_t test16 = 0x1122;
	serialWriteStream.readOrWriteUint16LE(0x1122);
	serialWriteStream.readOrWriteUint16LE(test16);
	serialWriteStream.readOrWriteUint16BE(0x1122);
	serialWriteStream.readOrWriteUint16BE(test16);

	serialWriteStream.readOrWriteByte(0xAF);
	serialWriteStream.readOrWriteByte(0xFA);

	byte test = 12;
	serialWriteStream.readOrWriteByte(test);
	serialWriteStream.readOrWriteByte(13);

	serialWriteStream.readOrWriteBytePrefixedASCIIString("Test");
	serialWriteStream.readOrWriteUint32LEPrefixedASCIIString("Test2");

	Common::UString testString = "TestString";
	serialWriteStream.readOrWriteBytePrefixedASCIIString(testString);
	serialWriteStream.readOrWriteUint32LEPrefixedASCIIString(testString);

	Common::MemoryReadStream readStream(writeStream.getData(), writeStream.size());
	Common::SerializationReadStream serialReadStream(readStream);

	EXPECT_NO_THROW(serialReadStream.readOrWriteUint64LE(UINT64_C(0x1122334455667788)));
	serialReadStream.readOrWriteUint64LE(test64);
	EXPECT_EQ(test64, UINT64_C(0x1122334455667788));
	EXPECT_NO_THROW(serialReadStream.readOrWriteUint64BE(UINT64_C(0x1122334455667788)));
	serialReadStream.readOrWriteUint64BE(test64);
	EXPECT_EQ(test64, UINT64_C(0x1122334455667788));

	EXPECT_NO_THROW(serialReadStream.readOrWriteUint32LE(0x11223344));
	serialReadStream.readOrWriteUint32LE(test32);
	EXPECT_EQ(test32, 0x11223344);
	EXPECT_NO_THROW(serialReadStream.readOrWriteUint32BE(0x11223344));
	serialReadStream.readOrWriteUint32BE(test32);
	EXPECT_EQ(test32, 0x11223344);

	EXPECT_NO_THROW(serialReadStream.readOrWriteUint16LE(0x1122));
	serialReadStream.readOrWriteUint16LE(test16);
	EXPECT_EQ(test16, 0x1122);
	EXPECT_NO_THROW(serialReadStream.readOrWriteUint16BE(0x1122));
	serialReadStream.readOrWriteUint16BE(test16);
	EXPECT_EQ(test16, 0x1122);

	EXPECT_NO_THROW(serialReadStream.readOrWriteByte(0xAF));
	EXPECT_THROW(serialReadStream.readOrWriteByte(0xAF), Common::Exception);

	EXPECT_NO_THROW(serialReadStream.readOrWriteByte(12));

	serialReadStream.readOrWriteByte(test);
	EXPECT_EQ(test, 13);

	EXPECT_NO_THROW(serialReadStream.readOrWriteBytePrefixedASCIIString("Test"));
	EXPECT_NO_THROW(serialReadStream.readOrWriteUint32LEPrefixedASCIIString("Test2"));

	serialReadStream.readOrWriteBytePrefixedASCIIString(testString);
	EXPECT_STREQ(testString.c_str(), "TestString");

	serialReadStream.readOrWriteUint32LEPrefixedASCIIString(testString);
	EXPECT_STREQ(testString.c_str(), "TestString");
}
