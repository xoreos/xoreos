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
 *  Encoding tests, included by the various encoding_* files.
 */

#ifndef TESTS_COMMON_ENCODING_TESTS_H
#define TESTS_COMMON_ENCODING_TESTS_H

GTEST_TEST(XOREOS_ENCODINGNAME, readString) {
	testSupport(kEncoding);

	Common::MemoryReadStream stream(stringData0X);

	const Common::UString string = Common::readString(stream, kEncoding);

	EXPECT_EQ(string.size(), stringChars);
	EXPECT_STREQ(string.c_str(), stringUString.c_str());
}

GTEST_TEST(XOREOS_ENCODINGNAME, readStringFixed) {
	testSupport(kEncoding);

	Common::MemoryReadStream stream(stringDataX);

	const Common::UString string = Common::readStringFixed(stream, kEncoding, stringBytes);

	EXPECT_EQ(string.size(), stringChars);
	EXPECT_STREQ(string.c_str(), stringUString.c_str());
}

GTEST_TEST(XOREOS_ENCODINGNAME, readStringLine) {
	testSupport(kEncoding);

	Common::MemoryReadStream stream(stringDataLineX);

	const Common::UString string = Common::readStringLine(stream, kEncoding);

	EXPECT_EQ(string.size(), stringChars);
	EXPECT_STREQ(string.c_str(), stringUString.c_str());
}

GTEST_TEST(XOREOS_ENCODINGNAME, readStringBuf) {
	testSupport(kEncoding);

	const Common::UString string = Common::readString(stringDataX, stringBytes, kEncoding);

	EXPECT_EQ(string.size(), stringChars);
	EXPECT_STREQ(string.c_str(), stringUString.c_str());
}

static void compareData(Common::SeekableReadStream &stream, const byte *data, size_t n, size_t t) {
	for (size_t i = 0; i < n; i++)
		EXPECT_EQ(stream.readByte(), data[i]) << "At case " << t << ", index " << i;
}

static void compareData(const byte *data1, const byte *data2, size_t n, size_t t, size_t offset1 = 0) {
	for (size_t i = 0; i < n; i++)
		EXPECT_EQ(data1[offset1 + i], data2[i]) << "At case " << t << ", index " << i;
}

GTEST_TEST(XOREOS_ENCODINGNAME, convertString) {
	testSupport(kEncoding);

	Common::MemoryReadStream *stream = 0;

	stream = convertString(stringUString, kEncoding, false);
	ASSERT_NE(stream, static_cast<Common::MemoryReadStream *>(0));

	EXPECT_EQ(stream->size(), stringBytes);
	compareData(*stream, stringData0, stringBytes, 0);

	delete stream;

	stream = convertString(stringUString, kEncoding, true);
	ASSERT_NE(stream, static_cast<Common::MemoryReadStream *>(0));

	EXPECT_EQ(stream->size(), sizeof(stringData0));
	compareData(*stream, stringData0, sizeof(stringData0), 1);

	delete stream;
}

GTEST_TEST(XOREOS_ENCODINGNAME, writeString) {
	testSupport(kEncoding);

	byte writeData[sizeof(stringData0) * 2] = { 0 };
	Common::MemoryWriteStream stream(writeData);

	const size_t written1 = Common::writeString(stream, stringUString, kEncoding, false);
	ASSERT_EQ(written1, stringBytes);

	compareData(writeData, stringData0, stringBytes, 0);

	const size_t written2 = Common::writeString(stream, stringUString, kEncoding, true);
	ASSERT_EQ(written2, sizeof(stringData0));

	compareData(writeData, stringData0, sizeof(stringData0), 1, written1);
}

GTEST_TEST(XOREOS_ENCODINGNAME, writeStringFixed) {
	testSupport(kEncoding);

	byte writeData[sizeof(stringData0) * 2] = { 0 };
	Common::MemoryWriteStream stream(writeData);

	Common::writeStringFixed(stream, stringUString, kEncoding, stringBytes);

	compareData(writeData, stringData0, stringBytes, 0);

	Common::writeStringFixed(stream, stringUString, kEncoding, sizeof(stringData0));

	compareData(writeData, stringData0, sizeof(stringData0), 1, stringBytes);
}

#endif // TESTS_COMMON_ENCODING_TESTS_H
