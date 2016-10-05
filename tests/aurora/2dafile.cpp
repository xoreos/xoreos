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
 *  Unit tests for our TwoDAFile class.
 */

#include <vector>

#include "gtest/gtest.h"

#include "src/common/ustring.h"
#include "src/common/error.h"
#include "src/common/memreadstream.h"
#include "src/common/memwritestream.h"

#include "src/aurora/types.h"
#include "src/aurora/2dafile.h"
#include "src/aurora/gdafile.h"

static const char *k2DAASCII =
  "2DA V2.0\n"
  "\n"
  "   ID   FloatValue StringValue\n"
  " 0 23   23.5       Foobar     \n"
  " 1 42   5.23       Barfoo     \n"
  " 2 **** 1.00       Quux       \n"
  " 3 5    ****       Blah       \n"
  " 4 9    0.10       ****       \n"
  " 5 **** ****       ****       \n"
  " 6 12   ****       Test1      \n"
  " 7 13   ****       Test2      \n"
  " 8 14   ****       Test3      \n"
  " 9 15   ****       Test4      \n"
  "10 16   ****       Test5      \n";

static const byte k2DABinary[] = {
	0x32,0x44,0x41,0x20,0x56,0x32,0x2E,0x62,0x0A,0x49,0x44,0x09,0x46,0x6C,0x6F,0x61,
	0x74,0x56,0x61,0x6C,0x75,0x65,0x09,0x53,0x74,0x72,0x69,0x6E,0x67,0x56,0x61,0x6C,
	0x75,0x65,0x09,0x00,0x0B,0x00,0x00,0x00,0x30,0x09,0x31,0x09,0x32,0x09,0x33,0x09,
	0x34,0x09,0x35,0x09,0x36,0x09,0x37,0x09,0x38,0x09,0x39,0x09,0x31,0x30,0x09,0x00,
	0x00,0x03,0x00,0x08,0x00,0x0F,0x00,0x12,0x00,0x17,0x00,0x1E,0x00,0x1F,0x00,0x24,
	0x00,0x29,0x00,0x1E,0x00,0x2B,0x00,0x30,0x00,0x32,0x00,0x1E,0x00,0x1E,0x00,0x1E,
	0x00,0x1E,0x00,0x37,0x00,0x1E,0x00,0x3A,0x00,0x40,0x00,0x1E,0x00,0x43,0x00,0x49,
	0x00,0x1E,0x00,0x4C,0x00,0x52,0x00,0x1E,0x00,0x55,0x00,0x5B,0x00,0x1E,0x00,0x5E,
	0x00,0x64,0x00,0x32,0x33,0x00,0x32,0x33,0x2E,0x35,0x00,0x46,0x6F,0x6F,0x62,0x61,
	0x72,0x00,0x34,0x32,0x00,0x35,0x2E,0x32,0x33,0x00,0x42,0x61,0x72,0x66,0x6F,0x6F,
	0x00,0x00,0x31,0x2E,0x30,0x30,0x00,0x51,0x75,0x75,0x78,0x00,0x35,0x00,0x42,0x6C,
	0x61,0x68,0x00,0x39,0x00,0x30,0x2E,0x31,0x30,0x00,0x31,0x32,0x00,0x54,0x65,0x73,
	0x74,0x31,0x00,0x31,0x33,0x00,0x54,0x65,0x73,0x74,0x32,0x00,0x31,0x34,0x00,0x54,
	0x65,0x73,0x74,0x33,0x00,0x31,0x35,0x00,0x54,0x65,0x73,0x74,0x34,0x00,0x31,0x36,
	0x00,0x54,0x65,0x73,0x74,0x35,0x00
};

static const char * const kHeaders[3] = {
	"ID", "FloatValue", "StringValue"
};

static const char * const kDataString[3][11] = {
	{ "23"    , "42"    , ""    , "5"   , "9"   , "", "12"   , "13"   , "14"   , "15"   , "16"   },
	{ "23.5"  , "5.23"  , "1.00", ""    , "0.10", "", ""     , ""     , ""     , ""     , ""     },
	{ "Foobar", "Barfoo", "Quux", "Blah", ""    , "", "Test1", "Test2", "Test3", "Test4", "Test5"}
};

static const int kDataInt[3][11] = {
	{ 23, 42, 0, 5, 9, 0, 12, 13, 14, 15, 16 },
	{  0,  0, 0, 0, 0, 0,  0,  0,  0,  0,  0 },
	{  0,  0, 0, 0, 0, 0,  0,  0,  0,  0,  0 }
};

static const float kDataFloat[3][11] = {
	{ 23.0f, 42.00f, 0.00f, 5.0f, 9.00f, 0.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f },
	{ 23.5f,  5.23f, 1.00f, 0.0f, 0.10f, 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f },
	{  0.0f,  0.00f, 0.00f, 0.0f, 0.00f, 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f }
};

static const bool kDataEmpty[3][11] = {
	{ false, false, true , false, false, true , false, false, false, false, false },
	{ false, false, false, true , false, true , true , true , true , true , true  },
	{ false, false, false, false, true , true , false, false, false, false, false }
};

// --- 2DA ASCII ---

GTEST_TEST(TwoDAFileASCII, getRowCount) {
	Common::MemoryReadStream stream(k2DAASCII);
	const Aurora::TwoDAFile twoda(stream);

	EXPECT_EQ(twoda.getRowCount(), ARRAYSIZE(kDataString[0]));
}

GTEST_TEST(TwoDAFileASCII, getColumnCount) {
	Common::MemoryReadStream stream(k2DAASCII);
	const Aurora::TwoDAFile twoda(stream);

	EXPECT_EQ(twoda.getColumnCount(), ARRAYSIZE(kHeaders));
}

GTEST_TEST(TwoDAFileASCII, getHeaders) {
	Common::MemoryReadStream stream(k2DAASCII);
	const Aurora::TwoDAFile twoda(stream);

	const std::vector<Common::UString> &headers = twoda.getHeaders();
	ASSERT_EQ(headers.size(), ARRAYSIZE(kHeaders));

	for (size_t i = 0; i < ARRAYSIZE(kHeaders); i++)
		EXPECT_STREQ(headers[i].c_str(), kHeaders[i]) << "At index " << i;
}

GTEST_TEST(TwoDAFileASCII, headerToColumn) {
	Common::MemoryReadStream stream(k2DAASCII);
	const Aurora::TwoDAFile twoda(stream);

	for (size_t i = 0; i < ARRAYSIZE(kHeaders); i++)
		EXPECT_EQ(twoda.headerToColumn(kHeaders[i]), i);

	EXPECT_EQ(twoda.headerToColumn("STRINGVALUE"), 2);
	EXPECT_EQ(twoda.headerToColumn("stringvalue"), 2);

	EXPECT_EQ(twoda.headerToColumn("Nope"), Aurora::kFieldIDInvalid);
}

GTEST_TEST(TwoDAFileASCII, getRow) {
	Common::MemoryReadStream stream(k2DAASCII);
	const Aurora::TwoDAFile twoda(stream);

	for (size_t i = 0; i < ARRAYSIZE(kDataString); i++) {
		for (size_t j = 0; j < ARRAYSIZE(kDataString[i]); j++) {
			if (kDataString[i][j][0] == '\0')
				continue;

			EXPECT_EQ(&twoda.getRow(kHeaders[i], kDataString[i][j]), &twoda.getRow(j)) <<
				"At index " << j << "." << i;
		}
	}

	EXPECT_EQ(&twoda.getRow("Nope", "0"   ), &twoda.getRow(Aurora::kFieldIDInvalid));
	EXPECT_EQ(&twoda.getRow("ID"  , "Nope"), &twoda.getRow(Aurora::kFieldIDInvalid));
}

GTEST_TEST(TwoDAFileASCII, writeBinary) {
	Common::MemoryReadStream stream(k2DAASCII);
	const Aurora::TwoDAFile twoda(stream);

	Common::MemoryWriteStreamDynamic writeStream(true);

	twoda.writeBinary(writeStream);

	ASSERT_EQ(writeStream.size(), sizeof(k2DABinary));
	for (size_t i = 0; i < sizeof(k2DABinary); i++)
		EXPECT_EQ(writeStream.getData()[i], k2DABinary[i]) << "At index " << i;
}

// --- 2DA Binary ---

GTEST_TEST(TwoDAFileBinary, getRowCount) {
	Common::MemoryReadStream stream(k2DABinary);
	const Aurora::TwoDAFile twoda(stream);

	EXPECT_EQ(twoda.getRowCount(), ARRAYSIZE(kDataString[0]));
}

GTEST_TEST(TwoDAFileBinary, getColumnCount) {
	Common::MemoryReadStream stream(k2DABinary);
	const Aurora::TwoDAFile twoda(stream);

	EXPECT_EQ(twoda.getColumnCount(), ARRAYSIZE(kHeaders));
}

GTEST_TEST(TwoDAFileBinary, getHeaders) {
	Common::MemoryReadStream stream(k2DABinary);
	const Aurora::TwoDAFile twoda(stream);

	const std::vector<Common::UString> &headers = twoda.getHeaders();
	ASSERT_EQ(headers.size(), ARRAYSIZE(kHeaders));

	for (size_t i = 0; i < ARRAYSIZE(kHeaders); i++)
		EXPECT_STREQ(headers[i].c_str(), kHeaders[i]) << "At index " << i;
}

GTEST_TEST(TwoDAFileBinary, headerToColumn) {
	Common::MemoryReadStream stream(k2DABinary);
	const Aurora::TwoDAFile twoda(stream);

	for (size_t i = 0; i < ARRAYSIZE(kHeaders); i++)
		EXPECT_EQ(twoda.headerToColumn(kHeaders[i]), i);

	EXPECT_EQ(twoda.headerToColumn("STRINGVALUE"), 2);
	EXPECT_EQ(twoda.headerToColumn("stringvalue"), 2);

	EXPECT_EQ(twoda.headerToColumn("Nope"), Aurora::kFieldIDInvalid);
}

GTEST_TEST(TwoDAFileBinary, getRow) {
	Common::MemoryReadStream stream(k2DABinary);
	const Aurora::TwoDAFile twoda(stream);

	for (size_t i = 0; i < ARRAYSIZE(kDataString); i++) {
		for (size_t j = 0; j < ARRAYSIZE(kDataString[i]); j++) {
			if (kDataString[i][j][0] == '\0')
				continue;

			EXPECT_EQ(&twoda.getRow(kHeaders[i], kDataString[i][j]), &twoda.getRow(j)) <<
				"At index " << j << "." << i;
		}
	}

	EXPECT_EQ(&twoda.getRow("Nope", "0"   ), &twoda.getRow(Aurora::kFieldIDInvalid));
	EXPECT_EQ(&twoda.getRow("ID"  , "Nope"), &twoda.getRow(Aurora::kFieldIDInvalid));
}

GTEST_TEST(TwoDAFileBinary, writeASCII) {
	Common::MemoryReadStream stream(k2DABinary);
	const Aurora::TwoDAFile twoda(stream);

	Common::MemoryWriteStreamDynamic writeStream(true);

	twoda.writeASCII(writeStream);

	ASSERT_EQ(writeStream.size(), strlen(k2DAASCII));
	for (size_t i = 0; i < strlen(k2DAASCII); i++)
		EXPECT_EQ(writeStream.getData()[i], k2DAASCII[i]) << "At index " << i;
}

// --- 2DA row ASCII ---

GTEST_TEST(TwoDARowASCII, emptyN) {
	Common::MemoryReadStream stream(k2DAASCII);
	const Aurora::TwoDAFile twoda(stream);

	for (size_t i = 0; i < ARRAYSIZE(kDataEmpty); i++)
		for (size_t j = 0; j < ARRAYSIZE(kDataEmpty[i]); j++)
			EXPECT_EQ(twoda.getRow(j).empty(i), kDataEmpty[i][j]) <<
				"At index " << j << "." << i;

	EXPECT_TRUE(twoda.getRow(Aurora::kFieldIDInvalid).empty(0));
	EXPECT_TRUE(twoda.getRow(0).empty(Aurora::kFieldIDInvalid));
}

GTEST_TEST(TwoDARowASCII, emptyStr) {
	Common::MemoryReadStream stream(k2DAASCII);
	const Aurora::TwoDAFile twoda(stream);

	for (size_t i = 0; i < ARRAYSIZE(kDataEmpty); i++)
		for (size_t j = 0; j < ARRAYSIZE(kDataEmpty[i]); j++)
			EXPECT_EQ(twoda.getRow(j).empty(kHeaders[i]), kDataEmpty[i][j]) <<
				"At index " << j << "." << i;

	EXPECT_TRUE(twoda.getRow(Aurora::kFieldIDInvalid).empty("Nope"));
	EXPECT_TRUE(twoda.getRow(0).empty("Nope"));
}

GTEST_TEST(TwoDARowASCII, getStringN) {
	Common::MemoryReadStream stream(k2DAASCII);
	const Aurora::TwoDAFile twoda(stream);

	for (size_t i = 0; i < ARRAYSIZE(kDataString); i++)
		for (size_t j = 0; j < ARRAYSIZE(kDataString[i]); j++)
			EXPECT_STREQ(twoda.getRow(j).getString(i).c_str(), kDataString[i][j]) <<
				"At index " << j << "." << i;

	EXPECT_STREQ(twoda.getRow(Aurora::kFieldIDInvalid).getString(0).c_str(), "");
	EXPECT_STREQ(twoda.getRow(0).getString(Aurora::kFieldIDInvalid).c_str(), "");
}

GTEST_TEST(TwoDARowASCII, getStringStr) {
	Common::MemoryReadStream stream(k2DAASCII);
	const Aurora::TwoDAFile twoda(stream);

	for (size_t i = 0; i < ARRAYSIZE(kDataString); i++)
		for (size_t j = 0; j < ARRAYSIZE(kDataString[i]); j++)
			EXPECT_STREQ(twoda.getRow(j).getString(kHeaders[i]).c_str(), kDataString[i][j]) <<
				"At index " << j << "." << i;

	EXPECT_STREQ(twoda.getRow(Aurora::kFieldIDInvalid).getString("Nope").c_str(), "");
	EXPECT_STREQ(twoda.getRow(0).getString("Nope").c_str(), "");
}

GTEST_TEST(TwoDARowASCII, getIntN) {
	Common::MemoryReadStream stream(k2DAASCII);
	const Aurora::TwoDAFile twoda(stream);

	for (size_t i = 0; i < ARRAYSIZE(kDataInt); i++)
		for (size_t j = 0; j < ARRAYSIZE(kDataInt[i]); j++)
			EXPECT_EQ(twoda.getRow(j).getInt(i), kDataInt[i][j]) <<
				"At index " << j << "." << i;

	EXPECT_EQ(twoda.getRow(Aurora::kFieldIDInvalid).getInt(0), 0);
	EXPECT_EQ(twoda.getRow(0).getInt(Aurora::kFieldIDInvalid), 0);
}

GTEST_TEST(TwoDARowASCII, getIntStr) {
	Common::MemoryReadStream stream(k2DAASCII);
	const Aurora::TwoDAFile twoda(stream);

	for (size_t i = 0; i < ARRAYSIZE(kDataInt); i++)
		for (size_t j = 0; j < ARRAYSIZE(kDataInt[i]); j++)
			EXPECT_EQ(twoda.getRow(j).getInt(kHeaders[i]), kDataInt[i][j]) <<
				"At index " << j << "." << i;

	EXPECT_EQ(twoda.getRow(Aurora::kFieldIDInvalid).getInt("Nope"), 0);
	EXPECT_EQ(twoda.getRow(0).getInt("Nope"), 0);
}

GTEST_TEST(TwoDARowASCII, getFloatN) {
	Common::MemoryReadStream stream(k2DAASCII);
	const Aurora::TwoDAFile twoda(stream);

	for (size_t i = 0; i < ARRAYSIZE(kDataFloat); i++)
		for (size_t j = 0; j < ARRAYSIZE(kDataFloat[i]); j++)
			EXPECT_FLOAT_EQ(twoda.getRow(j).getFloat(i), kDataFloat[i][j]) <<
				"At index " << j << "." << i;

	EXPECT_FLOAT_EQ(twoda.getRow(Aurora::kFieldIDInvalid).getFloat(0), 0.0f);
	EXPECT_FLOAT_EQ(twoda.getRow(0).getFloat(Aurora::kFieldIDInvalid), 0.0f);
}

GTEST_TEST(TwoDARowASCII, getFloatStr) {
	Common::MemoryReadStream stream(k2DAASCII);
	const Aurora::TwoDAFile twoda(stream);

	for (size_t i = 0; i < ARRAYSIZE(kDataFloat); i++)
		for (size_t j = 0; j < ARRAYSIZE(kDataFloat[i]); j++)
			EXPECT_FLOAT_EQ(twoda.getRow(j).getFloat(kHeaders[i]), kDataFloat[i][j]) <<
				"At index " << j << "." << i;

	EXPECT_FLOAT_EQ(twoda.getRow(Aurora::kFieldIDInvalid).getFloat("Nope"), 0.0f);
	EXPECT_FLOAT_EQ(twoda.getRow(0).getFloat("Nope"), 0.0f);
}

// --- 2DA row Binary ---

GTEST_TEST(TwoDARowBinary, emptyN) {
	Common::MemoryReadStream stream(k2DABinary);
	const Aurora::TwoDAFile twoda(stream);

	for (size_t i = 0; i < ARRAYSIZE(kDataEmpty); i++)
		for (size_t j = 0; j < ARRAYSIZE(kDataEmpty[i]); j++)
			EXPECT_EQ(twoda.getRow(j).empty(i), kDataEmpty[i][j]) <<
				"At index " << j << "." << i;

	EXPECT_TRUE(twoda.getRow(Aurora::kFieldIDInvalid).empty(0));
	EXPECT_TRUE(twoda.getRow(0).empty(Aurora::kFieldIDInvalid));
}

GTEST_TEST(TwoDARowBinary, emptyStr) {
	Common::MemoryReadStream stream(k2DABinary);
	const Aurora::TwoDAFile twoda(stream);

	for (size_t i = 0; i < ARRAYSIZE(kDataEmpty); i++)
		for (size_t j = 0; j < ARRAYSIZE(kDataEmpty[i]); j++)
			EXPECT_EQ(twoda.getRow(j).empty(kHeaders[i]), kDataEmpty[i][j]) <<
				"At index " << j << "." << i;

	EXPECT_TRUE(twoda.getRow(Aurora::kFieldIDInvalid).empty("Nope"));
	EXPECT_TRUE(twoda.getRow(0).empty("Nope"));
}

GTEST_TEST(TwoDARowBinary, getStringN) {
	Common::MemoryReadStream stream(k2DABinary);
	const Aurora::TwoDAFile twoda(stream);

	for (size_t i = 0; i < ARRAYSIZE(kDataString); i++)
		for (size_t j = 0; j < ARRAYSIZE(kDataString[i]); j++)
			EXPECT_STREQ(twoda.getRow(j).getString(i).c_str(), kDataString[i][j]) <<
				"At index " << j << "." << i;

	EXPECT_STREQ(twoda.getRow(Aurora::kFieldIDInvalid).getString(0).c_str(), "");
	EXPECT_STREQ(twoda.getRow(0).getString(Aurora::kFieldIDInvalid).c_str(), "");
}

GTEST_TEST(TwoDARowBinary, getStringStr) {
	Common::MemoryReadStream stream(k2DABinary);
	const Aurora::TwoDAFile twoda(stream);

	for (size_t i = 0; i < ARRAYSIZE(kDataString); i++)
		for (size_t j = 0; j < ARRAYSIZE(kDataString[i]); j++)
			EXPECT_STREQ(twoda.getRow(j).getString(kHeaders[i]).c_str(), kDataString[i][j]) <<
				"At index " << j << "." << i;

	EXPECT_STREQ(twoda.getRow(Aurora::kFieldIDInvalid).getString("Nope").c_str(), "");
	EXPECT_STREQ(twoda.getRow(0).getString("Nope").c_str(), "");
}

GTEST_TEST(TwoDARowBinary, getIntN) {
	Common::MemoryReadStream stream(k2DABinary);
	const Aurora::TwoDAFile twoda(stream);

	for (size_t i = 0; i < ARRAYSIZE(kDataInt); i++)
		for (size_t j = 0; j < ARRAYSIZE(kDataInt[i]); j++)
			EXPECT_EQ(twoda.getRow(j).getInt(i), kDataInt[i][j]) <<
				"At index " << j << "." << i;

	EXPECT_EQ(twoda.getRow(Aurora::kFieldIDInvalid).getInt(0), 0);
	EXPECT_EQ(twoda.getRow(0).getInt(Aurora::kFieldIDInvalid), 0);
}

GTEST_TEST(TwoDARowBinary, getIntStr) {
	Common::MemoryReadStream stream(k2DABinary);
	const Aurora::TwoDAFile twoda(stream);

	for (size_t i = 0; i < ARRAYSIZE(kDataInt); i++)
		for (size_t j = 0; j < ARRAYSIZE(kDataInt[i]); j++)
			EXPECT_EQ(twoda.getRow(j).getInt(kHeaders[i]), kDataInt[i][j]) <<
				"At index " << j << "." << i;

	EXPECT_EQ(twoda.getRow(Aurora::kFieldIDInvalid).getInt("Nope"), 0);
	EXPECT_EQ(twoda.getRow(0).getInt("Nope"), 0);
}

GTEST_TEST(TwoDARowBinary, getFloatN) {
	Common::MemoryReadStream stream(k2DABinary);
	const Aurora::TwoDAFile twoda(stream);

	for (size_t i = 0; i < ARRAYSIZE(kDataFloat); i++)
		for (size_t j = 0; j < ARRAYSIZE(kDataFloat[i]); j++)
			EXPECT_FLOAT_EQ(twoda.getRow(j).getFloat(i), kDataFloat[i][j]) <<
				"At index " << j << "." << i;

	EXPECT_FLOAT_EQ(twoda.getRow(Aurora::kFieldIDInvalid).getFloat(0), 0.0f);
	EXPECT_FLOAT_EQ(twoda.getRow(0).getFloat(Aurora::kFieldIDInvalid), 0.0f);
}

GTEST_TEST(TwoDARowBinary, getFloatStr) {
	Common::MemoryReadStream stream(k2DABinary);
	const Aurora::TwoDAFile twoda(stream);

	for (size_t i = 0; i < ARRAYSIZE(kDataFloat); i++)
		for (size_t j = 0; j < ARRAYSIZE(kDataFloat[i]); j++)
			EXPECT_FLOAT_EQ(twoda.getRow(j).getFloat(kHeaders[i]), kDataFloat[i][j]) <<
				"At index " << j << "." << i;

	EXPECT_FLOAT_EQ(twoda.getRow(Aurora::kFieldIDInvalid).getFloat("Nope"), 0.0f);
	EXPECT_FLOAT_EQ(twoda.getRow(0).getFloat("Nope"), 0.0f);
}

// --- 2DA variants ---

GTEST_TEST(TwoDAFileVariants, asciiTabs) {
	static const char *k2DAASCIITabs =
		"2DA	V2.0\n"
		"\n"
		"			ID			FloatValue	StringValue\n"
		"	0	23			23.5							Foobar					\n"
		"	1	42			5.23							Barfoo					\n"
		"	2	****	1.00							Quux							\n"
		"	3	5				****							Blah							\n"
		"	4	9				0.10							****							\n"
		"	5	****	****							****							\n"
		"	6	12			****							Test1						\n"
		"	7	13			****							Test2						\n"
		"	8	14			****							Test3						\n"
		"	9	15			****							Test4						\n"
		"10	16			****							Test5						\n";

	Common::MemoryReadStream stream(k2DAASCIITabs);
	const Aurora::TwoDAFile twoda(stream);

	EXPECT_EQ(twoda.getColumnCount(), ARRAYSIZE(kHeaders));
	EXPECT_EQ(twoda.getRowCount(), ARRAYSIZE(kDataString[0]));

	for (size_t i = 0; i < ARRAYSIZE(kDataString); i++)
		for (size_t j = 0; j < ARRAYSIZE(kDataString[i]); j++)
			EXPECT_STREQ(twoda.getRow(j).getString(kHeaders[i]).c_str(), kDataString[i][j]) <<
				"At index " << j << "." << i;

	EXPECT_STREQ(twoda.getRow(Aurora::kFieldIDInvalid).getString("Nope").c_str(), "");
	EXPECT_STREQ(twoda.getRow(0).getString("Nope").c_str(), "");
}

GTEST_TEST(TwoDAFileVariants, binaryTab) {
	static const byte k2DABinaryTab[] = {
		0x32,0x44,0x41,0x09,0x56,0x32,0x2E,0x62,0x0A,0x49,0x44,0x09,0x46,0x6C,0x6F,0x61,
		0x74,0x56,0x61,0x6C,0x75,0x65,0x09,0x53,0x74,0x72,0x69,0x6E,0x67,0x56,0x61,0x6C,
		0x75,0x65,0x09,0x00,0x0B,0x00,0x00,0x00,0x30,0x09,0x31,0x09,0x32,0x09,0x33,0x09,
		0x34,0x09,0x35,0x09,0x36,0x09,0x37,0x09,0x38,0x09,0x39,0x09,0x31,0x30,0x09,0x00,
		0x00,0x03,0x00,0x08,0x00,0x0F,0x00,0x12,0x00,0x17,0x00,0x1E,0x00,0x1F,0x00,0x24,
		0x00,0x29,0x00,0x1E,0x00,0x2B,0x00,0x30,0x00,0x32,0x00,0x1E,0x00,0x1E,0x00,0x1E,
		0x00,0x1E,0x00,0x37,0x00,0x1E,0x00,0x3A,0x00,0x40,0x00,0x1E,0x00,0x43,0x00,0x49,
		0x00,0x1E,0x00,0x4C,0x00,0x52,0x00,0x1E,0x00,0x55,0x00,0x5B,0x00,0x1E,0x00,0x5E,
		0x00,0x64,0x00,0x32,0x33,0x00,0x32,0x33,0x2E,0x35,0x00,0x46,0x6F,0x6F,0x62,0x61,
		0x72,0x00,0x34,0x32,0x00,0x35,0x2E,0x32,0x33,0x00,0x42,0x61,0x72,0x66,0x6F,0x6F,
		0x00,0x00,0x31,0x2E,0x30,0x30,0x00,0x51,0x75,0x75,0x78,0x00,0x35,0x00,0x42,0x6C,
		0x61,0x68,0x00,0x39,0x00,0x30,0x2E,0x31,0x30,0x00,0x31,0x32,0x00,0x54,0x65,0x73,
		0x74,0x31,0x00,0x31,0x33,0x00,0x54,0x65,0x73,0x74,0x32,0x00,0x31,0x34,0x00,0x54,
		0x65,0x73,0x74,0x33,0x00,0x31,0x35,0x00,0x54,0x65,0x73,0x74,0x34,0x00,0x31,0x36,
		0x00,0x54,0x65,0x73,0x74,0x35,0x00
	};

	Common::MemoryReadStream stream(k2DABinaryTab);
	const Aurora::TwoDAFile twoda(stream);

	EXPECT_EQ(twoda.getColumnCount(), ARRAYSIZE(kHeaders));
	EXPECT_EQ(twoda.getRowCount(), ARRAYSIZE(kDataString[0]));

	for (size_t i = 0; i < ARRAYSIZE(kDataString); i++)
		for (size_t j = 0; j < ARRAYSIZE(kDataString[i]); j++)
			EXPECT_STREQ(twoda.getRow(j).getString(kHeaders[i]).c_str(), kDataString[i][j]) <<
				"At index " << j << "." << i;

	EXPECT_STREQ(twoda.getRow(Aurora::kFieldIDInvalid).getString("Nope").c_str(), "");
	EXPECT_STREQ(twoda.getRow(0).getString("Nope").c_str(), "");
}

GTEST_TEST(TwoDAFileVariants, asciiMissingCells) {
	static const char *k2DAASCIIMissingCells =
		"2DA V2.0\n"
		"\n"
		"   ID   FloatValue StringValue\n"
		" 0 23   23.5       Foobar     \n"
		" 1 42   5.23       Barfoo     \n"
		" 2 **** 1.00       Quux       \n"
		" 3 5    ****       Blah       \n"
		" 4 9    0.10\n"
		" 5 **** ****\n"
		" 6 12   ****       Test1      \n"
		" 7 13   ****       Test2      \n"
		" 8 14   ****       Test3      \n"
		" 9 15   ****       Test4      \n"
		"10 16   ****       Test5      \n";

	Common::MemoryReadStream stream(k2DAASCIIMissingCells);
	const Aurora::TwoDAFile twoda(stream);

	EXPECT_EQ(twoda.getColumnCount(), ARRAYSIZE(kHeaders));
	EXPECT_EQ(twoda.getRowCount(), ARRAYSIZE(kDataString[0]));

	for (size_t i = 0; i < ARRAYSIZE(kDataString); i++)
		for (size_t j = 0; j < ARRAYSIZE(kDataString[i]); j++)
			EXPECT_STREQ(twoda.getRow(j).getString(kHeaders[i]).c_str(), kDataString[i][j]) <<
				"At index " << j << "." << i;

	EXPECT_STREQ(twoda.getRow(Aurora::kFieldIDInvalid).getString("Nope").c_str(), "");
	EXPECT_STREQ(twoda.getRow(0).getString("Nope").c_str(), "");
}

GTEST_TEST(TwoDAFileVariants, asciiExtraCells) {
	static const char *k2DAASCIIExtraCells =
		"2DA V2.0\n"
		"\n"
		"   ID   FloatValue StringValue\n"
		" 0 23   23.5       Foobar     Extra1\n"
		" 1 42   5.23       Barfoo     Extra2\n"
		" 2 **** 1.00       Quux       Extra3\n"
		" 3 5    ****       Blah       \n"
		" 4 9    0.10       ****       Extra4\n"
		" 5 **** ****       ****       Extra5\n"
		" 6 12   ****       Test1      Extra6\n"
		" 7 13   ****       Test2      \n"
		" 8 14   ****       Test3      Extra7\n"
		" 9 15   ****       Test4      Extra8\n"
		"10 16   ****       Test5      Extra9\n";

	Common::MemoryReadStream stream(k2DAASCIIExtraCells);
	const Aurora::TwoDAFile twoda(stream);

	EXPECT_EQ(twoda.getColumnCount(), ARRAYSIZE(kHeaders));
	EXPECT_EQ(twoda.getRowCount(), ARRAYSIZE(kDataString[0]));

	for (size_t i = 0; i < ARRAYSIZE(kDataString); i++)
		for (size_t j = 0; j < ARRAYSIZE(kDataString[i]); j++)
			EXPECT_STREQ(twoda.getRow(j).getString(kHeaders[i]).c_str(), kDataString[i][j]) <<
				"At index " << j << "." << i;

	EXPECT_STREQ(twoda.getRow(Aurora::kFieldIDInvalid).getString("Nope").c_str(), "");
	EXPECT_STREQ(twoda.getRow(0).getString("Nope").c_str(), "");
}

GTEST_TEST(TwoDAFileVariants, asciiWrongRowIndices) {
	static const char *k2DAASCIIWrongRowIndices =
		"2DA V2.0\n"
		"\n"
		"   ID   FloatValue StringValue\n"
		" 5 23   23.5       Foobar     \n"
		" 9 42   5.23       Barfoo     \n"
		" 0 **** 1.00       Quux       \n"
		"46 5    ****       Blah       \n"
		"99 9    0.10       ****       \n"
		" 2 **** ****       ****       \n"
		" 5 12   ****       Test1      \n"
		" 5 13   ****       Test2      \n"
		" 5 14   ****       Test3      \n"
		"23 15   ****       Test4      \n"
		"Foo 16   ****       Test5      \n";

	Common::MemoryReadStream stream(k2DAASCIIWrongRowIndices);
	const Aurora::TwoDAFile twoda(stream);

	EXPECT_EQ(twoda.getColumnCount(), ARRAYSIZE(kHeaders));
	EXPECT_EQ(twoda.getRowCount(), ARRAYSIZE(kDataString[0]));

	for (size_t i = 0; i < ARRAYSIZE(kDataString); i++)
		for (size_t j = 0; j < ARRAYSIZE(kDataString[i]); j++)
			EXPECT_STREQ(twoda.getRow(j).getString(kHeaders[i]).c_str(), kDataString[i][j]) <<
				"At index " << j << "." << i;

	EXPECT_STREQ(twoda.getRow(Aurora::kFieldIDInvalid).getString("Nope").c_str(), "");
	EXPECT_STREQ(twoda.getRow(0).getString("Nope").c_str(), "");
}

GTEST_TEST(TwoDAFileVariants, binaryWrongRowIndices) {
	static const byte k2DABinaryWrongRowIndices[] = {
		0x32,0x44,0x41,0x20,0x56,0x32,0x2E,0x62,0x0A,0x49,0x44,0x09,0x46,0x6C,0x6F,0x61,
		0x74,0x56,0x61,0x6C,0x75,0x65,0x09,0x53,0x74,0x72,0x69,0x6E,0x67,0x56,0x61,0x6C,
		0x75,0x65,0x09,0x00,0x0B,0x00,0x00,0x00,0x35,0x09,0x35,0x09,0x35,0x09,0x30,0x09,
		0x31,0x09,0x39,0x09,0x35,0x09,0x46,0x09,0x35,0x09,0x30,0x09,0x30,0x35,0x09,0x00,
		0x00,0x03,0x00,0x08,0x00,0x0F,0x00,0x12,0x00,0x17,0x00,0x1E,0x00,0x1F,0x00,0x24,
		0x00,0x29,0x00,0x1E,0x00,0x2B,0x00,0x30,0x00,0x32,0x00,0x1E,0x00,0x1E,0x00,0x1E,
		0x00,0x1E,0x00,0x37,0x00,0x1E,0x00,0x3A,0x00,0x40,0x00,0x1E,0x00,0x43,0x00,0x49,
		0x00,0x1E,0x00,0x4C,0x00,0x52,0x00,0x1E,0x00,0x55,0x00,0x5B,0x00,0x1E,0x00,0x5E,
		0x00,0x64,0x00,0x32,0x33,0x00,0x32,0x33,0x2E,0x35,0x00,0x46,0x6F,0x6F,0x62,0x61,
		0x72,0x00,0x34,0x32,0x00,0x35,0x2E,0x32,0x33,0x00,0x42,0x61,0x72,0x66,0x6F,0x6F,
		0x00,0x00,0x31,0x2E,0x30,0x30,0x00,0x51,0x75,0x75,0x78,0x00,0x35,0x00,0x42,0x6C,
		0x61,0x68,0x00,0x39,0x00,0x30,0x2E,0x31,0x30,0x00,0x31,0x32,0x00,0x54,0x65,0x73,
		0x74,0x31,0x00,0x31,0x33,0x00,0x54,0x65,0x73,0x74,0x32,0x00,0x31,0x34,0x00,0x54,
		0x65,0x73,0x74,0x33,0x00,0x31,0x35,0x00,0x54,0x65,0x73,0x74,0x34,0x00,0x31,0x36,
		0x00,0x54,0x65,0x73,0x74,0x35,0x00
	};

	Common::MemoryReadStream stream(k2DABinaryWrongRowIndices);
	const Aurora::TwoDAFile twoda(stream);

	EXPECT_EQ(twoda.getColumnCount(), ARRAYSIZE(kHeaders));
	EXPECT_EQ(twoda.getRowCount(), ARRAYSIZE(kDataString[0]));

	for (size_t i = 0; i < ARRAYSIZE(kDataString); i++)
		for (size_t j = 0; j < ARRAYSIZE(kDataString[i]); j++)
			EXPECT_STREQ(twoda.getRow(j).getString(kHeaders[i]).c_str(), kDataString[i][j]) <<
				"At index " << j << "." << i;

	EXPECT_STREQ(twoda.getRow(Aurora::kFieldIDInvalid).getString("Nope").c_str(), "");
	EXPECT_STREQ(twoda.getRow(0).getString("Nope").c_str(), "");
}

GTEST_TEST(TwoDAFileVariants, asciiEmpty) {
	static const char *k2DAASCIIEmpty = "2DA V2.0";

	Common::MemoryReadStream stream(k2DAASCIIEmpty);
	const Aurora::TwoDAFile twoda(stream);

	EXPECT_EQ(twoda.getColumnCount(), 0);
	EXPECT_EQ(twoda.getRowCount(), 0);
}

GTEST_TEST(TwoDAFileVariants, binaryEmpty) {
	static const byte k2DABinaryEmpty[] = "2DA V2.b\n""\x00""\x00""\x00""\x00""\x00""\x00""\x00";

	Common::MemoryReadStream stream(k2DABinaryEmpty);
	const Aurora::TwoDAFile twoda(stream);

	EXPECT_EQ(twoda.getColumnCount(), 0);
	EXPECT_EQ(twoda.getRowCount(), 0);
}

GTEST_TEST(TwoDAFileVariants, garbage) {
	static const byte k2DAAGarbage[] = "Nope";

	Common::MemoryReadStream stream(k2DAAGarbage);

	EXPECT_THROW(Aurora::TwoDAFile twoda(stream), Common::Exception);
}

GTEST_TEST(TwoDAFile, fromGDA) {
	static const byte kGDA[] = {
		0x47,0x46,0x46,0x20,0x56,0x34,0x2E,0x30,0x50,0x43,0x20,0x20,0x47,0x32,0x44,0x41,
		0x56,0x30,0x2E,0x31,0x03,0x00,0x00,0x00,0x94,0x00,0x00,0x00,0x67,0x74,0x6F,0x70,
		0x02,0x00,0x00,0x00,0x4C,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x63,0x6F,0x6C,0x6D,
		0x02,0x00,0x00,0x00,0x64,0x00,0x00,0x00,0x05,0x00,0x00,0x00,0x72,0x6F,0x77,0x73,
		0x02,0x00,0x00,0x00,0x7C,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x12,0x27,0x00,0x00,
		0x01,0x00,0x00,0xC0,0x00,0x00,0x00,0x00,0x13,0x27,0x00,0x00,0x02,0x00,0x00,0xC0,
		0x04,0x00,0x00,0x00,0x11,0x27,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0xF7,0x2A,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x15,0x27,0x00,0x00,
		0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x16,0x27,0x00,0x00,0x05,0x00,0x00,0x00,
		0x04,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x16,0x00,0x00,0x00,0x02,0x00,0x00,0x00,
		0x36,0xC9,0xFB,0x66,0x01,0xE1,0x3D,0xC1,0x3F,0x01,0x03,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x02,0x00,
		0x00,0x00,0x02,0x00,0x00,0x00
	};

	const Aurora::GDAFile gda(new Common::MemoryReadStream(kGDA));
	const Aurora::TwoDAFile twoda(gda);

	EXPECT_EQ(twoda.getColumnCount(), 2);
	EXPECT_EQ(twoda.getRowCount(), 3);

	EXPECT_STREQ(twoda.getHeaders()[0].c_str(), "ID");
	EXPECT_STREQ(twoda.getHeaders()[1].c_str(), "Value");

	for (size_t i = 0; i < 2; i++)
		for (size_t j = 0; j < 3; j++)
			EXPECT_EQ(twoda.getRow(j).getInt(i), j);
}
