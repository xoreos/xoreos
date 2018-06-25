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
 *  Unit tests for our file write stream.
 */

#include <string>
#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "gtest/gtest.h"

#include "src/common/util.h"
#include "src/common/platform.h"
#include "src/common/writefile.h"

boost::filesystem::path kFilePath;

class WriteFile : public ::testing::Test {
protected:
	static void SetUpTestCase() {
		Common::Platform::init();

		boost::filesystem::path tmpPath    = boost::filesystem::temp_directory_path();
		boost::filesystem::path uniquePath = boost::filesystem::unique_path("%%%%_%%%%_%%%%_%%%%.xoreos");

		kFilePath = tmpPath / uniquePath;
	}

	static void TearDownTestCase() {
		if (!kFilePath.empty())
			boost::filesystem::remove(kFilePath);
	}

	void SetUp() {
		if (!kFilePath.empty())
			boost::filesystem::remove(kFilePath);
	}
};

GTEST_TEST_F(WriteFile, write) {
	ASSERT_FALSE(kFilePath.empty());

	static const byte data[5] = { 0x12, 0x34, 0x56, 0x78, 0x90 };

	// Write the file with our WriteFile class

	Common::WriteFile file(kFilePath.generic_string());
	ASSERT_TRUE(file.isOpen());

	const size_t writeCount1 = file.write(data, sizeof(data));
	EXPECT_EQ(writeCount1, ARRAYSIZE(data));

	const size_t writeCount2 = file.write(data, sizeof(data));
	EXPECT_EQ(writeCount2, ARRAYSIZE(data));

	EXPECT_EQ(file.size(), 2 * ARRAYSIZE(data));

	file.flush();
	file.close();
	ASSERT_FALSE(file.isOpen());

	// Read back in the file and compare

	boost::filesystem::ifstream testFile(kFilePath, std::ofstream::binary);

	byte readData[ARRAYSIZE(data)] = { 0 };

	testFile.read(reinterpret_cast<char *>(readData), ARRAYSIZE(readData));
	ASSERT_FALSE(testFile.fail());

	testFile.close();

	for (size_t i = 0; i < ARRAYSIZE(data); i++)
		EXPECT_EQ(readData[i], data[i]) << "At index " << i;
}

GTEST_TEST_F(WriteFile, seek) {
	ASSERT_FALSE(kFilePath.empty());

	Common::WriteFile file(kFilePath.generic_string());
	ASSERT_TRUE(file.isOpen());

	file.writeString("Foobar");
	file.seek(3);
	file.writeString("Foo");
	file.seek(-2, Common::SeekableWriteStream::kOriginEnd);
	file.writeString("uu");
	file.seek(-6, Common::SeekableWriteStream::kOriginCurrent);
	file.writeString("bar");

	file.flush();
	file.close();

	// Read back in the file and compare

	boost::filesystem::ifstream testFile(kFilePath, std::ofstream::binary);
	byte data[6];
	testFile.read(reinterpret_cast<char *>(data), 6);
	ASSERT_FALSE(testFile.fail());

	EXPECT_EQ(data[0], 'b');
	EXPECT_EQ(data[1], 'a');
	EXPECT_EQ(data[2], 'r');
	EXPECT_EQ(data[3], 'F');
	EXPECT_EQ(data[4], 'u');
	EXPECT_EQ(data[5], 'u');
}

GTEST_TEST_F(WriteFile, size) {
	ASSERT_FALSE(kFilePath.empty());
	Common::WriteFile file(kFilePath.generic_string());

	ASSERT_TRUE(file.isOpen());

	file.writeUint64BE(0x0123456789ABCDEF);
	EXPECT_EQ(file.size(), 8);

	file.seek(-2, Common::SeekableWriteStream::kOriginCurrent);
	file.writeUint64BE(0x0123456789ABCDEF);
	EXPECT_EQ(file.size(), 14);

	file.seek(0);
	file.writeByte(0xFF);
	EXPECT_EQ(file.size(), 14);

	file.flush();
	file.close();

	// Read back in the file and compare

	boost::filesystem::ifstream testFile(kFilePath, std::ofstream::binary);
	byte data[14];
	testFile.read(reinterpret_cast<char *>(data), 14);
	ASSERT_FALSE(testFile.fail());

	EXPECT_EQ(data[0], 0xFF);
	EXPECT_EQ(data[1], 0x23);
	EXPECT_EQ(data[2], 0x45);
	EXPECT_EQ(data[3], 0x67);
	EXPECT_EQ(data[4], 0x89);
	EXPECT_EQ(data[5], 0xAB);
	EXPECT_EQ(data[6], 0x01);
	EXPECT_EQ(data[7], 0x23);
	EXPECT_EQ(data[8], 0x45);
	EXPECT_EQ(data[9], 0x67);
	EXPECT_EQ(data[10], 0x89);
	EXPECT_EQ(data[11], 0xAB);
	EXPECT_EQ(data[12], 0xCD);
	EXPECT_EQ(data[13], 0xEF);
}
