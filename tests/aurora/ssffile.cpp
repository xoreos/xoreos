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
 *  Unit tests for our SSFFile class.
 */

#include "gtest/gtest.h"

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/memreadstream.h"
#include "src/common/memwritestream.h"

#include "src/aurora/ssffile.h"

template<size_t N>
static void compareStream(Common::MemoryWriteStreamDynamic &stream, const byte (&data)[N]) {
	ASSERT_EQ(stream.size(), N);
	for (size_t i = 0; i < N; i++)
		EXPECT_EQ(stream.getData()[i], data[i]) << "At index " << i;
}

// --- V1.0 NWN ---

static const byte kSSFNWNV10[] = {
	0x53, 0x53, 0x46, 0x20, 0x56, 0x31, 0x2E, 0x30, 0x05, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x50, 0x00, 0x00, 0x00,
	0x64, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x8C, 0x00, 0x00, 0x00, 0x73, 0x6F, 0x75, 0x6E,
	0x64, 0x66, 0x69, 0x6C, 0x65, 0x30, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2A, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x2B, 0x00, 0x00, 0x00, 0x73, 0x6F, 0x75, 0x6E, 0x64, 0x66, 0x69, 0x6C, 0x65, 0x30, 0x33, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x73, 0x6F, 0x75, 0x6E,
	0x64, 0x66, 0x69, 0x6C, 0x65, 0x30, 0x35, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2E, 0x00, 0x00, 0x00
};

static const char * const kSSFNWNV10_Files[] = { "soundfile01", "", "soundfile03", "", "soundfile05" };
static const uint32_t kSSFNWNV10_StrRefs[] = { 0x0000002A, 0x0000002B, 0xFFFFFFFF, 0xFFFFFFFF, 0x0000002E };

GTEST_TEST(SSFFile, getSoundCountNWNV10) {
	Common::MemoryReadStream stream(kSSFNWNV10);
	const Aurora::SSFFile ssf(stream);

	EXPECT_EQ(ssf.getSoundCount(), 5);
}

GTEST_TEST(SSFFile, getSoundFileNWNV10) {
	Common::MemoryReadStream stream(kSSFNWNV10);
	const Aurora::SSFFile ssf(stream);

	ASSERT_EQ(ssf.getSoundCount(), ARRAYSIZE(kSSFNWNV10_Files));

	for (size_t i = 0; i < ARRAYSIZE(kSSFNWNV10_Files); i++)
		EXPECT_STREQ(ssf.getSoundFile(i).c_str(), kSSFNWNV10_Files[i]) << "At index " << i;
}

GTEST_TEST(SSFFile, getStrRefNWNV10) {
	Common::MemoryReadStream stream(kSSFNWNV10);
	const Aurora::SSFFile ssf(stream);

	ASSERT_EQ(ssf.getSoundCount(), ARRAYSIZE(kSSFNWNV10_StrRefs));

	for (size_t i = 0; i < ARRAYSIZE(kSSFNWNV10_StrRefs); i++)
		EXPECT_EQ(ssf.getStrRef(i), kSSFNWNV10_StrRefs[i]) << "At index " << i;
}

GTEST_TEST(SSFFile, getSoundNWNV10) {
	Common::MemoryReadStream stream(kSSFNWNV10);
	const Aurora::SSFFile ssf(stream);

	ASSERT_EQ(ssf.getSoundCount(), ARRAYSIZE(kSSFNWNV10_Files));
	ASSERT_EQ(ssf.getSoundCount(), ARRAYSIZE(kSSFNWNV10_StrRefs));

	for (size_t i = 0; i < ARRAYSIZE(kSSFNWNV10_Files); i++) {
		Common::UString soundFile;
		uint32_t strRef;

		ssf.getSound(i, soundFile, strRef);

		EXPECT_STREQ(soundFile.c_str(), kSSFNWNV10_Files[i]) << "At index " << i;
		EXPECT_EQ(strRef, kSSFNWNV10_StrRefs[i]) << "At index " << i;
	}
}

GTEST_TEST(SSFFile, determineVersionForGameNWNV10) {
	Aurora::SSFFile ssf;

	for (size_t i = 0; i < ARRAYSIZE(kSSFNWNV10_Files); i++)
		ssf.setSound(i, kSSFNWNV10_Files[i], kSSFNWNV10_StrRefs[i]);

	EXPECT_EQ(ssf.determineVersionForGame(Aurora::kGameIDNWN ), Aurora::SSFFile::kVersion10_NWN);
	EXPECT_EQ(ssf.determineVersionForGame(Aurora::kGameIDNWN2), Aurora::SSFFile::kVersion10_NWN);
}

GTEST_TEST(SSFFile, writeSSFsetSoundNWNV10) {
	Aurora::SSFFile ssf;

	for (size_t i = 0; i < ARRAYSIZE(kSSFNWNV10_Files); i++)
		ssf.setSound(i, kSSFNWNV10_Files[i], kSSFNWNV10_StrRefs[i]);

	Common::MemoryWriteStreamDynamic writeStream(true);
	ssf.writeSSF(writeStream, Aurora::SSFFile::kVersion10_NWN);

	compareStream(writeStream, kSSFNWNV10);
}

GTEST_TEST(SSFFile, writeSSFsetFileStrRefNWNV10) {
	Aurora::SSFFile ssf;

	for (size_t i = 0; i < ARRAYSIZE(kSSFNWNV10_Files); i++) {
		ssf.setSoundFile(i, kSSFNWNV10_Files[i]);
		ssf.setStrRef(i, kSSFNWNV10_StrRefs[i]);
	}

	Common::MemoryWriteStreamDynamic writeStream(true);
	ssf.writeSSF(writeStream, Aurora::SSFFile::kVersion10_NWN);

	compareStream(writeStream, kSSFNWNV10);
}

// --- V1.1 NWN2 ---

static const byte kSSFNWN2V11[] = {
	0x53, 0x53, 0x46, 0x20, 0x56, 0x31, 0x2E, 0x31, 0x05, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00,
	0x84, 0x00, 0x00, 0x00, 0xA8, 0x00, 0x00, 0x00, 0xCC, 0x00, 0x00, 0x00, 0x74, 0x68, 0x69, 0x73,
	0x69, 0x73, 0x61, 0x76, 0x65, 0x72, 0x79, 0x6C, 0x6F, 0x6E, 0x67, 0x73, 0x6F, 0x75, 0x6E, 0x64,
	0x66, 0x69, 0x6C, 0x65, 0x30, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2A, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x2B, 0x00, 0x00, 0x00, 0x74, 0x68, 0x69, 0x73, 0x69, 0x73, 0x61, 0x76, 0x65, 0x72, 0x79, 0x6C,
	0x6F, 0x6E, 0x67, 0x73, 0x6F, 0x75, 0x6E, 0x64, 0x66, 0x69, 0x6C, 0x65, 0x30, 0x33, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x74, 0x68, 0x69, 0x73,
	0x69, 0x73, 0x61, 0x76, 0x65, 0x72, 0x79, 0x6C, 0x6F, 0x6E, 0x67, 0x73, 0x6F, 0x75, 0x6E, 0x64,
	0x66, 0x69, 0x6C, 0x65, 0x30, 0x35, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2E, 0x00, 0x00, 0x00
};

static const char * const kSSFNWN2V11_Files[] = { "thisisaverylongsoundfile01", "", "thisisaverylongsoundfile03", "", "thisisaverylongsoundfile05" };
static const uint32_t kSSFNWN2V11_StrRefs[] = { 0x0000002A, 0x0000002B, 0xFFFFFFFF, 0xFFFFFFFF, 0x0000002E };

GTEST_TEST(SSFFile, getSoundCountNWN2V11) {
	Common::MemoryReadStream stream(kSSFNWN2V11);
	const Aurora::SSFFile ssf(stream);

	EXPECT_EQ(ssf.getSoundCount(), 5);
}

GTEST_TEST(SSFFile, getSoundFileNWN2V11) {
	Common::MemoryReadStream stream(kSSFNWN2V11);
	const Aurora::SSFFile ssf(stream);

	ASSERT_EQ(ssf.getSoundCount(), ARRAYSIZE(kSSFNWN2V11_Files));

	for (size_t i = 0; i < ARRAYSIZE(kSSFNWN2V11_Files); i++)
		EXPECT_STREQ(ssf.getSoundFile(i).c_str(), kSSFNWN2V11_Files[i]) << "At index " << i;
}

GTEST_TEST(SSFFile, getStrRefNWN2V11) {
	Common::MemoryReadStream stream(kSSFNWN2V11);
	const Aurora::SSFFile ssf(stream);

	ASSERT_EQ(ssf.getSoundCount(), ARRAYSIZE(kSSFNWN2V11_StrRefs));

	for (size_t i = 0; i < ARRAYSIZE(kSSFNWN2V11_StrRefs); i++)
		EXPECT_EQ(ssf.getStrRef(i), kSSFNWN2V11_StrRefs[i]) << "At index " << i;
}

GTEST_TEST(SSFFile, getSoundNWN2V11) {
	Common::MemoryReadStream stream(kSSFNWN2V11);
	const Aurora::SSFFile ssf(stream);

	ASSERT_EQ(ssf.getSoundCount(), ARRAYSIZE(kSSFNWN2V11_Files));
	ASSERT_EQ(ssf.getSoundCount(), ARRAYSIZE(kSSFNWN2V11_StrRefs));

	for (size_t i = 0; i < ARRAYSIZE(kSSFNWN2V11_Files); i++) {
		Common::UString soundFile;
		uint32_t strRef;

		ssf.getSound(i, soundFile, strRef);

		EXPECT_STREQ(soundFile.c_str(), kSSFNWN2V11_Files[i]) << "At index " << i;
		EXPECT_EQ(strRef, kSSFNWN2V11_StrRefs[i]) << "At index " << i;
	}
}

GTEST_TEST(SSFFile, determineVersionForGameNWN2V11) {
	Aurora::SSFFile ssf;

	for (size_t i = 0; i < ARRAYSIZE(kSSFNWN2V11_Files); i++)
		ssf.setSound(i, kSSFNWN2V11_Files[i], kSSFNWN2V11_StrRefs[i]);

	EXPECT_EQ(ssf.determineVersionForGame(Aurora::kGameIDNWN2), Aurora::SSFFile::kVersion11_NWN2);
}

GTEST_TEST(SSFFile, writeSSFsetSoundNWN2V11) {
	Aurora::SSFFile ssf;

	for (size_t i = 0; i < ARRAYSIZE(kSSFNWN2V11_Files); i++)
		ssf.setSound(i, kSSFNWN2V11_Files[i], kSSFNWN2V11_StrRefs[i]);

	Common::MemoryWriteStreamDynamic writeStream(true);
	ssf.writeSSF(writeStream, Aurora::SSFFile::kVersion11_NWN2);

	compareStream(writeStream, kSSFNWN2V11);
}

GTEST_TEST(SSFFile, writeSSFsetFileStrRefNWN2V11) {
	Aurora::SSFFile ssf;

	for (size_t i = 0; i < ARRAYSIZE(kSSFNWN2V11_Files); i++) {
		ssf.setSoundFile(i, kSSFNWN2V11_Files[i]);
		ssf.setStrRef(i, kSSFNWN2V11_StrRefs[i]);
	}

	Common::MemoryWriteStreamDynamic writeStream(true);
	ssf.writeSSF(writeStream, Aurora::SSFFile::kVersion11_NWN2);

	compareStream(writeStream, kSSFNWN2V11);
}

// --- V1.1 KotOR/KotOR2 ---

static const byte kSSFKotORV11[] = {
	0x53, 0x53, 0x46, 0x20, 0x56, 0x31, 0x2E, 0x31, 0x0C, 0x00, 0x00, 0x00, 0x2A, 0x00, 0x00, 0x00,
	0x2B, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x2D, 0x00, 0x00, 0x00, 0x2E, 0x00, 0x00, 0x00
};

static const char * const kSSFKotORV11_Files[] = { "", "", "", "", "" };
static const uint32_t kSSFKotORV11_StrRefs[] = { 0x0000002A, 0x0000002B, 0xFFFFFFFF, 0x0000002D, 0x0000002E };

GTEST_TEST(SSFFile, getSoundCountKotORV11) {
	Common::MemoryReadStream stream(kSSFKotORV11);
	const Aurora::SSFFile ssf(stream);

	EXPECT_EQ(ssf.getSoundCount(), 5);
}

GTEST_TEST(SSFFile, getSoundFileKotORV11) {
	Common::MemoryReadStream stream(kSSFKotORV11);
	const Aurora::SSFFile ssf(stream);

	ASSERT_EQ(ssf.getSoundCount(), ARRAYSIZE(kSSFKotORV11_Files));

	for (size_t i = 0; i < ARRAYSIZE(kSSFKotORV11_Files); i++)
		EXPECT_STREQ(ssf.getSoundFile(i).c_str(), kSSFKotORV11_Files[i]) << "At index " << i;
}

GTEST_TEST(SSFFile, getStrRefKotORV11) {
	Common::MemoryReadStream stream(kSSFKotORV11);
	const Aurora::SSFFile ssf(stream);

	ASSERT_EQ(ssf.getSoundCount(), ARRAYSIZE(kSSFKotORV11_StrRefs));

	for (size_t i = 0; i < ARRAYSIZE(kSSFKotORV11_StrRefs); i++)
		EXPECT_EQ(ssf.getStrRef(i), kSSFKotORV11_StrRefs[i]) << "At index " << i;
}

GTEST_TEST(SSFFile, getSoundKotORV11) {
	Common::MemoryReadStream stream(kSSFKotORV11);
	const Aurora::SSFFile ssf(stream);

	ASSERT_EQ(ssf.getSoundCount(), ARRAYSIZE(kSSFKotORV11_Files));
	ASSERT_EQ(ssf.getSoundCount(), ARRAYSIZE(kSSFKotORV11_StrRefs));

	for (size_t i = 0; i < ARRAYSIZE(kSSFKotORV11_Files); i++) {
		Common::UString soundFile;
		uint32_t strRef;

		ssf.getSound(i, soundFile, strRef);

		EXPECT_STREQ(soundFile.c_str(), kSSFKotORV11_Files[i]) << "At index " << i;
		EXPECT_EQ(strRef, kSSFKotORV11_StrRefs[i]) << "At index " << i;
	}
}

GTEST_TEST(SSFFile, determineVersionForGameKotORV11) {
	Aurora::SSFFile ssf;

	for (size_t i = 0; i < ARRAYSIZE(kSSFKotORV11_Files); i++)
		ssf.setSound(i, kSSFKotORV11_Files[i], kSSFKotORV11_StrRefs[i]);

	EXPECT_EQ(ssf.determineVersionForGame(Aurora::kGameIDKotOR ), Aurora::SSFFile::kVersion11_KotOR);
	EXPECT_EQ(ssf.determineVersionForGame(Aurora::kGameIDKotOR2), Aurora::SSFFile::kVersion11_KotOR);
}

GTEST_TEST(SSFFile, writeSSFsetSoundKotORV11) {
	Aurora::SSFFile ssf;

	for (size_t i = 0; i < ARRAYSIZE(kSSFKotORV11_Files); i++)
		ssf.setSound(i, kSSFKotORV11_Files[i], kSSFKotORV11_StrRefs[i]);

	Common::MemoryWriteStreamDynamic writeStream(true);
	ssf.writeSSF(writeStream, Aurora::SSFFile::kVersion11_KotOR);

	compareStream(writeStream, kSSFKotORV11);
}

GTEST_TEST(SSFFile, writeSSFsetFileStrRefKotORV11) {
	Aurora::SSFFile ssf;

	for (size_t i = 0; i < ARRAYSIZE(kSSFKotORV11_Files); i++) {
		ssf.setSoundFile(i, kSSFKotORV11_Files[i]);
		ssf.setStrRef(i, kSSFKotORV11_StrRefs[i]);
	}

	Common::MemoryWriteStreamDynamic writeStream(true);
	ssf.writeSSF(writeStream, Aurora::SSFFile::kVersion11_KotOR);

	compareStream(writeStream, kSSFKotORV11);
}
