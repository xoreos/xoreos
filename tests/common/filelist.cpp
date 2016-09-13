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
 *  Unit tests for the FileList class.
 */

#include <string>
#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "gtest/gtest.h"

#include "src/common/util.h"
#include "src/common/platform.h"
#include "src/common/filelist.h"

boost::filesystem::path kDirectoryPath;
boost::filesystem::path kFilename, kFilePath;
boost::filesystem::path kFilenameFake, kFilePathFake;

Common::UString kFilenameGlob, kFilenameFakeGlob;

class FileList: public ::testing::Test {
protected:
	static void SetUpTestCase() {
		Common::Platform::init();

		boost::filesystem::path tmpPath     = boost::filesystem::temp_directory_path();
		boost::filesystem::path uniquePath1 = boost::filesystem::unique_path("%%%%_%%%%_%%%%_%%%%.xoreos");
		boost::filesystem::path uniquePath2 = boost::filesystem::unique_path("%%%%_%%%%_%%%%_%%%%.xoreos");
		boost::filesystem::path uniquePath3 = boost::filesystem::unique_path("%%%%_%%%%_%%%%_%%%%.xoreos");

		kDirectoryPath = tmpPath / uniquePath1;

		kFilename      = uniquePath2;
		kFilePath      = kDirectoryPath / kFilename;

		kFilenameFake  = uniquePath3;
		kFilePathFake  = kDirectoryPath / kFilenameFake;

		kFilenameGlob     = ".*/" + kFilename.generic_string();
		kFilenameFakeGlob = ".*/" + kFilenameFake.generic_string();

		boost::filesystem::create_directories(kDirectoryPath);

		boost::filesystem::ofstream testFile(kFilePath, std::ofstream::binary);
		ASSERT_FALSE(testFile.fail());

		testFile.close();
	}

	static void TearDownTestCase() {
		if (!kDirectoryPath.empty())
			boost::filesystem::remove_all(kDirectoryPath);
	}
};


static const char *findPathFile(const char *path) {
	const char *f = strrchr(path, '/');

	return f ? (f + 1) : path;
}

GTEST_TEST_F(FileList, addDirectory) {
	Common::FileList list;

	EXPECT_TRUE(list.empty());
	EXPECT_EQ(list.size(), 0);

	list.addDirectory(kDirectoryPath.generic_string());

	EXPECT_FALSE(list.empty());
	EXPECT_GE(list.size(), 1U);
}

GTEST_TEST_F(FileList, copy) {
	const Common::FileList list1(kDirectoryPath.generic_string());
	const Common::FileList list2(list1);

	EXPECT_EQ(list1.size(), list2.size());

	Common::FileList::const_iterator i1 = list1.begin();
	Common::FileList::const_iterator i2 = list2.begin();
	for ( ; i1 != list1.end() && i2 != list2.end(); ++i1, ++i2)
		EXPECT_STREQ(i1->c_str(), i2->c_str());

	EXPECT_EQ(i1, list1.end());
	EXPECT_EQ(i2, list2.end());
}

GTEST_TEST_F(FileList, clear) {
	Common::FileList list(kDirectoryPath.generic_string());

	EXPECT_FALSE(list.empty());
	EXPECT_GE(list.size(), 1U);

	list.clear();

	EXPECT_TRUE(list.empty());
	EXPECT_EQ(list.size(), 0);
}

GTEST_TEST_F(FileList, contains) {
	const Common::FileList list(kDirectoryPath.generic_string());

	EXPECT_TRUE (list.contains(kFilename.generic_string()    , true));
	EXPECT_FALSE(list.contains(kFilenameFake.generic_string(), true));
}

GTEST_TEST_F(FileList, containsGlob) {
	const Common::FileList list(kDirectoryPath.generic_string());

	EXPECT_TRUE (list.containsGlob(kFilenameGlob    , true));
	EXPECT_FALSE(list.containsGlob(kFilenameFakeGlob, true));
}

GTEST_TEST_F(FileList, findFirst) {
	const Common::FileList list(kDirectoryPath.generic_string());
	EXPECT_STREQ(list.findFirst(kFilenameFake.generic_string(), true).c_str(), "");

	EXPECT_STREQ(findPathFile(list.findFirst(kFilename.generic_string(), true).c_str()), kFilename.generic_string().c_str());
}

GTEST_TEST_F(FileList, findFirstGlob) {
	const Common::FileList list(kDirectoryPath.generic_string());
	EXPECT_STREQ(list.findFirstGlob(kFilenameFakeGlob, true).c_str(), "");

	EXPECT_STREQ(findPathFile(list.findFirstGlob(kFilenameGlob, true).c_str()), kFilename.generic_string().c_str());
}

GTEST_TEST_F(FileList, getSubList) {
	const Common::FileList list(kDirectoryPath.generic_string());

	Common::FileList subList1;
	EXPECT_TRUE(list.getSubList(kFilename.generic_string(), true, subList1));

	EXPECT_FALSE(subList1.empty());
	EXPECT_GE(subList1.size(), 1U);

	EXPECT_TRUE(subList1.contains(kFilename.generic_string(), true));

	Common::FileList subList2;
	EXPECT_FALSE(list.getSubList(kFilenameFake.generic_string(), true, subList2));

	EXPECT_TRUE(subList2.empty());
	EXPECT_EQ(subList2.size(), 0);
}

GTEST_TEST_F(FileList, getSubListGlob) {
	const Common::FileList list(kDirectoryPath.generic_string());

	Common::FileList subList1;
	EXPECT_TRUE(list.getSubListGlob(kFilenameGlob, true, subList1));

	EXPECT_FALSE(subList1.empty());
	EXPECT_GE(subList1.size(), 1U);

	EXPECT_TRUE(subList1.contains(kFilename.generic_string(), true));

	Common::FileList subList2;
	EXPECT_FALSE(list.getSubListGlob(kFilenameFakeGlob, true, subList2));

	EXPECT_TRUE(subList2.empty());
	EXPECT_EQ(subList2.size(), 0);
}
