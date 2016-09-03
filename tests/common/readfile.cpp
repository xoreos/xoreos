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
 *  Unit tests for our file read stream.
 */

#include <string>
#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "gtest/gtest.h"

#include "src/common/util.h"
#include "src/common/platform.h"
#include "src/common/readfile.h"

boost::filesystem::path kFilePath;

class ReadFile : public ::testing::Test {
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
};

GTEST_TEST_F(ReadFile, write) {
	ASSERT_FALSE(kFilePath.empty());

	static const byte data[5] = { 0x12, 0x34, 0x56, 0x78, 0x90 };

	// Create the input file

	boost::filesystem::ofstream testFile(kFilePath, std::ofstream::binary);

	testFile.write(reinterpret_cast<const char *>(data), ARRAYSIZE(data));
	testFile.flush();
	ASSERT_FALSE(testFile.fail());

	testFile.close();

	// Read the file with our ReadFile class

	Common::ReadFile file(kFilePath.generic_string());
	ASSERT_TRUE(file.isOpen());

	EXPECT_EQ(file.size(), ARRAYSIZE(data));

	byte readData[ARRAYSIZE(data)];
	const size_t readCount = file.read(readData, sizeof(readData));
	EXPECT_EQ(readCount, ARRAYSIZE(readData));

	file.close();
	ASSERT_FALSE(file.isOpen());

	// Compare

	for (size_t i = 0; i < ARRAYSIZE(data); i++)
		EXPECT_EQ(readData[i], data[i]) << "At index " << i;
}
