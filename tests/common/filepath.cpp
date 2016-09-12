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
 *  Unit tests for the FilePath utility class.
 */

/* NOTE: The following methods can't be tested (at the moment?) because
 * their behaviour is operating-system-dependant:
 * - Common::FilePath::isAbsolute()
 * - Common::FilePath::absolutize()
 * - Common::FilePath::normalize()
 * - Common::FilePath::canonicalize()
 *
 * The following methods can't be tested because their behaviour changes
 * depending on the file and directory structure:
 * - Common::FilePath::findSubDirectory()
 * - Common::FilePath::getSubDirectories()
 * - Common::FilePath::createDirectories()
 *
 * The following methods can't be tested because their behaviour changes
 * both with the operating system and user configuration:
 * - Common::FilePath::getHomeDirectory()
 * - Common::FilePath::getConfigDirectory()
 * - Common::FilePath::getUserDataDirectory()
 * - Common::FilePath::getUserDataFile()
 */

#include <string>
#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "gtest/gtest.h"

#include "src/common/util.h"
#include "src/common/platform.h"
#include "src/common/filepath.h"

boost::filesystem::path kDirectoryPath;
boost::filesystem::path kFilename, kFilePath;
boost::filesystem::path kFilenameFake, kFilePathFake;

class FilePath : public ::testing::Test {
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

		boost::filesystem::create_directories(kDirectoryPath);

		const char buf[23] = { 0 };

		boost::filesystem::ofstream testFile(kFilePath, std::ofstream::binary);

		testFile.write(buf, ARRAYSIZE(buf));
		testFile.flush();
		ASSERT_FALSE(testFile.fail());

		testFile.close();
	}

	static void TearDownTestCase() {
		if (!kDirectoryPath.empty())
			boost::filesystem::remove_all(kDirectoryPath);
	}
};

GTEST_TEST_F(FilePath, isRegularFile) {
	EXPECT_TRUE (Common::FilePath::isRegularFile(kFilePath.generic_string()));
	EXPECT_FALSE(Common::FilePath::isRegularFile(kFilePathFake.generic_string()));
	EXPECT_FALSE(Common::FilePath::isRegularFile(kDirectoryPath.generic_string()));
}

GTEST_TEST_F(FilePath, isDirectory) {
	EXPECT_FALSE(Common::FilePath::isDirectory(kFilePath.generic_string()));
	EXPECT_FALSE(Common::FilePath::isDirectory(kFilePathFake.generic_string()));
	EXPECT_TRUE (Common::FilePath::isDirectory(kDirectoryPath.generic_string()));
}

GTEST_TEST_F(FilePath, getFileSize) {
	EXPECT_EQ(Common::FilePath::getFileSize(kFilePath.generic_string()), 23);
	EXPECT_EQ(Common::FilePath::getFileSize(kFilePathFake.generic_string()), Common::kFileInvalid);
	EXPECT_EQ(Common::FilePath::getFileSize(kDirectoryPath.generic_string()), Common::kFileInvalid);
}

GTEST_TEST_F(FilePath, getFile) {
	EXPECT_STREQ(Common::FilePath::getFile("/path/to/file.ext").c_str(), "file.ext");
	EXPECT_STREQ(Common::FilePath::getFile("path/to/file.ext" ).c_str(), "file.ext");
	EXPECT_STREQ(Common::FilePath::getFile("file.ext"         ).c_str(), "file.ext");
	EXPECT_STREQ(Common::FilePath::getFile("/path/to/"        ).c_str(), ".");
}

GTEST_TEST_F(FilePath, getStem) {
	EXPECT_STREQ(Common::FilePath::getStem("/path/to/file.ext").c_str(), "file");
	EXPECT_STREQ(Common::FilePath::getStem("path/to/file.ext" ).c_str(), "file");
	EXPECT_STREQ(Common::FilePath::getStem("file.ext"         ).c_str(), "file");
	EXPECT_STREQ(Common::FilePath::getStem("file.ext.ext"     ).c_str(), "file.ext");
	EXPECT_STREQ(Common::FilePath::getStem("file."            ).c_str(), "file");
	EXPECT_STREQ(Common::FilePath::getStem("file"             ).c_str(), "file");
	EXPECT_STREQ(Common::FilePath::getStem("/path/to/"        ).c_str(), ".");
}

GTEST_TEST_F(FilePath, getExtension) {
	EXPECT_STREQ(Common::FilePath::getExtension("/path/to/file.ext").c_str(), ".ext");
	EXPECT_STREQ(Common::FilePath::getExtension("path/to/file.ext" ).c_str(), ".ext");
	EXPECT_STREQ(Common::FilePath::getExtension("file.ext"         ).c_str(), ".ext");
	EXPECT_STREQ(Common::FilePath::getExtension("file.ext.ext"     ).c_str(), ".ext");
	EXPECT_STREQ(Common::FilePath::getExtension("file."            ).c_str(), ".");
	EXPECT_STREQ(Common::FilePath::getExtension("file"             ).c_str(), "");
	EXPECT_STREQ(Common::FilePath::getExtension("/path/to/"        ).c_str(), "");
}

GTEST_TEST_F(FilePath, getDirectory) {
	EXPECT_STREQ(Common::FilePath::getDirectory("/path/to/file.ext").c_str(), "/path/to");
	EXPECT_STREQ(Common::FilePath::getDirectory("path/to/file.ext" ).c_str(), "path/to");
	EXPECT_STREQ(Common::FilePath::getDirectory("file.ext"         ).c_str(), "");
	EXPECT_STREQ(Common::FilePath::getDirectory("/path/to/"        ).c_str(), "/path/to");
}

GTEST_TEST_F(FilePath, changeExtension) {
	EXPECT_STREQ(Common::FilePath::changeExtension("/path/file.ext", ".new"  ).c_str(), "/path/file.new");
	EXPECT_STREQ(Common::FilePath::changeExtension("/path/file.ext", ".new.m").c_str(), "/path/file.new.m");
	EXPECT_STREQ(Common::FilePath::changeExtension("/path/file.ext", "new"   ).c_str(), "/path/file.new");
	EXPECT_STREQ(Common::FilePath::changeExtension("/path/file.ext", ""      ).c_str(), "/path/file");
	EXPECT_STREQ(Common::FilePath::changeExtension("file.ext.ext"  , ".new"  ).c_str(), "file.ext.new");
	EXPECT_STREQ(Common::FilePath::changeExtension("file.ext.ext"  , ".new.m").c_str(), "file.ext.new.m");
	EXPECT_STREQ(Common::FilePath::changeExtension("file.ext.ext"  , "new"   ).c_str(), "file.ext.new");
	EXPECT_STREQ(Common::FilePath::changeExtension("file.ext.ext"  , ""      ).c_str(), "file.ext");
	EXPECT_STREQ(Common::FilePath::changeExtension("file."         , ".new"  ).c_str(), "file.new");
	EXPECT_STREQ(Common::FilePath::changeExtension("file."         , ".new.m").c_str(), "file.new.m");
	EXPECT_STREQ(Common::FilePath::changeExtension("file."         , "new"   ).c_str(), "file.new");
	EXPECT_STREQ(Common::FilePath::changeExtension("file."         , ""      ).c_str(), "file");
	EXPECT_STREQ(Common::FilePath::changeExtension("file"          , ".new"  ).c_str(), "file.new");
	EXPECT_STREQ(Common::FilePath::changeExtension("file"          , ".new.m").c_str(), "file.new.m");
	EXPECT_STREQ(Common::FilePath::changeExtension("file"          , "new"   ).c_str(), "file.new");
	EXPECT_STREQ(Common::FilePath::changeExtension("file"          , ""      ).c_str(), "file");
	EXPECT_STREQ(Common::FilePath::changeExtension("/path/"        , ".new"  ).c_str(), "/path/.new");
	EXPECT_STREQ(Common::FilePath::changeExtension("/path/"        , ".new.m").c_str(), "/path/.new.m");
	EXPECT_STREQ(Common::FilePath::changeExtension("/path/"        , "new"   ).c_str(), "/path/.new");
	EXPECT_STREQ(Common::FilePath::changeExtension("/path/"        , ""      ).c_str(), "/path/");
}

GTEST_TEST_F(FilePath, relativize) {
	EXPECT_STREQ(Common::FilePath::relativize("/path/to", "/path/to/file.ext").c_str(), "file.ext");
	EXPECT_STREQ(Common::FilePath::relativize("/other"  , "/path/to/file.ext").c_str(), "");

	// Testing paths that have not been normalized

	EXPECT_STREQ(Common::FilePath::relativize("/path/to/", "/path/to/file.ext" ).c_str(), "file.ext");
	EXPECT_STREQ(Common::FilePath::relativize("/path/to" , "/path/to//file.ext").c_str(), "file.ext");
	EXPECT_STREQ(Common::FilePath::relativize("/path//to", "/path/to/file.ext" ).c_str(), "file.ext");

	// Paths that can't be normalized by relativize, because of potential symlinks

	EXPECT_STREQ(Common::FilePath::relativize("/path/to/foo/.../", "/path/to/file.ext" ).c_str(), "");
	EXPECT_STREQ(Common::FilePath::relativize("/path/to/foo/..." , "/path/to//file.ext").c_str(), "");
	EXPECT_STREQ(Common::FilePath::relativize("/path//to/foo/...", "/path/to/file.ext" ).c_str(), "");
}

GTEST_TEST_F(FilePath, escapeStringLiteral) {
	EXPECT_STREQ(Common::FilePath::escapeStringLiteral("/file.ext").c_str(), "\\/file\\.ext");
}
