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
 *  Unit tests for our GFF3 file reader class.
 */

/* NOTE: The following features aren't tested, because we have no
 * real-world examples files that use these, so we don't know how
 * they *should* work:
 * - A list of kFieldTypeASCIIString
 * - A list of kFieldTypeTlkString
 * - Fields (non-generic) that are both list and reference
 * - Generics that are lists and not references
 * - kFieldTypeASCIIString in a file with shared strings
 */

#include <algorithm>
#include <vector>

#include "gtest/gtest.h"

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/encoding.h"
#include "src/common/memreadstream.h"

#include "src/aurora/gff4file.h"

// --- GFF4, single values ---

static const byte kGFF4SingleValues[] = {
	0x47,0x46,0x46,0x20,0x56,0x34,0x2E,0x30,0x50,0x43,0x20,0x20,0x54,0x45,0x53,0x54,
	0x56,0x31,0x2E,0x30,0x01,0x00,0x00,0x00,0x10,0x01,0x00,0x00,0x53,0x54,0x43,0x54,
	0x13,0x00,0x00,0x00,0x2C,0x00,0x00,0x00,0xC0,0x00,0x00,0x00,0x00,0x01,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x00,0x00,0x01,0x00,0x00,0x00,
	0x01,0x00,0x00,0x00,0x02,0x01,0x00,0x00,0x02,0x00,0x00,0x00,0x02,0x00,0x00,0x00,
	0x03,0x01,0x00,0x00,0x03,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x04,0x01,0x00,0x00,
	0x04,0x00,0x00,0x00,0x06,0x00,0x00,0x00,0x05,0x01,0x00,0x00,0x05,0x00,0x00,0x00,
	0x0A,0x00,0x00,0x00,0x06,0x01,0x00,0x00,0x06,0x00,0x00,0x00,0x0E,0x00,0x00,0x00,
	0x07,0x01,0x00,0x00,0x07,0x00,0x00,0x00,0x16,0x00,0x00,0x00,0x00,0x02,0x00,0x00,
	0x08,0x00,0x00,0x00,0x1E,0x00,0x00,0x00,0x01,0x02,0x00,0x00,0x09,0x00,0x00,0x00,
	0x22,0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x0A,0x00,0x00,0x00,0x2A,0x00,0x00,0x00,
	0x01,0x03,0x00,0x00,0x0C,0x00,0x00,0x00,0x36,0x00,0x00,0x00,0x02,0x03,0x00,0x00,
	0x0D,0x00,0x00,0x00,0x46,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x0E,0x00,0x00,0x00,
	0x56,0x00,0x00,0x00,0x03,0x03,0x00,0x00,0x0F,0x00,0x00,0x00,0x5A,0x00,0x00,0x00,
	0x04,0x03,0x00,0x00,0x10,0x00,0x00,0x00,0x6A,0x00,0x00,0x00,0x01,0x04,0x00,0x00,
	0x11,0x00,0x00,0x00,0xAA,0x00,0x00,0x00,0x02,0x02,0x00,0x00,0x12,0x00,0x00,0x00,
	0xB2,0x00,0x00,0x00,0x02,0x04,0x00,0x00,0x14,0x00,0x00,0x00,0xB6,0x00,0x00,0x00,
	0x17,0xE9,0x18,0x00,0xE8,0xFF,0x19,0x00,0x00,0x00,0xE7,0xFF,0xFF,0xFF,0x1A,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0xE6,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xCD,0xCC,
	0xD8,0x41,0x33,0x33,0x33,0x33,0x33,0x33,0x3B,0x40,0xCD,0xCC,0xE0,0x41,0x9A,0x99,
	0xE1,0x41,0x66,0x66,0xE2,0x41,0xCD,0xCC,0xE8,0x41,0x9A,0x99,0xE9,0x41,0x66,0x66,
	0xEA,0x41,0x33,0x33,0xEB,0x41,0xCD,0xCC,0xF0,0x41,0x9A,0x99,0xF1,0x41,0x66,0x66,
	0xF2,0x41,0x33,0x33,0xF3,0x41,0xC0,0x00,0x00,0x00,0xCD,0xCC,0xF8,0x41,0x9A,0x99,
	0xF9,0x41,0x66,0x66,0xFA,0x41,0x33,0x33,0xFB,0x41,0x00,0x00,0x20,0x42,0x66,0x66,
	0x20,0x42,0xCD,0xCC,0x20,0x42,0x33,0x33,0x21,0x42,0x00,0x00,0x24,0x42,0x66,0x66,
	0x24,0x42,0xCD,0xCC,0x24,0x42,0x33,0x33,0x25,0x42,0x00,0x00,0x28,0x42,0x66,0x66,
	0x28,0x42,0xCD,0xCC,0x28,0x42,0x33,0x33,0x29,0x42,0x00,0x00,0x2C,0x42,0x66,0x66,
	0x2C,0x42,0xCD,0xCC,0x2C,0x42,0x33,0x33,0x2D,0x42,0x33,0x00,0x00,0x00,0xD0,0x00,
	0x00,0x00,0xAE,0x53,0x00,0x00,0x06,0x00,0x00,0x00,0x46,0x6F,0x6F,0x62,0x61,0x72,
	0x06,0x00,0x00,0x00,0x42,0x00,0x61,0x00,0x72,0x00,0x66,0x00,0x6F,0x00,0x6F,0x00,
	0x06,0x00,0x00,0x00,0x51,0x00,0x75,0x00,0x75,0x00,0x75,0x00,0x75,0x00,0x78,0x00
};

static const uint32_t kFieldLabelsSingle[] = {
	256, 257, 258, 259, 260, 261, 262, 263, 512, 513, 514, 768, 769, 770, 771, 772, 1024, 1025, 1026,
};

static const Aurora::GFF4Struct::FieldType kFieldTypesSingle[] = {
	Aurora::GFF4Struct::kFieldTypeUint8,
	Aurora::GFF4Struct::kFieldTypeSint8,
	Aurora::GFF4Struct::kFieldTypeUint16,
	Aurora::GFF4Struct::kFieldTypeSint16,
	Aurora::GFF4Struct::kFieldTypeUint32,
	Aurora::GFF4Struct::kFieldTypeSint32,
	Aurora::GFF4Struct::kFieldTypeUint64,
	Aurora::GFF4Struct::kFieldTypeSint64,
	Aurora::GFF4Struct::kFieldTypeFloat32,
	Aurora::GFF4Struct::kFieldTypeFloat64,
	Aurora::GFF4Struct::kFieldTypeNDSFixed,
	Aurora::GFF4Struct::kFieldTypeVector3f,
	Aurora::GFF4Struct::kFieldTypeVector4f,
	Aurora::GFF4Struct::kFieldTypeQuaternionf,
	Aurora::GFF4Struct::kFieldTypeColor4f,
	Aurora::GFF4Struct::kFieldTypeMatrix4x4f,
	Aurora::GFF4Struct::kFieldTypeString,
	Aurora::GFF4Struct::kFieldTypeTlkString,
	Aurora::GFF4Struct::kFieldTypeASCIIString
};

GTEST_TEST(GFF4File, getType) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4SingleValues));

	EXPECT_EQ(gff4.getType(), MKTAG('T', 'E', 'S', 'T'));
}

GTEST_TEST(GFF4File, enforceType) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4SingleValues), MKTAG('T', 'E', 'S', 'T'));

	EXPECT_THROW(Aurora::GFF4File gffNope(new Common::MemoryReadStream(kGFF4SingleValues), MKTAG('N', 'O', 'P', 'E')), Common::Exception);
}

GTEST_TEST(GFF4File, getTypeVersion) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4SingleValues));

	EXPECT_EQ(gff4.getTypeVersion(), MKTAG('V', '1', '.', '0'));
}

GTEST_TEST(GFF4File, getPlatform) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4SingleValues));

	EXPECT_EQ(gff4.getPlatform(), MKTAG('P', 'C', ' ', ' '));
}

GTEST_TEST(GFF4Struct, getRefCount) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4SingleValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	EXPECT_EQ(strct.getRefCount(), 1);
}

GTEST_TEST(GFF4Struct, getLabel) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4SingleValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	EXPECT_EQ(strct.getLabel(), MKTAG('S', 'T', 'C', 'T'));
}

GTEST_TEST(GFF4StructSingle, getFieldCount) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4SingleValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	EXPECT_EQ(strct.getFieldCount(), ARRAYSIZE(kFieldLabelsSingle));
}

GTEST_TEST(GFF4StructSingle, hasField) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4SingleValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	for (size_t i = 0; i < ARRAYSIZE(kFieldLabelsSingle); i++)
		EXPECT_TRUE(strct.hasField(kFieldLabelsSingle[i])) << "At index " << i;

	EXPECT_FALSE(strct.hasField(9999));
}

GTEST_TEST(GFF4StructSingle, getFieldLabels) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4SingleValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	std::vector<uint32_t> labels = strct.getFieldLabels();
	std::sort(labels.begin(), labels.end());

	for (size_t i = 0; i < ARRAYSIZE(kFieldLabelsSingle); i++)
		EXPECT_EQ(labels[i], kFieldLabelsSingle[i]) << "At index " << i;
}

GTEST_TEST(GFF4StructSingle, getFieldType) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4SingleValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	for (size_t i = 0; i < ARRAYSIZE(kFieldLabelsSingle); i++)
		EXPECT_EQ(strct.getFieldType(kFieldLabelsSingle[i]), kFieldTypesSingle[i]) << "At index " << i;

	EXPECT_EQ(strct.getFieldType(9999), Aurora::GFF4Struct::kFieldTypeNone);
}

GTEST_TEST(GFF4StructSingle, getFieldTypeList) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4SingleValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	for (size_t i = 0; i < ARRAYSIZE(kFieldLabelsSingle); i++) {
		bool isList;

		EXPECT_EQ(strct.getFieldType(kFieldLabelsSingle[i], isList), kFieldTypesSingle[i]) << "At index " << i;
		EXPECT_FALSE(isList);
	}

	bool isList;
	EXPECT_EQ(strct.getFieldType(9999, isList), Aurora::GFF4Struct::kFieldTypeNone);
}

GTEST_TEST(GFF4StructSingle, getFieldProperties) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4SingleValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	for (size_t i = 0; i < ARRAYSIZE(kFieldLabelsSingle); i++) {
		Aurora::GFF4Struct::FieldType type;
		uint32_t label;
		bool isList;

		EXPECT_TRUE(strct.getFieldProperties(kFieldLabelsSingle[i], type, label, isList)) << "At index " << i;
		EXPECT_EQ(type, kFieldTypesSingle[i]);
		EXPECT_EQ(label, kFieldLabelsSingle[i]);
		EXPECT_FALSE(isList);
	}

	Aurora::GFF4Struct::FieldType type;
	uint32_t label;
	bool isList;

	EXPECT_FALSE(strct.getFieldProperties(9999, type, label, isList));
}

GTEST_TEST(GFF4StructSingle, getUint) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4SingleValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	EXPECT_EQ(strct.getUint(256), 23);
	EXPECT_EQ(strct.getUint(258), 24);
	EXPECT_EQ(strct.getUint(260), 25);
	EXPECT_EQ(strct.getUint(262), 26);

	EXPECT_EQ(strct.getUint(257), (uint64_t)((int64_t) -23));
	EXPECT_EQ(strct.getUint(259), (uint64_t)((int64_t) -24));
	EXPECT_EQ(strct.getUint(261), (uint64_t)((int64_t) -25));
	EXPECT_EQ(strct.getUint(263), (uint64_t)((int64_t) -26));

	EXPECT_EQ(strct.getUint(9999, 9999), 9999);

	EXPECT_THROW(strct.getUint(1024), Common::Exception);
}

GTEST_TEST(GFF4StructSingle, getSint) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4SingleValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	EXPECT_EQ(strct.getSint(256), 23);
	EXPECT_EQ(strct.getSint(258), 24);
	EXPECT_EQ(strct.getSint(260), 25);
	EXPECT_EQ(strct.getSint(262), 26);

	EXPECT_EQ(strct.getSint(257), -23);
	EXPECT_EQ(strct.getSint(259), -24);
	EXPECT_EQ(strct.getSint(261), -25);
	EXPECT_EQ(strct.getSint(263), -26);

	EXPECT_EQ(strct.getSint(9999, 9999), 9999);

	EXPECT_THROW(strct.getSint(1024), Common::Exception);
}

GTEST_TEST(GFF4StructSingle, getBool) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4SingleValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	EXPECT_EQ(strct.getBool(256), true);
	EXPECT_EQ(strct.getBool(258), true);
	EXPECT_EQ(strct.getBool(260), true);
	EXPECT_EQ(strct.getBool(262), true);

	EXPECT_EQ(strct.getBool(257), true);
	EXPECT_EQ(strct.getBool(259), true);
	EXPECT_EQ(strct.getBool(261), true);
	EXPECT_EQ(strct.getBool(263), true);

	EXPECT_EQ(strct.getBool(9999, false), false);

	EXPECT_THROW(strct.getBool(1024), Common::Exception);
}

GTEST_TEST(GFF4StructSingle, getDouble) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4SingleValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	EXPECT_DOUBLE_EQ(strct.getDouble(512), 27.1f);
	EXPECT_DOUBLE_EQ(strct.getDouble(513), 27.2);

	// Nintendo fixed point
	EXPECT_NEAR(strct.getDouble(514), 5.23, 0.00005);

	EXPECT_DOUBLE_EQ(strct.getDouble(9999, 27.3), 27.3);

	EXPECT_THROW(strct.getDouble(1024), Common::Exception);
}

GTEST_TEST(GFF4StructSingle, getFloat) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4SingleValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	EXPECT_FLOAT_EQ(strct.getFloat(512), 27.1f);
	EXPECT_FLOAT_EQ(strct.getFloat(513), 27.2f);

	// Nintendo fixed point
	EXPECT_NEAR(strct.getFloat(514), 5.23f, 0.00005);

	EXPECT_FLOAT_EQ(strct.getFloat(9999, 27.3f), 27.3f);

	EXPECT_THROW(strct.getFloat(1024), Common::Exception);
}

GTEST_TEST(GFF4StructSingle, getString) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4SingleValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	EXPECT_STREQ(strct.getString(1024).c_str(), "Barfoo");
	EXPECT_STREQ(strct.getString(1026).c_str(), "Foobar");

	EXPECT_STREQ(strct.getString(9999).c_str(), "");

	EXPECT_THROW(strct.getString(512), Common::Exception);
}

GTEST_TEST(GFF4StructSingle, getStringEncoding) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4SingleValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	EXPECT_STREQ(strct.getString(1024, Common::kEncodingUTF16LE).c_str(), "Barfoo");
	EXPECT_STREQ(strct.getString(1026, Common::kEncodingASCII).c_str(), "Foobar");

	EXPECT_STREQ(strct.getString(9999, Common::kEncodingUTF8).c_str(), "");

	EXPECT_THROW(strct.getString(512, Common::kEncodingUTF8), Common::Exception);
}

GTEST_TEST(GFF4StructSingle, getTalkString) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4SingleValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	uint32_t strRef;
	Common::UString str;
	EXPECT_TRUE(strct.getTalkString(1025, strRef, str));

	EXPECT_EQ(strRef, 51);
	EXPECT_STREQ(str.c_str(), "Quuuux");

	EXPECT_FALSE(strct.getTalkString(9999, strRef, str));

	EXPECT_THROW(strct.getTalkString(512, strRef, str), Common::Exception);
}

GTEST_TEST(GFF4StructSingle, getTalkStringEncoding) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4SingleValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	uint32_t strRef;
	Common::UString str;
	EXPECT_TRUE(strct.getTalkString(1025, Common::kEncodingUTF16LE, strRef, str));

	EXPECT_EQ(strRef, 51);
	EXPECT_STREQ(str.c_str(), "Quuuux");

	EXPECT_FALSE(strct.getTalkString(9999, Common::kEncodingUTF16LE, strRef, str));

	EXPECT_THROW(strct.getTalkString(512, Common::kEncodingUTF16LE, strRef, str), Common::Exception);
}

GTEST_TEST(GFF4StructSingle, getVector3Double) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4SingleValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	double v1, v2, v3;
	EXPECT_TRUE(strct.getVector3(768, v1, v2, v3));

	EXPECT_DOUBLE_EQ(v1, 28.1f);
	EXPECT_DOUBLE_EQ(v2, 28.2f);
	EXPECT_DOUBLE_EQ(v3, 28.3f);

	EXPECT_FALSE(strct.getVector3(9999, v1, v2, v3));

	EXPECT_THROW(strct.getVector3(1024, v1, v2, v3), Common::Exception);
}

GTEST_TEST(GFF4StructSingle, getVector3Float) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4SingleValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	float v1, v2, v3;
	EXPECT_TRUE(strct.getVector3(768, v1, v2, v3));

	EXPECT_FLOAT_EQ(v1, 28.1f);
	EXPECT_FLOAT_EQ(v2, 28.2f);
	EXPECT_FLOAT_EQ(v3, 28.3f);

	EXPECT_FALSE(strct.getVector3(9999, v1, v2, v3));

	EXPECT_THROW(strct.getVector3(1024, v1, v2, v3), Common::Exception);
}

GTEST_TEST(GFF4StructSingle, getVector4Double) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4SingleValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	double v1, v2, v3, v4;

	EXPECT_TRUE(strct.getVector4(769, v1, v2, v3, v4));
	EXPECT_DOUBLE_EQ(v1, 29.1f);
	EXPECT_DOUBLE_EQ(v2, 29.2f);
	EXPECT_DOUBLE_EQ(v3, 29.3f);
	EXPECT_DOUBLE_EQ(v4, 29.4f);

	EXPECT_TRUE(strct.getVector4(770, v1, v2, v3, v4));
	EXPECT_DOUBLE_EQ(v1, 30.1f);
	EXPECT_DOUBLE_EQ(v2, 30.2f);
	EXPECT_DOUBLE_EQ(v3, 30.3f);
	EXPECT_DOUBLE_EQ(v4, 30.4f);

	EXPECT_TRUE(strct.getVector4(771, v1, v2, v3, v4));
	EXPECT_DOUBLE_EQ(v1, 31.1f);
	EXPECT_DOUBLE_EQ(v2, 31.2f);
	EXPECT_DOUBLE_EQ(v3, 31.3f);
	EXPECT_DOUBLE_EQ(v4, 31.4f);

	EXPECT_FALSE(strct.getVector4(9999, v1, v2, v3, v4));

	EXPECT_THROW(strct.getVector4(1024, v1, v2, v3, v4), Common::Exception);
}

GTEST_TEST(GFF4StructSingle, getVector4Float) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4SingleValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	float v1, v2, v3, v4;

	EXPECT_TRUE(strct.getVector4(769, v1, v2, v3, v4));
	EXPECT_FLOAT_EQ(v1, 29.1f);
	EXPECT_FLOAT_EQ(v2, 29.2f);
	EXPECT_FLOAT_EQ(v3, 29.3f);
	EXPECT_FLOAT_EQ(v4, 29.4f);

	EXPECT_TRUE(strct.getVector4(770, v1, v2, v3, v4));
	EXPECT_FLOAT_EQ(v1, 30.1f);
	EXPECT_FLOAT_EQ(v2, 30.2f);
	EXPECT_FLOAT_EQ(v3, 30.3f);
	EXPECT_FLOAT_EQ(v4, 30.4f);

	EXPECT_TRUE(strct.getVector4(771, v1, v2, v3, v4));
	EXPECT_FLOAT_EQ(v1, 31.1f);
	EXPECT_FLOAT_EQ(v2, 31.2f);
	EXPECT_FLOAT_EQ(v3, 31.3f);
	EXPECT_FLOAT_EQ(v4, 31.4f);

	EXPECT_FALSE(strct.getVector4(9999, v1, v2, v3, v4));

	EXPECT_THROW(strct.getVector4(1024, v1, v2, v3, v4), Common::Exception);
}

GTEST_TEST(GFF4StructSingle, getMatrix4x4Double) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4SingleValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	double m[16];
	EXPECT_TRUE(strct.getMatrix4x4(772, m));

	EXPECT_DOUBLE_EQ(m[ 0], 40.0f);
	EXPECT_DOUBLE_EQ(m[ 1], 40.1f);
	EXPECT_DOUBLE_EQ(m[ 2], 40.2f);
	EXPECT_DOUBLE_EQ(m[ 3], 40.3f);
	EXPECT_DOUBLE_EQ(m[ 4], 41.0f);
	EXPECT_DOUBLE_EQ(m[ 5], 41.1f);
	EXPECT_DOUBLE_EQ(m[ 6], 41.2f);
	EXPECT_DOUBLE_EQ(m[ 7], 41.3f);
	EXPECT_DOUBLE_EQ(m[ 8], 42.0f);
	EXPECT_DOUBLE_EQ(m[ 9], 42.1f);
	EXPECT_DOUBLE_EQ(m[10], 42.2f);
	EXPECT_DOUBLE_EQ(m[11], 42.3f);
	EXPECT_DOUBLE_EQ(m[12], 43.0f);
	EXPECT_DOUBLE_EQ(m[13], 43.1f);
	EXPECT_DOUBLE_EQ(m[14], 43.2f);
	EXPECT_DOUBLE_EQ(m[15], 43.3f);

	EXPECT_FALSE(strct.getMatrix4x4(9999, m));

	EXPECT_THROW(strct.getMatrix4x4(1024, m), Common::Exception);
}

GTEST_TEST(GFF4StructSingle, getMatrix4x4Float) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4SingleValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	float m[16];
	EXPECT_TRUE(strct.getMatrix4x4(772, m));

	EXPECT_FLOAT_EQ(m[ 0], 40.0f);
	EXPECT_FLOAT_EQ(m[ 1], 40.1f);
	EXPECT_FLOAT_EQ(m[ 2], 40.2f);
	EXPECT_FLOAT_EQ(m[ 3], 40.3f);
	EXPECT_FLOAT_EQ(m[ 4], 41.0f);
	EXPECT_FLOAT_EQ(m[ 5], 41.1f);
	EXPECT_FLOAT_EQ(m[ 6], 41.2f);
	EXPECT_FLOAT_EQ(m[ 7], 41.3f);
	EXPECT_FLOAT_EQ(m[ 8], 42.0f);
	EXPECT_FLOAT_EQ(m[ 9], 42.1f);
	EXPECT_FLOAT_EQ(m[10], 42.2f);
	EXPECT_FLOAT_EQ(m[11], 42.3f);
	EXPECT_FLOAT_EQ(m[12], 43.0f);
	EXPECT_FLOAT_EQ(m[13], 43.1f);
	EXPECT_FLOAT_EQ(m[14], 43.2f);
	EXPECT_FLOAT_EQ(m[15], 43.3f);

	EXPECT_FALSE(strct.getMatrix4x4(9999, m));

	EXPECT_THROW(strct.getMatrix4x4(1024, m), Common::Exception);
}

GTEST_TEST(GFF4StructSingle, getMatrix4x4Matrix) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4SingleValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	glm::mat4 m;
	EXPECT_TRUE(strct.getMatrix4x4(772, m));

	EXPECT_FLOAT_EQ(m[0][0], 40.0f);
	EXPECT_FLOAT_EQ(m[0][1], 40.1f);
	EXPECT_FLOAT_EQ(m[0][2], 40.2f);
	EXPECT_FLOAT_EQ(m[0][3], 40.3f);
	EXPECT_FLOAT_EQ(m[1][0], 41.0f);
	EXPECT_FLOAT_EQ(m[1][1], 41.1f);
	EXPECT_FLOAT_EQ(m[1][2], 41.2f);
	EXPECT_FLOAT_EQ(m[1][3], 41.3f);
	EXPECT_FLOAT_EQ(m[2][0], 42.0f);
	EXPECT_FLOAT_EQ(m[2][1], 42.1f);
	EXPECT_FLOAT_EQ(m[2][2], 42.2f);
	EXPECT_FLOAT_EQ(m[2][3], 42.3f);
	EXPECT_FLOAT_EQ(m[3][0], 43.0f);
	EXPECT_FLOAT_EQ(m[3][1], 43.1f);
	EXPECT_FLOAT_EQ(m[3][2], 43.2f);
	EXPECT_FLOAT_EQ(m[3][3], 43.3f);

	EXPECT_FALSE(strct.getMatrix4x4(9999, m));

	EXPECT_THROW(strct.getMatrix4x4(1024, m), Common::Exception);
}

GTEST_TEST(GFF4StructSingle, getVectorMatrixDouble) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4SingleValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	std::vector<double> v;

	EXPECT_TRUE(strct.getVectorMatrix(768, v));
	ASSERT_EQ(v.size(), 3);
	EXPECT_DOUBLE_EQ(v[0], 28.1f);
	EXPECT_DOUBLE_EQ(v[1], 28.2f);
	EXPECT_DOUBLE_EQ(v[2], 28.3f);

	EXPECT_TRUE(strct.getVectorMatrix(769, v));
	ASSERT_EQ(v.size(), 4);
	EXPECT_DOUBLE_EQ(v[0], 29.1f);
	EXPECT_DOUBLE_EQ(v[1], 29.2f);
	EXPECT_DOUBLE_EQ(v[2], 29.3f);
	EXPECT_DOUBLE_EQ(v[3], 29.4f);

	EXPECT_TRUE(strct.getVectorMatrix(770, v));
	ASSERT_EQ(v.size(), 4);
	EXPECT_DOUBLE_EQ(v[0], 30.1f);
	EXPECT_DOUBLE_EQ(v[1], 30.2f);
	EXPECT_DOUBLE_EQ(v[2], 30.3f);
	EXPECT_DOUBLE_EQ(v[3], 30.4f);

	EXPECT_TRUE(strct.getVectorMatrix(771, v));
	ASSERT_EQ(v.size(), 4);
	EXPECT_DOUBLE_EQ(v[0], 31.1f);
	EXPECT_DOUBLE_EQ(v[1], 31.2f);
	EXPECT_DOUBLE_EQ(v[2], 31.3f);
	EXPECT_DOUBLE_EQ(v[3], 31.4f);

	EXPECT_TRUE(strct.getVectorMatrix(772, v));
	ASSERT_EQ(v.size(), 16);
	EXPECT_DOUBLE_EQ(v[ 0], 40.0f);
	EXPECT_DOUBLE_EQ(v[ 1], 40.1f);
	EXPECT_DOUBLE_EQ(v[ 2], 40.2f);
	EXPECT_DOUBLE_EQ(v[ 3], 40.3f);
	EXPECT_DOUBLE_EQ(v[ 4], 41.0f);
	EXPECT_DOUBLE_EQ(v[ 5], 41.1f);
	EXPECT_DOUBLE_EQ(v[ 6], 41.2f);
	EXPECT_DOUBLE_EQ(v[ 7], 41.3f);
	EXPECT_DOUBLE_EQ(v[ 8], 42.0f);
	EXPECT_DOUBLE_EQ(v[ 9], 42.1f);
	EXPECT_DOUBLE_EQ(v[10], 42.2f);
	EXPECT_DOUBLE_EQ(v[11], 42.3f);
	EXPECT_DOUBLE_EQ(v[12], 43.0f);
	EXPECT_DOUBLE_EQ(v[13], 43.1f);
	EXPECT_DOUBLE_EQ(v[14], 43.2f);
	EXPECT_DOUBLE_EQ(v[15], 43.3f);

	EXPECT_FALSE(strct.getVectorMatrix(9999, v));

	EXPECT_THROW(strct.getVectorMatrix(1024, v), Common::Exception);
}

GTEST_TEST(GFF4StructSingle, getVectorMatrixFloat) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4SingleValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	std::vector<float> v;

	EXPECT_TRUE(strct.getVectorMatrix(768, v));
	ASSERT_EQ(v.size(), 3);
	EXPECT_FLOAT_EQ(v[0], 28.1f);
	EXPECT_FLOAT_EQ(v[1], 28.2f);
	EXPECT_FLOAT_EQ(v[2], 28.3f);

	EXPECT_TRUE(strct.getVectorMatrix(769, v));
	ASSERT_EQ(v.size(), 4);
	EXPECT_FLOAT_EQ(v[0], 29.1f);
	EXPECT_FLOAT_EQ(v[1], 29.2f);
	EXPECT_FLOAT_EQ(v[2], 29.3f);
	EXPECT_FLOAT_EQ(v[3], 29.4f);

	EXPECT_TRUE(strct.getVectorMatrix(770, v));
	ASSERT_EQ(v.size(), 4);
	EXPECT_FLOAT_EQ(v[0], 30.1f);
	EXPECT_FLOAT_EQ(v[1], 30.2f);
	EXPECT_FLOAT_EQ(v[2], 30.3f);
	EXPECT_FLOAT_EQ(v[3], 30.4f);

	EXPECT_TRUE(strct.getVectorMatrix(771, v));
	ASSERT_EQ(v.size(), 4);
	EXPECT_FLOAT_EQ(v[0], 31.1f);
	EXPECT_FLOAT_EQ(v[1], 31.2f);
	EXPECT_FLOAT_EQ(v[2], 31.3f);
	EXPECT_FLOAT_EQ(v[3], 31.4f);

	EXPECT_TRUE(strct.getVectorMatrix(772, v));
	ASSERT_EQ(v.size(), 16);
	EXPECT_FLOAT_EQ(v[ 0], 40.0f);
	EXPECT_FLOAT_EQ(v[ 1], 40.1f);
	EXPECT_FLOAT_EQ(v[ 2], 40.2f);
	EXPECT_FLOAT_EQ(v[ 3], 40.3f);
	EXPECT_FLOAT_EQ(v[ 4], 41.0f);
	EXPECT_FLOAT_EQ(v[ 5], 41.1f);
	EXPECT_FLOAT_EQ(v[ 6], 41.2f);
	EXPECT_FLOAT_EQ(v[ 7], 41.3f);
	EXPECT_FLOAT_EQ(v[ 8], 42.0f);
	EXPECT_FLOAT_EQ(v[ 9], 42.1f);
	EXPECT_FLOAT_EQ(v[10], 42.2f);
	EXPECT_FLOAT_EQ(v[11], 42.3f);
	EXPECT_FLOAT_EQ(v[12], 43.0f);
	EXPECT_FLOAT_EQ(v[13], 43.1f);
	EXPECT_FLOAT_EQ(v[14], 43.2f);
	EXPECT_FLOAT_EQ(v[15], 43.3f);

	EXPECT_FALSE(strct.getVectorMatrix(9999, v));

	EXPECT_THROW(strct.getVectorMatrix(1024, v), Common::Exception);
}


GTEST_TEST(GFF4StructSingle, getUintList) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4SingleValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	std::vector<uint64_t> list;

	EXPECT_TRUE(strct.getUint(256, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], 23);

	EXPECT_TRUE(strct.getUint(258, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], 24);

	EXPECT_TRUE(strct.getUint(260, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], 25);

	EXPECT_TRUE(strct.getUint(262, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], 26);

	EXPECT_TRUE(strct.getUint(257, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], (uint64_t)((int64_t) -23));

	EXPECT_TRUE(strct.getUint(259, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], (uint64_t)((int64_t) -24));

	EXPECT_TRUE(strct.getUint(261, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], (uint64_t)((int64_t) -25));

	EXPECT_TRUE(strct.getUint(263, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], (uint64_t)((int64_t) -26));

	EXPECT_FALSE(strct.getUint(9999, list));

	EXPECT_THROW(strct.getUint(1024, list), Common::Exception);
}

GTEST_TEST(GFF4StructSingle, getSintList) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4SingleValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	std::vector<int64_t> list;

	EXPECT_TRUE(strct.getSint(256, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], 23);

	EXPECT_TRUE(strct.getSint(258, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], 24);

	EXPECT_TRUE(strct.getSint(260, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], 25);

	EXPECT_TRUE(strct.getSint(262, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], 26);

	EXPECT_TRUE(strct.getSint(257, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], -23);

	EXPECT_TRUE(strct.getSint(259, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], -24);

	EXPECT_TRUE(strct.getSint(261, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], -25);

	EXPECT_TRUE(strct.getSint(263, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], -26);

	EXPECT_FALSE(strct.getSint(9999, list));

	EXPECT_THROW(strct.getSint(1024, list), Common::Exception);
}

GTEST_TEST(GFF4StructSingle, getBoolList) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4SingleValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	std::vector<bool> list;

	EXPECT_TRUE(strct.getBool(256, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], true);

	EXPECT_TRUE(strct.getBool(258, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], true);

	EXPECT_TRUE(strct.getBool(260, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], true);

	EXPECT_TRUE(strct.getBool(262, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], true);

	EXPECT_TRUE(strct.getBool(257, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], true);

	EXPECT_TRUE(strct.getBool(259, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], true);

	EXPECT_TRUE(strct.getBool(261, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], true);

	EXPECT_TRUE(strct.getBool(263, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], true);

	EXPECT_FALSE(strct.getBool(9999, list));

	EXPECT_THROW(strct.getBool(1024, list), Common::Exception);
}

GTEST_TEST(GFF4StructSingle, getDoubleList) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4SingleValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	std::vector<double> list;

	EXPECT_TRUE(strct.getDouble(512, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_DOUBLE_EQ(list[0], 27.1f);

	EXPECT_TRUE(strct.getDouble(513, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_DOUBLE_EQ(list[0], 27.2);

	// Nintendo fixed point
	EXPECT_TRUE(strct.getDouble(514, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_NEAR(list[0], 5.23, 0.00005);

	EXPECT_FALSE(strct.getDouble(9999, list));

	EXPECT_THROW(strct.getDouble(1024, list), Common::Exception);
}

GTEST_TEST(GFF4StructSingle, getFloatList) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4SingleValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	std::vector<float> list;

	EXPECT_TRUE(strct.getFloat(512, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_FLOAT_EQ(list[0], 27.1f);

	EXPECT_TRUE(strct.getFloat(513, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_FLOAT_EQ(list[0], 27.2f);

	// Nintendo fixed point
	EXPECT_TRUE(strct.getFloat(514, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_NEAR(list[0], 5.23f, 0.00005);

	EXPECT_FALSE(strct.getFloat(9999, list));

	EXPECT_THROW(strct.getFloat(1024, list), Common::Exception);
}

GTEST_TEST(GFF4StructSingle, getStringList) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4SingleValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	std::vector<Common::UString> list;

	EXPECT_TRUE(strct.getString(1024, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_STREQ(list[0].c_str(), "Barfoo");

	EXPECT_TRUE(strct.getString(1026, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_STREQ(list[0].c_str(), "Foobar");

	EXPECT_FALSE(strct.getString(9999, list));

	EXPECT_THROW(strct.getString(512, list), Common::Exception);
}

GTEST_TEST(GFF4StructSingle, getStringEncodingList) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4SingleValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	std::vector<Common::UString> list;

	EXPECT_TRUE(strct.getString(1024, Common::kEncodingUTF16LE, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_STREQ(list[0].c_str(), "Barfoo");

	EXPECT_TRUE(strct.getString(1026, Common::kEncodingASCII, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_STREQ(list[0].c_str(), "Foobar");

	EXPECT_FALSE(strct.getString(9999, Common::kEncodingUTF8, list));

	EXPECT_THROW(strct.getString(512, Common::kEncodingUTF8, list), Common::Exception);
}

GTEST_TEST(GFF4StructSingle, getTalkStringList) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4SingleValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	std::vector<uint32_t> strRefs;
	std::vector<Common::UString> strs;

	EXPECT_TRUE(strct.getTalkString(1025, strRefs, strs));

	ASSERT_EQ(strRefs.size(), 1);
	ASSERT_EQ(strs.size(), 1);

	EXPECT_EQ(strRefs[0], 51);
	EXPECT_STREQ(strs[0].c_str(), "Quuuux");

	EXPECT_FALSE(strct.getTalkString(9999, strRefs, strs));

	EXPECT_THROW(strct.getTalkString(512, strRefs, strs), Common::Exception);
}

GTEST_TEST(GFF4StructSingle, getTalkStringEncodingList) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4SingleValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	std::vector<uint32_t> strRefs;
	std::vector<Common::UString> strs;

	EXPECT_TRUE(strct.getTalkString(1025, Common::kEncodingUTF16LE, strRefs, strs));

	ASSERT_EQ(strRefs.size(), 1);
	ASSERT_EQ(strs.size(), 1);

	EXPECT_EQ(strRefs[0], 51);
	EXPECT_STREQ(strs[0].c_str(), "Quuuux");

	EXPECT_FALSE(strct.getTalkString(9999, Common::kEncodingUTF16LE, strRefs, strs));

	EXPECT_THROW(strct.getTalkString(512, Common::kEncodingUTF16LE, strRefs, strs), Common::Exception);
}

GTEST_TEST(GFF4StructSingle, getMatrix4x4MatrixList) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4SingleValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	std::vector<glm::mat4> list;

	EXPECT_TRUE(strct.getMatrix4x4(772, list));
	ASSERT_EQ(list.size(), 1);

	EXPECT_FLOAT_EQ(list[0][0][0], 40.0f);
	EXPECT_FLOAT_EQ(list[0][0][1], 40.1f);
	EXPECT_FLOAT_EQ(list[0][0][2], 40.2f);
	EXPECT_FLOAT_EQ(list[0][0][3], 40.3f);
	EXPECT_FLOAT_EQ(list[0][1][0], 41.0f);
	EXPECT_FLOAT_EQ(list[0][1][1], 41.1f);
	EXPECT_FLOAT_EQ(list[0][1][2], 41.2f);
	EXPECT_FLOAT_EQ(list[0][1][3], 41.3f);
	EXPECT_FLOAT_EQ(list[0][2][0], 42.0f);
	EXPECT_FLOAT_EQ(list[0][2][1], 42.1f);
	EXPECT_FLOAT_EQ(list[0][2][2], 42.2f);
	EXPECT_FLOAT_EQ(list[0][2][3], 42.3f);
	EXPECT_FLOAT_EQ(list[0][3][0], 43.0f);
	EXPECT_FLOAT_EQ(list[0][3][1], 43.1f);
	EXPECT_FLOAT_EQ(list[0][3][2], 43.2f);
	EXPECT_FLOAT_EQ(list[0][3][3], 43.3f);

	EXPECT_FALSE(strct.getMatrix4x4(9999, list));

	EXPECT_THROW(strct.getMatrix4x4(1024, list), Common::Exception);
}

GTEST_TEST(GFF4StructSingle, getVectorMatrixDoubleList) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4SingleValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	std::vector< std::vector<double> > v;

	EXPECT_TRUE(strct.getVectorMatrix(768, v));
	ASSERT_EQ(v.size(), 1);
	ASSERT_EQ(v[0].size(), 3);
	EXPECT_DOUBLE_EQ(v[0][0], 28.1f);
	EXPECT_DOUBLE_EQ(v[0][1], 28.2f);
	EXPECT_DOUBLE_EQ(v[0][2], 28.3f);

	EXPECT_TRUE(strct.getVectorMatrix(769, v));
	ASSERT_EQ(v.size(), 1);
	ASSERT_EQ(v[0].size(), 4);
	EXPECT_DOUBLE_EQ(v[0][0], 29.1f);
	EXPECT_DOUBLE_EQ(v[0][1], 29.2f);
	EXPECT_DOUBLE_EQ(v[0][2], 29.3f);
	EXPECT_DOUBLE_EQ(v[0][3], 29.4f);

	EXPECT_TRUE(strct.getVectorMatrix(770, v));
	ASSERT_EQ(v.size(), 1);
	ASSERT_EQ(v[0].size(), 4);
	EXPECT_DOUBLE_EQ(v[0][0], 30.1f);
	EXPECT_DOUBLE_EQ(v[0][1], 30.2f);
	EXPECT_DOUBLE_EQ(v[0][2], 30.3f);
	EXPECT_DOUBLE_EQ(v[0][3], 30.4f);

	EXPECT_TRUE(strct.getVectorMatrix(771, v));
	ASSERT_EQ(v.size(), 1);
	ASSERT_EQ(v[0].size(), 4);
	EXPECT_DOUBLE_EQ(v[0][0], 31.1f);
	EXPECT_DOUBLE_EQ(v[0][1], 31.2f);
	EXPECT_DOUBLE_EQ(v[0][2], 31.3f);
	EXPECT_DOUBLE_EQ(v[0][3], 31.4f);

	EXPECT_TRUE(strct.getVectorMatrix(772, v));
	ASSERT_EQ(v.size(), 1);
	ASSERT_EQ(v[0].size(), 16);
	EXPECT_DOUBLE_EQ(v[0][ 0], 40.0f);
	EXPECT_DOUBLE_EQ(v[0][ 1], 40.1f);
	EXPECT_DOUBLE_EQ(v[0][ 2], 40.2f);
	EXPECT_DOUBLE_EQ(v[0][ 3], 40.3f);
	EXPECT_DOUBLE_EQ(v[0][ 4], 41.0f);
	EXPECT_DOUBLE_EQ(v[0][ 5], 41.1f);
	EXPECT_DOUBLE_EQ(v[0][ 6], 41.2f);
	EXPECT_DOUBLE_EQ(v[0][ 7], 41.3f);
	EXPECT_DOUBLE_EQ(v[0][ 8], 42.0f);
	EXPECT_DOUBLE_EQ(v[0][ 9], 42.1f);
	EXPECT_DOUBLE_EQ(v[0][10], 42.2f);
	EXPECT_DOUBLE_EQ(v[0][11], 42.3f);
	EXPECT_DOUBLE_EQ(v[0][12], 43.0f);
	EXPECT_DOUBLE_EQ(v[0][13], 43.1f);
	EXPECT_DOUBLE_EQ(v[0][14], 43.2f);
	EXPECT_DOUBLE_EQ(v[0][15], 43.3f);

	EXPECT_FALSE(strct.getVectorMatrix(9999, v));

	EXPECT_THROW(strct.getVectorMatrix(1024, v), Common::Exception);
}

GTEST_TEST(GFF4StructSingle, getVectorMatrixFloatList) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4SingleValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	std::vector< std::vector<float> > v;

	EXPECT_TRUE(strct.getVectorMatrix(768, v));
	ASSERT_EQ(v.size(), 1);
	ASSERT_EQ(v[0].size(), 3);
	EXPECT_FLOAT_EQ(v[0][0], 28.1f);
	EXPECT_FLOAT_EQ(v[0][1], 28.2f);
	EXPECT_FLOAT_EQ(v[0][2], 28.3f);

	EXPECT_TRUE(strct.getVectorMatrix(769, v));
	ASSERT_EQ(v.size(), 1);
	ASSERT_EQ(v[0].size(), 4);
	EXPECT_FLOAT_EQ(v[0][0], 29.1f);
	EXPECT_FLOAT_EQ(v[0][1], 29.2f);
	EXPECT_FLOAT_EQ(v[0][2], 29.3f);
	EXPECT_FLOAT_EQ(v[0][3], 29.4f);

	EXPECT_TRUE(strct.getVectorMatrix(770, v));
	ASSERT_EQ(v.size(), 1);
	ASSERT_EQ(v[0].size(), 4);
	EXPECT_FLOAT_EQ(v[0][0], 30.1f);
	EXPECT_FLOAT_EQ(v[0][1], 30.2f);
	EXPECT_FLOAT_EQ(v[0][2], 30.3f);
	EXPECT_FLOAT_EQ(v[0][3], 30.4f);

	EXPECT_TRUE(strct.getVectorMatrix(771, v));
	ASSERT_EQ(v.size(), 1);
	ASSERT_EQ(v[0].size(), 4);
	EXPECT_FLOAT_EQ(v[0][0], 31.1f);
	EXPECT_FLOAT_EQ(v[0][1], 31.2f);
	EXPECT_FLOAT_EQ(v[0][2], 31.3f);
	EXPECT_FLOAT_EQ(v[0][3], 31.4f);

	EXPECT_TRUE(strct.getVectorMatrix(772, v));
	ASSERT_EQ(v.size(), 1);
	ASSERT_EQ(v[0].size(), 16);
	EXPECT_FLOAT_EQ(v[0][ 0], 40.0f);
	EXPECT_FLOAT_EQ(v[0][ 1], 40.1f);
	EXPECT_FLOAT_EQ(v[0][ 2], 40.2f);
	EXPECT_FLOAT_EQ(v[0][ 3], 40.3f);
	EXPECT_FLOAT_EQ(v[0][ 4], 41.0f);
	EXPECT_FLOAT_EQ(v[0][ 5], 41.1f);
	EXPECT_FLOAT_EQ(v[0][ 6], 41.2f);
	EXPECT_FLOAT_EQ(v[0][ 7], 41.3f);
	EXPECT_FLOAT_EQ(v[0][ 8], 42.0f);
	EXPECT_FLOAT_EQ(v[0][ 9], 42.1f);
	EXPECT_FLOAT_EQ(v[0][10], 42.2f);
	EXPECT_FLOAT_EQ(v[0][11], 42.3f);
	EXPECT_FLOAT_EQ(v[0][12], 43.0f);
	EXPECT_FLOAT_EQ(v[0][13], 43.1f);
	EXPECT_FLOAT_EQ(v[0][14], 43.2f);
	EXPECT_FLOAT_EQ(v[0][15], 43.3f);

	EXPECT_FALSE(strct.getVectorMatrix(9999, v));

	EXPECT_THROW(strct.getVectorMatrix(1024, v), Common::Exception);
}

GTEST_TEST(GFF4StructSingle, getData) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4SingleValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	Common::SeekableReadStream *data1 = strct.getData(256);
	ASSERT_NE(data1, static_cast<Common::SeekableReadStream *>(0));
	ASSERT_EQ(data1->size(), 1);

	EXPECT_EQ(data1->readByte(), 23);
	delete data1;

	Common::SeekableReadStream *data2 = strct.getData(9999);
	ASSERT_EQ(data2, static_cast<Common::SeekableReadStream *>(0));
}

// --- GFF4, list values ---

static const byte kGFF4ListValues[] = {
	0x47,0x46,0x46,0x20,0x56,0x34,0x2E,0x30,0x50,0x43,0x20,0x20,0x54,0x45,0x53,0x54,
	0x56,0x31,0x2E,0x30,0x01,0x00,0x00,0x00,0xF8,0x00,0x00,0x00,0x53,0x54,0x43,0x54,
	0x11,0x00,0x00,0x00,0x2C,0x00,0x00,0x00,0x92,0x02,0x00,0x00,0x00,0x01,0x00,0x00,
	0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x01,0x01,0x00,0x00,0x01,0x00,0x00,0x80,
	0x04,0x00,0x00,0x00,0x02,0x01,0x00,0x00,0x02,0x00,0x00,0x80,0x08,0x00,0x00,0x00,
	0x03,0x01,0x00,0x00,0x03,0x00,0x00,0x80,0x0C,0x00,0x00,0x00,0x04,0x01,0x00,0x00,
	0x04,0x00,0x00,0x80,0x10,0x00,0x00,0x00,0x05,0x01,0x00,0x00,0x05,0x00,0x00,0x80,
	0x14,0x00,0x00,0x00,0x06,0x01,0x00,0x00,0x06,0x00,0x00,0x80,0x18,0x00,0x00,0x00,
	0x07,0x01,0x00,0x00,0x07,0x00,0x00,0x80,0x1C,0x00,0x00,0x00,0x00,0x02,0x00,0x00,
	0x08,0x00,0x00,0x80,0x20,0x00,0x00,0x00,0x01,0x02,0x00,0x00,0x09,0x00,0x00,0x80,
	0x24,0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x0A,0x00,0x00,0x80,0x28,0x00,0x00,0x00,
	0x01,0x03,0x00,0x00,0x0C,0x00,0x00,0x80,0x2C,0x00,0x00,0x00,0x02,0x03,0x00,0x00,
	0x0D,0x00,0x00,0x80,0x30,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x0E,0x00,0x00,0x80,
	0x34,0x00,0x00,0x00,0x03,0x03,0x00,0x00,0x0F,0x00,0x00,0x80,0x38,0x00,0x00,0x00,
	0x04,0x03,0x00,0x00,0x10,0x00,0x00,0x80,0x3C,0x00,0x00,0x00,0x02,0x02,0x00,0x00,
	0x12,0x00,0x00,0x80,0x40,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x4B,0x00,0x00,0x00,
	0x52,0x00,0x00,0x00,0x5C,0x00,0x00,0x00,0x66,0x00,0x00,0x00,0x76,0x00,0x00,0x00,
	0x86,0x00,0x00,0x00,0xA2,0x00,0x00,0x00,0xBE,0x00,0x00,0x00,0xCE,0x00,0x00,0x00,
	0xEA,0x00,0x00,0x00,0x12,0x01,0x00,0x00,0x46,0x01,0x00,0x00,0x7A,0x01,0x00,0x00,
	0x8A,0x01,0x00,0x00,0xBE,0x01,0x00,0x00,0x82,0x02,0x00,0x00,0x03,0x00,0x00,0x00,
	0x17,0x18,0x19,0x03,0x00,0x00,0x00,0xE9,0xE8,0xE7,0x03,0x00,0x00,0x00,0x21,0x00,
	0x22,0x00,0x23,0x00,0x03,0x00,0x00,0x00,0xDF,0xFF,0xDE,0xFF,0xDD,0xFF,0x03,0x00,
	0x00,0x00,0x2B,0x00,0x00,0x00,0x2C,0x00,0x00,0x00,0x2D,0x00,0x00,0x00,0x03,0x00,
	0x00,0x00,0xD5,0xFF,0xFF,0xFF,0xD4,0xFF,0xFF,0xFF,0xD3,0xFF,0xFF,0xFF,0x03,0x00,
	0x00,0x00,0x35,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x36,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x37,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0xCB,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xCA,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xC9,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x03,0x00,0x00,0x00,0x66,0x66,0x74,0x42,0x66,0x66,
	0x78,0x42,0x66,0x66,0x7C,0x42,0x03,0x00,0x00,0x00,0x66,0x66,0x66,0x66,0x66,0xC6,
	0x51,0x40,0x66,0x66,0x66,0x66,0x66,0x06,0x52,0x40,0x66,0x66,0x66,0x66,0x66,0x46,
	0x52,0x40,0x03,0x00,0x00,0x00,0x33,0x33,0xA2,0x42,0x66,0x66,0xA2,0x42,0x9A,0x99,
	0xA2,0x42,0x33,0x33,0xA4,0x42,0x66,0x66,0xA4,0x42,0x9A,0x99,0xA4,0x42,0x33,0x33,
	0xA6,0x42,0x66,0x66,0xA6,0x42,0x9A,0x99,0xA6,0x42,0x03,0x00,0x00,0x00,0x33,0x33,
	0xB6,0x42,0x66,0x66,0xB6,0x42,0x9A,0x99,0xB6,0x42,0xCD,0xCC,0xB6,0x42,0x33,0x33,
	0xB8,0x42,0x66,0x66,0xB8,0x42,0x9A,0x99,0xB8,0x42,0xCD,0xCC,0xB8,0x42,0x33,0x33,
	0xBA,0x42,0x66,0x66,0xBA,0x42,0x9A,0x99,0xBA,0x42,0xCD,0xCC,0xBA,0x42,0x03,0x00,
	0x00,0x00,0x33,0x33,0xCA,0x42,0x66,0x66,0xCA,0x42,0x9A,0x99,0xCA,0x42,0xCD,0xCC,
	0xCA,0x42,0x33,0x33,0xCC,0x42,0x66,0x66,0xCC,0x42,0x9A,0x99,0xCC,0x42,0xCD,0xCC,
	0xCC,0x42,0x33,0x33,0xCE,0x42,0x66,0x66,0xCE,0x42,0x9A,0x99,0xCE,0x42,0xCD,0xCC,
	0xCE,0x42,0x03,0x00,0x00,0x00,0x92,0x02,0x00,0x00,0xA2,0x02,0x00,0x00,0xB2,0x02,
	0x00,0x00,0x03,0x00,0x00,0x00,0x33,0x33,0xDE,0x42,0x66,0x66,0xDE,0x42,0x9A,0x99,
	0xDE,0x42,0xCD,0xCC,0xDE,0x42,0x33,0x33,0xE0,0x42,0x66,0x66,0xE0,0x42,0x9A,0x99,
	0xE0,0x42,0xCD,0xCC,0xE0,0x42,0x33,0x33,0xE2,0x42,0x66,0x66,0xE2,0x42,0x9A,0x99,
	0xE2,0x42,0xCD,0xCC,0xE2,0x42,0x03,0x00,0x00,0x00,0x00,0x00,0xF0,0x42,0x33,0x33,
	0xF0,0x42,0x66,0x66,0xF0,0x42,0x9A,0x99,0xF0,0x42,0x00,0x00,0xF2,0x42,0x33,0x33,
	0xF2,0x42,0x66,0x66,0xF2,0x42,0x9A,0x99,0xF2,0x42,0x00,0x00,0xF4,0x42,0x33,0x33,
	0xF4,0x42,0x66,0x66,0xF4,0x42,0x9A,0x99,0xF4,0x42,0x00,0x00,0xF6,0x42,0x33,0x33,
	0xF6,0x42,0x66,0x66,0xF6,0x42,0x9A,0x99,0xF6,0x42,0x00,0x00,0x02,0x43,0x9A,0x19,
	0x02,0x43,0x33,0x33,0x02,0x43,0xCD,0x4C,0x02,0x43,0x00,0x00,0x03,0x43,0x9A,0x19,
	0x03,0x43,0x33,0x33,0x03,0x43,0xCD,0x4C,0x03,0x43,0x00,0x00,0x04,0x43,0x9A,0x19,
	0x04,0x43,0x33,0x33,0x04,0x43,0xCD,0x4C,0x04,0x43,0x00,0x00,0x05,0x43,0x9A,0x19,
	0x05,0x43,0x33,0x33,0x05,0x43,0xCD,0x4C,0x05,0x43,0x00,0x00,0x0C,0x43,0x9A,0x19,
	0x0C,0x43,0x33,0x33,0x0C,0x43,0xCD,0x4C,0x0C,0x43,0x00,0x00,0x0D,0x43,0x9A,0x19,
	0x0D,0x43,0x33,0x33,0x0D,0x43,0xCD,0x4C,0x0D,0x43,0x00,0x00,0x0E,0x43,0x9A,0x19,
	0x0E,0x43,0x33,0x33,0x0E,0x43,0xCD,0x4C,0x0E,0x43,0x00,0x00,0x0F,0x43,0x9A,0x19,
	0x0F,0x43,0x33,0x33,0x0F,0x43,0xCD,0x4C,0x0F,0x43,0x03,0x00,0x00,0x00,0xAE,0x53,
	0x00,0x00,0xAE,0x63,0x00,0x00,0xAE,0x73,0x00,0x00,0x06,0x00,0x00,0x00,0x42,0x00,
	0x61,0x00,0x72,0x00,0x30,0x00,0x30,0x00,0x31,0x00,0x06,0x00,0x00,0x00,0x42,0x00,
	0x61,0x00,0x72,0x00,0x30,0x00,0x30,0x00,0x32,0x00,0x06,0x00,0x00,0x00,0x42,0x00,
	0x61,0x00,0x72,0x00,0x30,0x00,0x30,0x00,0x33,0x00
};

static const uint32_t kFieldLabelsList[] = {
	256, 257, 258, 259, 260, 261, 262, 263, 512, 513, 514, 768, 769, 770, 771, 772, 1024
};

static const Aurora::GFF4Struct::FieldType kFieldTypesList[] = {
	Aurora::GFF4Struct::kFieldTypeUint8,
	Aurora::GFF4Struct::kFieldTypeSint8,
	Aurora::GFF4Struct::kFieldTypeUint16,
	Aurora::GFF4Struct::kFieldTypeSint16,
	Aurora::GFF4Struct::kFieldTypeUint32,
	Aurora::GFF4Struct::kFieldTypeSint32,
	Aurora::GFF4Struct::kFieldTypeUint64,
	Aurora::GFF4Struct::kFieldTypeSint64,
	Aurora::GFF4Struct::kFieldTypeFloat32,
	Aurora::GFF4Struct::kFieldTypeFloat64,
	Aurora::GFF4Struct::kFieldTypeNDSFixed,
	Aurora::GFF4Struct::kFieldTypeVector3f,
	Aurora::GFF4Struct::kFieldTypeVector4f,
	Aurora::GFF4Struct::kFieldTypeQuaternionf,
	Aurora::GFF4Struct::kFieldTypeColor4f,
	Aurora::GFF4Struct::kFieldTypeMatrix4x4f,
	Aurora::GFF4Struct::kFieldTypeString,
};

GTEST_TEST(GFF4StructList, getFieldCount) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4ListValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	EXPECT_EQ(strct.getFieldCount(), ARRAYSIZE(kFieldLabelsList));
}

GTEST_TEST(GFF4StructList, hasField) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4ListValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	for (size_t i = 0; i < ARRAYSIZE(kFieldLabelsList); i++)
		EXPECT_TRUE(strct.hasField(kFieldLabelsList[i])) << "At index " << i;

	EXPECT_FALSE(strct.hasField(9999));
}

GTEST_TEST(GFF4StructList, getFieldLabels) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4ListValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	std::vector<uint32_t> labels = strct.getFieldLabels();
	std::sort(labels.begin(), labels.end());

	for (size_t i = 0; i < ARRAYSIZE(kFieldLabelsList); i++)
		EXPECT_EQ(labels[i], kFieldLabelsList[i]) << "At index " << i;
}

GTEST_TEST(GFF4StructList, getFieldType) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4ListValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	for (size_t i = 0; i < ARRAYSIZE(kFieldLabelsList); i++)
		EXPECT_EQ(strct.getFieldType(kFieldLabelsList[i]), kFieldTypesList[i]) << "At index " << i;

	EXPECT_EQ(strct.getFieldType(9999), Aurora::GFF4Struct::kFieldTypeNone);
}

GTEST_TEST(GFF4StructList, getFieldTypeList) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4ListValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	for (size_t i = 0; i < ARRAYSIZE(kFieldLabelsList); i++) {
		bool isList;

		EXPECT_EQ(strct.getFieldType(kFieldLabelsList[i], isList), kFieldTypesList[i]) << "At index " << i;
		EXPECT_TRUE(isList);
	}

	bool isList;
	EXPECT_EQ(strct.getFieldType(9999, isList), Aurora::GFF4Struct::kFieldTypeNone);
}

GTEST_TEST(GFF4StructList, getFieldProperties) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4ListValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	for (size_t i = 0; i < ARRAYSIZE(kFieldLabelsList); i++) {
		Aurora::GFF4Struct::FieldType type;
		uint32_t label;
		bool isList;

		EXPECT_TRUE(strct.getFieldProperties(kFieldLabelsList[i], type, label, isList)) << "At index " << i;
		EXPECT_EQ(type, kFieldTypesList[i]);
		EXPECT_EQ(label, kFieldLabelsList[i]);
		EXPECT_TRUE(isList);
	}

	Aurora::GFF4Struct::FieldType type;
	uint32_t label;
	bool isList;

	EXPECT_FALSE(strct.getFieldProperties(9999, type, label, isList));
}

GTEST_TEST(GFF4StructList, getUint) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4ListValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	EXPECT_THROW(strct.getUint(256), Common::Exception);
	EXPECT_THROW(strct.getUint(257), Common::Exception);
	EXPECT_THROW(strct.getUint(258), Common::Exception);
	EXPECT_THROW(strct.getUint(259), Common::Exception);
	EXPECT_THROW(strct.getUint(260), Common::Exception);
	EXPECT_THROW(strct.getUint(261), Common::Exception);
	EXPECT_THROW(strct.getUint(262), Common::Exception);
	EXPECT_THROW(strct.getUint(263), Common::Exception);

	std::vector<uint64_t> list;

	EXPECT_TRUE(strct.getUint(256, list));
	ASSERT_EQ(list.size(), 3);
	EXPECT_EQ(list[0], 23);
	EXPECT_EQ(list[1], 24);
	EXPECT_EQ(list[2], 25);

	EXPECT_TRUE(strct.getUint(258, list));
	ASSERT_EQ(list.size(), 3);
	EXPECT_EQ(list[0], 33);
	EXPECT_EQ(list[1], 34);
	EXPECT_EQ(list[2], 35);

	EXPECT_TRUE(strct.getUint(260, list));
	ASSERT_EQ(list.size(), 3);
	EXPECT_EQ(list[0], 43);
	EXPECT_EQ(list[1], 44);
	EXPECT_EQ(list[2], 45);

	EXPECT_TRUE(strct.getUint(262, list));
	ASSERT_EQ(list.size(), 3);
	EXPECT_EQ(list[0], 53);
	EXPECT_EQ(list[1], 54);
	EXPECT_EQ(list[2], 55);

	EXPECT_TRUE(strct.getUint(257, list));
	ASSERT_EQ(list.size(), 3);
	EXPECT_EQ(list[0], (uint64_t)((int64_t) -23));
	EXPECT_EQ(list[1], (uint64_t)((int64_t) -24));
	EXPECT_EQ(list[2], (uint64_t)((int64_t) -25));

	EXPECT_TRUE(strct.getUint(259, list));
	ASSERT_EQ(list.size(), 3);
	EXPECT_EQ(list[0], (uint64_t)((int64_t) -33));
	EXPECT_EQ(list[1], (uint64_t)((int64_t) -34));
	EXPECT_EQ(list[2], (uint64_t)((int64_t) -35));

	EXPECT_TRUE(strct.getUint(261, list));
	ASSERT_EQ(list.size(), 3);
	EXPECT_EQ(list[0], (uint64_t)((int64_t) -43));
	EXPECT_EQ(list[1], (uint64_t)((int64_t) -44));
	EXPECT_EQ(list[2], (uint64_t)((int64_t) -45));

	EXPECT_TRUE(strct.getUint(263, list));
	ASSERT_EQ(list.size(), 3);
	EXPECT_EQ(list[0], (uint64_t)((int64_t) -53));
	EXPECT_EQ(list[1], (uint64_t)((int64_t) -54));
	EXPECT_EQ(list[2], (uint64_t)((int64_t) -55));

	EXPECT_FALSE(strct.getUint(9999, list));

	EXPECT_THROW(strct.getUint(1024, list), Common::Exception);
}

GTEST_TEST(GFF4StructList, getSint) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4ListValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	EXPECT_THROW(strct.getSint(256), Common::Exception);
	EXPECT_THROW(strct.getSint(257), Common::Exception);
	EXPECT_THROW(strct.getSint(258), Common::Exception);
	EXPECT_THROW(strct.getSint(259), Common::Exception);
	EXPECT_THROW(strct.getSint(260), Common::Exception);
	EXPECT_THROW(strct.getSint(261), Common::Exception);
	EXPECT_THROW(strct.getSint(262), Common::Exception);
	EXPECT_THROW(strct.getSint(263), Common::Exception);

	std::vector<int64_t> list;

	EXPECT_TRUE(strct.getSint(256, list));
	ASSERT_EQ(list.size(), 3);
	EXPECT_EQ(list[0], 23);
	EXPECT_EQ(list[1], 24);
	EXPECT_EQ(list[2], 25);

	EXPECT_TRUE(strct.getSint(258, list));
	ASSERT_EQ(list.size(), 3);
	EXPECT_EQ(list[0], 33);
	EXPECT_EQ(list[1], 34);
	EXPECT_EQ(list[2], 35);

	EXPECT_TRUE(strct.getSint(260, list));
	ASSERT_EQ(list.size(), 3);
	EXPECT_EQ(list[0], 43);
	EXPECT_EQ(list[1], 44);
	EXPECT_EQ(list[2], 45);

	EXPECT_TRUE(strct.getSint(262, list));
	ASSERT_EQ(list.size(), 3);
	EXPECT_EQ(list[0], 53);
	EXPECT_EQ(list[1], 54);
	EXPECT_EQ(list[2], 55);

	EXPECT_TRUE(strct.getSint(257, list));
	ASSERT_EQ(list.size(), 3);
	EXPECT_EQ(list[0], -23);
	EXPECT_EQ(list[1], -24);
	EXPECT_EQ(list[2], -25);

	EXPECT_TRUE(strct.getSint(259, list));
	ASSERT_EQ(list.size(), 3);
	EXPECT_EQ(list[0], -33);
	EXPECT_EQ(list[1], -34);
	EXPECT_EQ(list[2], -35);

	EXPECT_TRUE(strct.getSint(261, list));
	ASSERT_EQ(list.size(), 3);
	EXPECT_EQ(list[0], -43);
	EXPECT_EQ(list[1], -44);
	EXPECT_EQ(list[2], -45);

	EXPECT_TRUE(strct.getSint(263, list));
	ASSERT_EQ(list.size(), 3);
	EXPECT_EQ(list[0], -53);
	EXPECT_EQ(list[1], -54);
	EXPECT_EQ(list[2], -55);

	EXPECT_FALSE(strct.getSint(9999, list));

	EXPECT_THROW(strct.getSint(1024, list), Common::Exception);
}

GTEST_TEST(GFF4StructList, getBool) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4ListValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	EXPECT_THROW(strct.getBool(256), Common::Exception);
	EXPECT_THROW(strct.getBool(257), Common::Exception);
	EXPECT_THROW(strct.getBool(258), Common::Exception);
	EXPECT_THROW(strct.getBool(259), Common::Exception);
	EXPECT_THROW(strct.getBool(260), Common::Exception);
	EXPECT_THROW(strct.getBool(261), Common::Exception);
	EXPECT_THROW(strct.getBool(262), Common::Exception);
	EXPECT_THROW(strct.getBool(263), Common::Exception);

	std::vector<bool> list;

	EXPECT_TRUE(strct.getBool(256, list));
	ASSERT_EQ(list.size(), 3);
	EXPECT_EQ(list[0], true);
	EXPECT_EQ(list[1], true);
	EXPECT_EQ(list[2], true);

	EXPECT_TRUE(strct.getBool(258, list));
	ASSERT_EQ(list.size(), 3);
	EXPECT_EQ(list[0], true);
	EXPECT_EQ(list[1], true);
	EXPECT_EQ(list[2], true);

	EXPECT_TRUE(strct.getBool(260, list));
	ASSERT_EQ(list.size(), 3);
	EXPECT_EQ(list[0], true);
	EXPECT_EQ(list[1], true);
	EXPECT_EQ(list[2], true);

	EXPECT_TRUE(strct.getBool(262, list));
	ASSERT_EQ(list.size(), 3);
	EXPECT_EQ(list[0], true);
	EXPECT_EQ(list[1], true);
	EXPECT_EQ(list[2], true);

	EXPECT_TRUE(strct.getBool(257, list));
	ASSERT_EQ(list.size(), 3);
	EXPECT_EQ(list[0], true);
	EXPECT_EQ(list[1], true);
	EXPECT_EQ(list[2], true);

	EXPECT_TRUE(strct.getBool(259, list));
	ASSERT_EQ(list.size(), 3);
	EXPECT_EQ(list[0], true);
	EXPECT_EQ(list[1], true);
	EXPECT_EQ(list[2], true);

	EXPECT_TRUE(strct.getBool(261, list));
	ASSERT_EQ(list.size(), 3);
	EXPECT_EQ(list[0], true);
	EXPECT_EQ(list[1], true);
	EXPECT_EQ(list[2], true);

	EXPECT_TRUE(strct.getBool(263, list));
	ASSERT_EQ(list.size(), 3);
	EXPECT_EQ(list[0], true);
	EXPECT_EQ(list[1], true);
	EXPECT_EQ(list[2], true);

	EXPECT_FALSE(strct.getBool(9999, list));

	EXPECT_THROW(strct.getBool(1024, list), Common::Exception);
}

GTEST_TEST(GFF4StructList, getDouble) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4ListValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	std::vector<double> list;

	EXPECT_TRUE(strct.getDouble(512, list));
	ASSERT_EQ(list.size(), 3);
	EXPECT_DOUBLE_EQ(list[0], 61.1f);
	EXPECT_DOUBLE_EQ(list[1], 62.1f);
	EXPECT_DOUBLE_EQ(list[2], 63.1f);

	EXPECT_TRUE(strct.getDouble(513, list));
	ASSERT_EQ(list.size(), 3);
	EXPECT_DOUBLE_EQ(list[0], 71.1);
	EXPECT_DOUBLE_EQ(list[1], 72.1);
	EXPECT_DOUBLE_EQ(list[2], 73.1);

	// Nintendo fixed point
	EXPECT_TRUE(strct.getDouble(514, list));
	ASSERT_EQ(list.size(), 3);
	EXPECT_NEAR(list[0], 5.23, 0.00005);
	EXPECT_NEAR(list[1], 6.23, 0.00005);
	EXPECT_NEAR(list[2], 7.23, 0.00005);

	EXPECT_FALSE(strct.getDouble(9999, list));

	EXPECT_THROW(strct.getDouble(1024, list), Common::Exception);
}

GTEST_TEST(GFF4StructList, getFloat) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4ListValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	std::vector<float> list;

	EXPECT_TRUE(strct.getFloat(512, list));
	ASSERT_EQ(list.size(), 3);
	EXPECT_FLOAT_EQ(list[0], 61.1f);
	EXPECT_FLOAT_EQ(list[1], 62.1f);
	EXPECT_FLOAT_EQ(list[2], 63.1f);

	EXPECT_TRUE(strct.getFloat(513, list));
	ASSERT_EQ(list.size(), 3);
	EXPECT_FLOAT_EQ(list[0], 71.1f);
	EXPECT_FLOAT_EQ(list[1], 72.1f);
	EXPECT_FLOAT_EQ(list[2], 73.1f);

	// Nintendo fixed point
	EXPECT_TRUE(strct.getFloat(514, list));
	ASSERT_EQ(list.size(), 3);
	EXPECT_NEAR(list[0], 5.23f, 0.00005);
	EXPECT_NEAR(list[1], 6.23f, 0.00005);
	EXPECT_NEAR(list[2], 7.23f, 0.00005);

	EXPECT_FALSE(strct.getFloat(9999, list));

	EXPECT_THROW(strct.getFloat(1024, list), Common::Exception);
}

GTEST_TEST(GFF4StructList, getString) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4ListValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	std::vector<Common::UString> list;

	EXPECT_TRUE(strct.getString(1024, list));
	ASSERT_EQ(list.size(), 3);
	EXPECT_STREQ(list[0].c_str(), "Bar001");
	EXPECT_STREQ(list[1].c_str(), "Bar002");
	EXPECT_STREQ(list[2].c_str(), "Bar003");

	EXPECT_FALSE(strct.getString(9999, list));

	EXPECT_THROW(strct.getString(512, list), Common::Exception);
}

GTEST_TEST(GFF4StructList, getStringEncoding) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4ListValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	std::vector<Common::UString> list;

	EXPECT_TRUE(strct.getString(1024, Common::kEncodingUTF16LE, list));
	ASSERT_EQ(list.size(), 3);
	EXPECT_STREQ(list[0].c_str(), "Bar001");
	EXPECT_STREQ(list[1].c_str(), "Bar002");
	EXPECT_STREQ(list[2].c_str(), "Bar003");

	EXPECT_FALSE(strct.getString(9999, Common::kEncodingUTF16LE, list));

	EXPECT_THROW(strct.getString(512, Common::kEncodingUTF16LE, list), Common::Exception);
}

GTEST_TEST(GFF4StructList, getMatrix4x4Matrix) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4ListValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	std::vector<glm::mat4> list;

	EXPECT_TRUE(strct.getMatrix4x4(772, list));
	ASSERT_EQ(list.size(), 3);

	EXPECT_FLOAT_EQ(list[0][0][0], 120.0f);
	EXPECT_FLOAT_EQ(list[0][0][1], 120.1f);
	EXPECT_FLOAT_EQ(list[0][0][2], 120.2f);
	EXPECT_FLOAT_EQ(list[0][0][3], 120.3f);
	EXPECT_FLOAT_EQ(list[0][1][0], 121.0f);
	EXPECT_FLOAT_EQ(list[0][1][1], 121.1f);
	EXPECT_FLOAT_EQ(list[0][1][2], 121.2f);
	EXPECT_FLOAT_EQ(list[0][1][3], 121.3f);
	EXPECT_FLOAT_EQ(list[0][2][0], 122.0f);
	EXPECT_FLOAT_EQ(list[0][2][1], 122.1f);
	EXPECT_FLOAT_EQ(list[0][2][2], 122.2f);
	EXPECT_FLOAT_EQ(list[0][2][3], 122.3f);
	EXPECT_FLOAT_EQ(list[0][3][0], 123.0f);
	EXPECT_FLOAT_EQ(list[0][3][1], 123.1f);
	EXPECT_FLOAT_EQ(list[0][3][2], 123.2f);
	EXPECT_FLOAT_EQ(list[0][3][3], 123.3f);
	EXPECT_FLOAT_EQ(list[1][0][0], 130.0f);
	EXPECT_FLOAT_EQ(list[1][0][1], 130.1f);
	EXPECT_FLOAT_EQ(list[1][0][2], 130.2f);
	EXPECT_FLOAT_EQ(list[1][0][3], 130.3f);
	EXPECT_FLOAT_EQ(list[1][1][0], 131.0f);
	EXPECT_FLOAT_EQ(list[1][1][1], 131.1f);
	EXPECT_FLOAT_EQ(list[1][1][2], 131.2f);
	EXPECT_FLOAT_EQ(list[1][1][3], 131.3f);
	EXPECT_FLOAT_EQ(list[1][2][0], 132.0f);
	EXPECT_FLOAT_EQ(list[1][2][1], 132.1f);
	EXPECT_FLOAT_EQ(list[1][2][2], 132.2f);
	EXPECT_FLOAT_EQ(list[1][2][3], 132.3f);
	EXPECT_FLOAT_EQ(list[1][3][0], 133.0f);
	EXPECT_FLOAT_EQ(list[1][3][1], 133.1f);
	EXPECT_FLOAT_EQ(list[1][3][2], 133.2f);
	EXPECT_FLOAT_EQ(list[1][3][3], 133.3f);
	EXPECT_FLOAT_EQ(list[2][0][0], 140.0f);
	EXPECT_FLOAT_EQ(list[2][0][1], 140.1f);
	EXPECT_FLOAT_EQ(list[2][0][2], 140.2f);
	EXPECT_FLOAT_EQ(list[2][0][3], 140.3f);
	EXPECT_FLOAT_EQ(list[2][1][0], 141.0f);
	EXPECT_FLOAT_EQ(list[2][1][1], 141.1f);
	EXPECT_FLOAT_EQ(list[2][1][2], 141.2f);
	EXPECT_FLOAT_EQ(list[2][1][3], 141.3f);
	EXPECT_FLOAT_EQ(list[2][2][0], 142.0f);
	EXPECT_FLOAT_EQ(list[2][2][1], 142.1f);
	EXPECT_FLOAT_EQ(list[2][2][2], 142.2f);
	EXPECT_FLOAT_EQ(list[2][2][3], 142.3f);
	EXPECT_FLOAT_EQ(list[2][3][0], 143.0f);
	EXPECT_FLOAT_EQ(list[2][3][1], 143.1f);
	EXPECT_FLOAT_EQ(list[2][3][2], 143.2f);
	EXPECT_FLOAT_EQ(list[2][3][3], 143.3f);

	EXPECT_FALSE(strct.getMatrix4x4(9999, list));

	EXPECT_THROW(strct.getMatrix4x4(1024, list), Common::Exception);
}

GTEST_TEST(GFF4StructList, getVectorMatrixDouble) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4ListValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	std::vector< std::vector<double> > v;

	EXPECT_TRUE(strct.getVectorMatrix(768, v));
	ASSERT_EQ(v.size(), 3);
	ASSERT_EQ(v[0].size(), 3);
	ASSERT_EQ(v[1].size(), 3);
	ASSERT_EQ(v[2].size(), 3);
	EXPECT_DOUBLE_EQ(v[0][0], 81.1f);
	EXPECT_DOUBLE_EQ(v[0][1], 81.2f);
	EXPECT_DOUBLE_EQ(v[0][2], 81.3f);
	EXPECT_DOUBLE_EQ(v[1][0], 82.1f);
	EXPECT_DOUBLE_EQ(v[1][1], 82.2f);
	EXPECT_DOUBLE_EQ(v[1][2], 82.3f);
	EXPECT_DOUBLE_EQ(v[2][0], 83.1f);
	EXPECT_DOUBLE_EQ(v[2][1], 83.2f);
	EXPECT_DOUBLE_EQ(v[2][2], 83.3f);

	EXPECT_TRUE(strct.getVectorMatrix(769, v));
	ASSERT_EQ(v.size(), 3);
	ASSERT_EQ(v[0].size(), 4);
	ASSERT_EQ(v[1].size(), 4);
	ASSERT_EQ(v[2].size(), 4);
	EXPECT_DOUBLE_EQ(v[0][0], 91.1f);
	EXPECT_DOUBLE_EQ(v[0][1], 91.2f);
	EXPECT_DOUBLE_EQ(v[0][2], 91.3f);
	EXPECT_DOUBLE_EQ(v[0][3], 91.4f);
	EXPECT_DOUBLE_EQ(v[1][0], 92.1f);
	EXPECT_DOUBLE_EQ(v[1][1], 92.2f);
	EXPECT_DOUBLE_EQ(v[1][2], 92.3f);
	EXPECT_DOUBLE_EQ(v[1][3], 92.4f);
	EXPECT_DOUBLE_EQ(v[2][0], 93.1f);
	EXPECT_DOUBLE_EQ(v[2][1], 93.2f);
	EXPECT_DOUBLE_EQ(v[2][2], 93.3f);
	EXPECT_DOUBLE_EQ(v[2][3], 93.4f);

	EXPECT_TRUE(strct.getVectorMatrix(770, v));
	ASSERT_EQ(v.size(), 3);
	ASSERT_EQ(v[0].size(), 4);
	ASSERT_EQ(v[1].size(), 4);
	ASSERT_EQ(v[2].size(), 4);
	EXPECT_DOUBLE_EQ(v[0][0], 101.1f);
	EXPECT_DOUBLE_EQ(v[0][1], 101.2f);
	EXPECT_DOUBLE_EQ(v[0][2], 101.3f);
	EXPECT_DOUBLE_EQ(v[0][3], 101.4f);
	EXPECT_DOUBLE_EQ(v[1][0], 102.1f);
	EXPECT_DOUBLE_EQ(v[1][1], 102.2f);
	EXPECT_DOUBLE_EQ(v[1][2], 102.3f);
	EXPECT_DOUBLE_EQ(v[1][3], 102.4f);
	EXPECT_DOUBLE_EQ(v[2][0], 103.1f);
	EXPECT_DOUBLE_EQ(v[2][1], 103.2f);
	EXPECT_DOUBLE_EQ(v[2][2], 103.3f);
	EXPECT_DOUBLE_EQ(v[2][3], 103.4f);

	EXPECT_TRUE(strct.getVectorMatrix(771, v));
	ASSERT_EQ(v.size(), 3);
	ASSERT_EQ(v[0].size(), 4);
	ASSERT_EQ(v[1].size(), 4);
	ASSERT_EQ(v[2].size(), 4);
	EXPECT_DOUBLE_EQ(v[0][0], 111.1f);
	EXPECT_DOUBLE_EQ(v[0][1], 111.2f);
	EXPECT_DOUBLE_EQ(v[0][2], 111.3f);
	EXPECT_DOUBLE_EQ(v[0][3], 111.4f);
	EXPECT_DOUBLE_EQ(v[1][0], 112.1f);
	EXPECT_DOUBLE_EQ(v[1][1], 112.2f);
	EXPECT_DOUBLE_EQ(v[1][2], 112.3f);
	EXPECT_DOUBLE_EQ(v[1][3], 112.4f);
	EXPECT_DOUBLE_EQ(v[2][0], 113.1f);
	EXPECT_DOUBLE_EQ(v[2][1], 113.2f);
	EXPECT_DOUBLE_EQ(v[2][2], 113.3f);
	EXPECT_DOUBLE_EQ(v[2][3], 113.4f);

	EXPECT_TRUE(strct.getVectorMatrix(772, v));
	ASSERT_EQ(v.size(), 3);
	ASSERT_EQ(v[0].size(), 16);
	ASSERT_EQ(v[1].size(), 16);
	ASSERT_EQ(v[2].size(), 16);
	EXPECT_DOUBLE_EQ(v[0][ 0], 120.0f);
	EXPECT_DOUBLE_EQ(v[0][ 1], 120.1f);
	EXPECT_DOUBLE_EQ(v[0][ 2], 120.2f);
	EXPECT_DOUBLE_EQ(v[0][ 3], 120.3f);
	EXPECT_DOUBLE_EQ(v[0][ 4], 121.0f);
	EXPECT_DOUBLE_EQ(v[0][ 5], 121.1f);
	EXPECT_DOUBLE_EQ(v[0][ 6], 121.2f);
	EXPECT_DOUBLE_EQ(v[0][ 7], 121.3f);
	EXPECT_DOUBLE_EQ(v[0][ 8], 122.0f);
	EXPECT_DOUBLE_EQ(v[0][ 9], 122.1f);
	EXPECT_DOUBLE_EQ(v[0][10], 122.2f);
	EXPECT_DOUBLE_EQ(v[0][11], 122.3f);
	EXPECT_DOUBLE_EQ(v[0][12], 123.0f);
	EXPECT_DOUBLE_EQ(v[0][13], 123.1f);
	EXPECT_DOUBLE_EQ(v[0][14], 123.2f);
	EXPECT_DOUBLE_EQ(v[0][15], 123.3f);
	EXPECT_DOUBLE_EQ(v[1][ 0], 130.0f);
	EXPECT_DOUBLE_EQ(v[1][ 1], 130.1f);
	EXPECT_DOUBLE_EQ(v[1][ 2], 130.2f);
	EXPECT_DOUBLE_EQ(v[1][ 3], 130.3f);
	EXPECT_DOUBLE_EQ(v[1][ 4], 131.0f);
	EXPECT_DOUBLE_EQ(v[1][ 5], 131.1f);
	EXPECT_DOUBLE_EQ(v[1][ 6], 131.2f);
	EXPECT_DOUBLE_EQ(v[1][ 7], 131.3f);
	EXPECT_DOUBLE_EQ(v[1][ 8], 132.0f);
	EXPECT_DOUBLE_EQ(v[1][ 9], 132.1f);
	EXPECT_DOUBLE_EQ(v[1][10], 132.2f);
	EXPECT_DOUBLE_EQ(v[1][11], 132.3f);
	EXPECT_DOUBLE_EQ(v[1][12], 133.0f);
	EXPECT_DOUBLE_EQ(v[1][13], 133.1f);
	EXPECT_DOUBLE_EQ(v[1][14], 133.2f);
	EXPECT_DOUBLE_EQ(v[1][15], 133.3f);
	EXPECT_DOUBLE_EQ(v[2][ 0], 140.0f);
	EXPECT_DOUBLE_EQ(v[2][ 1], 140.1f);
	EXPECT_DOUBLE_EQ(v[2][ 2], 140.2f);
	EXPECT_DOUBLE_EQ(v[2][ 3], 140.3f);
	EXPECT_DOUBLE_EQ(v[2][ 4], 141.0f);
	EXPECT_DOUBLE_EQ(v[2][ 5], 141.1f);
	EXPECT_DOUBLE_EQ(v[2][ 6], 141.2f);
	EXPECT_DOUBLE_EQ(v[2][ 7], 141.3f);
	EXPECT_DOUBLE_EQ(v[2][ 8], 142.0f);
	EXPECT_DOUBLE_EQ(v[2][ 9], 142.1f);
	EXPECT_DOUBLE_EQ(v[2][10], 142.2f);
	EXPECT_DOUBLE_EQ(v[2][11], 142.3f);
	EXPECT_DOUBLE_EQ(v[2][12], 143.0f);
	EXPECT_DOUBLE_EQ(v[2][13], 143.1f);
	EXPECT_DOUBLE_EQ(v[2][14], 143.2f);
	EXPECT_DOUBLE_EQ(v[2][15], 143.3f);

	EXPECT_FALSE(strct.getVectorMatrix(9999, v));

	EXPECT_THROW(strct.getVectorMatrix(1024, v), Common::Exception);
}

GTEST_TEST(GFF4StructList, getVectorMatrixFloat) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4ListValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	std::vector< std::vector<float> > v;

	EXPECT_TRUE(strct.getVectorMatrix(768, v));
	ASSERT_EQ(v.size(), 3);
	ASSERT_EQ(v[0].size(), 3);
	ASSERT_EQ(v[1].size(), 3);
	ASSERT_EQ(v[2].size(), 3);
	EXPECT_FLOAT_EQ(v[0][0], 81.1f);
	EXPECT_FLOAT_EQ(v[0][1], 81.2f);
	EXPECT_FLOAT_EQ(v[0][2], 81.3f);
	EXPECT_FLOAT_EQ(v[1][0], 82.1f);
	EXPECT_FLOAT_EQ(v[1][1], 82.2f);
	EXPECT_FLOAT_EQ(v[1][2], 82.3f);
	EXPECT_FLOAT_EQ(v[2][0], 83.1f);
	EXPECT_FLOAT_EQ(v[2][1], 83.2f);
	EXPECT_FLOAT_EQ(v[2][2], 83.3f);

	EXPECT_TRUE(strct.getVectorMatrix(769, v));
	ASSERT_EQ(v.size(), 3);
	ASSERT_EQ(v[0].size(), 4);
	ASSERT_EQ(v[1].size(), 4);
	ASSERT_EQ(v[2].size(), 4);
	EXPECT_FLOAT_EQ(v[0][0], 91.1f);
	EXPECT_FLOAT_EQ(v[0][1], 91.2f);
	EXPECT_FLOAT_EQ(v[0][2], 91.3f);
	EXPECT_FLOAT_EQ(v[0][3], 91.4f);
	EXPECT_FLOAT_EQ(v[1][0], 92.1f);
	EXPECT_FLOAT_EQ(v[1][1], 92.2f);
	EXPECT_FLOAT_EQ(v[1][2], 92.3f);
	EXPECT_FLOAT_EQ(v[1][3], 92.4f);
	EXPECT_FLOAT_EQ(v[2][0], 93.1f);
	EXPECT_FLOAT_EQ(v[2][1], 93.2f);
	EXPECT_FLOAT_EQ(v[2][2], 93.3f);
	EXPECT_FLOAT_EQ(v[2][3], 93.4f);

	EXPECT_TRUE(strct.getVectorMatrix(770, v));
	ASSERT_EQ(v.size(), 3);
	ASSERT_EQ(v[0].size(), 4);
	ASSERT_EQ(v[1].size(), 4);
	ASSERT_EQ(v[2].size(), 4);
	EXPECT_FLOAT_EQ(v[0][0], 101.1f);
	EXPECT_FLOAT_EQ(v[0][1], 101.2f);
	EXPECT_FLOAT_EQ(v[0][2], 101.3f);
	EXPECT_FLOAT_EQ(v[0][3], 101.4f);
	EXPECT_FLOAT_EQ(v[1][0], 102.1f);
	EXPECT_FLOAT_EQ(v[1][1], 102.2f);
	EXPECT_FLOAT_EQ(v[1][2], 102.3f);
	EXPECT_FLOAT_EQ(v[1][3], 102.4f);
	EXPECT_FLOAT_EQ(v[2][0], 103.1f);
	EXPECT_FLOAT_EQ(v[2][1], 103.2f);
	EXPECT_FLOAT_EQ(v[2][2], 103.3f);
	EXPECT_FLOAT_EQ(v[2][3], 103.4f);

	EXPECT_TRUE(strct.getVectorMatrix(771, v));
	ASSERT_EQ(v.size(), 3);
	ASSERT_EQ(v[0].size(), 4);
	ASSERT_EQ(v[1].size(), 4);
	ASSERT_EQ(v[2].size(), 4);
	EXPECT_FLOAT_EQ(v[0][0], 111.1f);
	EXPECT_FLOAT_EQ(v[0][1], 111.2f);
	EXPECT_FLOAT_EQ(v[0][2], 111.3f);
	EXPECT_FLOAT_EQ(v[0][3], 111.4f);
	EXPECT_FLOAT_EQ(v[1][0], 112.1f);
	EXPECT_FLOAT_EQ(v[1][1], 112.2f);
	EXPECT_FLOAT_EQ(v[1][2], 112.3f);
	EXPECT_FLOAT_EQ(v[1][3], 112.4f);
	EXPECT_FLOAT_EQ(v[2][0], 113.1f);
	EXPECT_FLOAT_EQ(v[2][1], 113.2f);
	EXPECT_FLOAT_EQ(v[2][2], 113.3f);
	EXPECT_FLOAT_EQ(v[2][3], 113.4f);

	EXPECT_TRUE(strct.getVectorMatrix(772, v));
	ASSERT_EQ(v.size(), 3);
	ASSERT_EQ(v[0].size(), 16);
	ASSERT_EQ(v[1].size(), 16);
	ASSERT_EQ(v[2].size(), 16);
	EXPECT_FLOAT_EQ(v[0][ 0], 120.0f);
	EXPECT_FLOAT_EQ(v[0][ 1], 120.1f);
	EXPECT_FLOAT_EQ(v[0][ 2], 120.2f);
	EXPECT_FLOAT_EQ(v[0][ 3], 120.3f);
	EXPECT_FLOAT_EQ(v[0][ 4], 121.0f);
	EXPECT_FLOAT_EQ(v[0][ 5], 121.1f);
	EXPECT_FLOAT_EQ(v[0][ 6], 121.2f);
	EXPECT_FLOAT_EQ(v[0][ 7], 121.3f);
	EXPECT_FLOAT_EQ(v[0][ 8], 122.0f);
	EXPECT_FLOAT_EQ(v[0][ 9], 122.1f);
	EXPECT_FLOAT_EQ(v[0][10], 122.2f);
	EXPECT_FLOAT_EQ(v[0][11], 122.3f);
	EXPECT_FLOAT_EQ(v[0][12], 123.0f);
	EXPECT_FLOAT_EQ(v[0][13], 123.1f);
	EXPECT_FLOAT_EQ(v[0][14], 123.2f);
	EXPECT_FLOAT_EQ(v[0][15], 123.3f);
	EXPECT_FLOAT_EQ(v[1][ 0], 130.0f);
	EXPECT_FLOAT_EQ(v[1][ 1], 130.1f);
	EXPECT_FLOAT_EQ(v[1][ 2], 130.2f);
	EXPECT_FLOAT_EQ(v[1][ 3], 130.3f);
	EXPECT_FLOAT_EQ(v[1][ 4], 131.0f);
	EXPECT_FLOAT_EQ(v[1][ 5], 131.1f);
	EXPECT_FLOAT_EQ(v[1][ 6], 131.2f);
	EXPECT_FLOAT_EQ(v[1][ 7], 131.3f);
	EXPECT_FLOAT_EQ(v[1][ 8], 132.0f);
	EXPECT_FLOAT_EQ(v[1][ 9], 132.1f);
	EXPECT_FLOAT_EQ(v[1][10], 132.2f);
	EXPECT_FLOAT_EQ(v[1][11], 132.3f);
	EXPECT_FLOAT_EQ(v[1][12], 133.0f);
	EXPECT_FLOAT_EQ(v[1][13], 133.1f);
	EXPECT_FLOAT_EQ(v[1][14], 133.2f);
	EXPECT_FLOAT_EQ(v[1][15], 133.3f);
	EXPECT_FLOAT_EQ(v[2][ 0], 140.0f);
	EXPECT_FLOAT_EQ(v[2][ 1], 140.1f);
	EXPECT_FLOAT_EQ(v[2][ 2], 140.2f);
	EXPECT_FLOAT_EQ(v[2][ 3], 140.3f);
	EXPECT_FLOAT_EQ(v[2][ 4], 141.0f);
	EXPECT_FLOAT_EQ(v[2][ 5], 141.1f);
	EXPECT_FLOAT_EQ(v[2][ 6], 141.2f);
	EXPECT_FLOAT_EQ(v[2][ 7], 141.3f);
	EXPECT_FLOAT_EQ(v[2][ 8], 142.0f);
	EXPECT_FLOAT_EQ(v[2][ 9], 142.1f);
	EXPECT_FLOAT_EQ(v[2][10], 142.2f);
	EXPECT_FLOAT_EQ(v[2][11], 142.3f);
	EXPECT_FLOAT_EQ(v[2][12], 143.0f);
	EXPECT_FLOAT_EQ(v[2][13], 143.1f);
	EXPECT_FLOAT_EQ(v[2][14], 143.2f);
	EXPECT_FLOAT_EQ(v[2][15], 143.3f);

	EXPECT_FALSE(strct.getVectorMatrix(9999, v));

	EXPECT_THROW(strct.getVectorMatrix(1024, v), Common::Exception);
}

GTEST_TEST(GFF4StructList, getData) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4ListValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	Common::SeekableReadStream *data1 = strct.getData(256);
	ASSERT_NE(data1, static_cast<Common::SeekableReadStream *>(0));
	ASSERT_EQ(data1->size(), 3);

	EXPECT_EQ(data1->readByte(), 23);
	EXPECT_EQ(data1->readByte(), 24);
	EXPECT_EQ(data1->readByte(), 25);
	delete data1;

	Common::SeekableReadStream *data2 = strct.getData(9999);
	ASSERT_EQ(data2, static_cast<Common::SeekableReadStream *>(0));
}

// --- GFF4, reference values ---

static const byte kGFF4RefValues[] = {
	0x47,0x46,0x46,0x20,0x56,0x34,0x2E,0x30,0x50,0x43,0x20,0x20,0x54,0x45,0x53,0x54,
	0x56,0x31,0x2E,0x30,0x01,0x00,0x00,0x00,0x10,0x01,0x00,0x00,0x53,0x54,0x43,0x54,
	0x13,0x00,0x00,0x00,0x2C,0x00,0x00,0x00,0x18,0x01,0x00,0x00,0x00,0x01,0x00,0x00,
	0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x00,0x01,0x01,0x00,0x00,0x01,0x00,0x00,0x20,
	0x04,0x00,0x00,0x00,0x02,0x01,0x00,0x00,0x02,0x00,0x00,0x20,0x08,0x00,0x00,0x00,
	0x03,0x01,0x00,0x00,0x03,0x00,0x00,0x20,0x0C,0x00,0x00,0x00,0x04,0x01,0x00,0x00,
	0x04,0x00,0x00,0x20,0x10,0x00,0x00,0x00,0x05,0x01,0x00,0x00,0x05,0x00,0x00,0x20,
	0x14,0x00,0x00,0x00,0x06,0x01,0x00,0x00,0x06,0x00,0x00,0x20,0x18,0x00,0x00,0x00,
	0x07,0x01,0x00,0x00,0x07,0x00,0x00,0x20,0x1C,0x00,0x00,0x00,0x00,0x02,0x00,0x00,
	0x08,0x00,0x00,0x20,0x20,0x00,0x00,0x00,0x01,0x02,0x00,0x00,0x09,0x00,0x00,0x20,
	0x24,0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x0A,0x00,0x00,0x20,0x28,0x00,0x00,0x00,
	0x01,0x03,0x00,0x00,0x0C,0x00,0x00,0x20,0x2C,0x00,0x00,0x00,0x02,0x03,0x00,0x00,
	0x0D,0x00,0x00,0x20,0x30,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x0E,0x00,0x00,0x20,
	0x34,0x00,0x00,0x00,0x03,0x03,0x00,0x00,0x0F,0x00,0x00,0x20,0x38,0x00,0x00,0x00,
	0x04,0x03,0x00,0x00,0x10,0x00,0x00,0x20,0x3C,0x00,0x00,0x00,0x01,0x04,0x00,0x00,
	0x11,0x00,0x00,0x20,0x40,0x00,0x00,0x00,0x02,0x02,0x00,0x00,0x12,0x00,0x00,0x20,
	0x44,0x00,0x00,0x00,0x02,0x04,0x00,0x00,0x14,0x00,0x00,0x20,0x48,0x00,0x00,0x00,
	0x4C,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4E,0x00,0x00,0x00,0x50,0x00,0x00,0x00,
	0x52,0x00,0x00,0x00,0x56,0x00,0x00,0x00,0x5A,0x00,0x00,0x00,0x62,0x00,0x00,0x00,
	0x6A,0x00,0x00,0x00,0x6E,0x00,0x00,0x00,0x76,0x00,0x00,0x00,0x82,0x00,0x00,0x00,
	0x92,0x00,0x00,0x00,0xA2,0x00,0x00,0x00,0xB2,0x00,0x00,0x00,0xC2,0x00,0x00,0x00,
	0x02,0x01,0x00,0x00,0x0A,0x01,0x00,0x00,0x0E,0x01,0x00,0x00,0x17,0xE9,0x18,0x00,
	0xE8,0xFF,0x19,0x00,0x00,0x00,0xE7,0xFF,0xFF,0xFF,0x1A,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0xE6,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xCD,0xCC,0xD8,0x41,0x33,0x33,
	0x33,0x33,0x33,0x33,0x3B,0x40,0xCD,0xCC,0xE0,0x41,0x9A,0x99,0xE1,0x41,0x66,0x66,
	0xE2,0x41,0xCD,0xCC,0xE8,0x41,0x9A,0x99,0xE9,0x41,0x66,0x66,0xEA,0x41,0x33,0x33,
	0xEB,0x41,0xCD,0xCC,0xF0,0x41,0x9A,0x99,0xF1,0x41,0x66,0x66,0xF2,0x41,0x33,0x33,
	0xF3,0x41,0x06,0x00,0x00,0x00,0x42,0x00,0x61,0x00,0x72,0x00,0x66,0x00,0x6F,0x00,
	0x6F,0x00,0xCD,0xCC,0xF8,0x41,0x9A,0x99,0xF9,0x41,0x66,0x66,0xFA,0x41,0x33,0x33,
	0xFB,0x41,0x00,0x00,0x20,0x42,0x66,0x66,0x20,0x42,0xCD,0xCC,0x20,0x42,0x33,0x33,
	0x21,0x42,0x00,0x00,0x24,0x42,0x66,0x66,0x24,0x42,0xCD,0xCC,0x24,0x42,0x33,0x33,
	0x25,0x42,0x00,0x00,0x28,0x42,0x66,0x66,0x28,0x42,0xCD,0xCC,0x28,0x42,0x33,0x33,
	0x29,0x42,0x00,0x00,0x2C,0x42,0x66,0x66,0x2C,0x42,0xCD,0xCC,0x2C,0x42,0x33,0x33,
	0x2D,0x42,0x33,0x00,0x00,0x00,0x18,0x01,0x00,0x00,0xAE,0x53,0x00,0x00,0x06,0x00,
	0x00,0x00,0x46,0x6F,0x6F,0x62,0x61,0x72,0x06,0x00,0x00,0x00,0x51,0x00,0x75,0x00,
	0x75,0x00,0x75,0x00,0x75,0x00,0x78,0x00
};

static const uint32_t kFieldLabelsRef[] = {
	256, 257, 258, 259, 260, 261, 262, 263, 512, 513, 514, 768, 769, 770, 771, 772, 1024, 1025, 1026,
};

static const Aurora::GFF4Struct::FieldType kFieldTypesRef[] = {
	Aurora::GFF4Struct::kFieldTypeUint8,
	Aurora::GFF4Struct::kFieldTypeSint8,
	Aurora::GFF4Struct::kFieldTypeUint16,
	Aurora::GFF4Struct::kFieldTypeSint16,
	Aurora::GFF4Struct::kFieldTypeUint32,
	Aurora::GFF4Struct::kFieldTypeSint32,
	Aurora::GFF4Struct::kFieldTypeUint64,
	Aurora::GFF4Struct::kFieldTypeSint64,
	Aurora::GFF4Struct::kFieldTypeFloat32,
	Aurora::GFF4Struct::kFieldTypeFloat64,
	Aurora::GFF4Struct::kFieldTypeNDSFixed,
	Aurora::GFF4Struct::kFieldTypeVector3f,
	Aurora::GFF4Struct::kFieldTypeVector4f,
	Aurora::GFF4Struct::kFieldTypeQuaternionf,
	Aurora::GFF4Struct::kFieldTypeColor4f,
	Aurora::GFF4Struct::kFieldTypeMatrix4x4f,
	Aurora::GFF4Struct::kFieldTypeString,
	Aurora::GFF4Struct::kFieldTypeTlkString,
	Aurora::GFF4Struct::kFieldTypeASCIIString
};

GTEST_TEST(GFF4StructRef, getFieldCount) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4RefValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	EXPECT_EQ(strct.getFieldCount(), ARRAYSIZE(kFieldLabelsRef));
}

GTEST_TEST(GFF4StructRef, hasField) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4RefValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	for (size_t i = 0; i < ARRAYSIZE(kFieldLabelsRef); i++)
		EXPECT_TRUE(strct.hasField(kFieldLabelsRef[i])) << "At index " << i;

	EXPECT_FALSE(strct.hasField(9999));
}

GTEST_TEST(GFF4StructRef, getFieldLabels) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4RefValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	std::vector<uint32_t> labels = strct.getFieldLabels();
	std::sort(labels.begin(), labels.end());

	for (size_t i = 0; i < ARRAYSIZE(kFieldLabelsRef); i++)
		EXPECT_EQ(labels[i], kFieldLabelsRef[i]) << "At index " << i;
}

GTEST_TEST(GFF4StructRef, getFieldType) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4RefValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	for (size_t i = 0; i < ARRAYSIZE(kFieldLabelsRef); i++)
		EXPECT_EQ(strct.getFieldType(kFieldLabelsRef[i]), kFieldTypesRef[i]) << "At index " << i;

	EXPECT_EQ(strct.getFieldType(9999), Aurora::GFF4Struct::kFieldTypeNone);
}

GTEST_TEST(GFF4StructRef, getFieldTypeList) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4RefValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	for (size_t i = 0; i < ARRAYSIZE(kFieldLabelsRef); i++) {
		bool isList;

		EXPECT_EQ(strct.getFieldType(kFieldLabelsRef[i], isList), kFieldTypesRef[i]) << "At index " << i;
		EXPECT_FALSE(isList);
	}

	bool isList;
	EXPECT_EQ(strct.getFieldType(9999, isList), Aurora::GFF4Struct::kFieldTypeNone);
}

GTEST_TEST(GFF4StructRef, getFieldProperties) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4RefValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	for (size_t i = 0; i < ARRAYSIZE(kFieldLabelsRef); i++) {
		Aurora::GFF4Struct::FieldType type;
		uint32_t label;
		bool isList;

		EXPECT_TRUE(strct.getFieldProperties(kFieldLabelsRef[i], type, label, isList)) << "At index " << i;
		EXPECT_EQ(type, kFieldTypesRef[i]);
		EXPECT_EQ(label, kFieldLabelsRef[i]);
		EXPECT_FALSE(isList);
	}

	Aurora::GFF4Struct::FieldType type;
	uint32_t label;
	bool isList;

	EXPECT_FALSE(strct.getFieldProperties(9999, type, label, isList));
}

GTEST_TEST(GFF4StructRef, getUint) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4RefValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	EXPECT_EQ(strct.getUint(256), 23);
	EXPECT_EQ(strct.getUint(258), 24);
	EXPECT_EQ(strct.getUint(260), 25);
	EXPECT_EQ(strct.getUint(262), 26);

	EXPECT_EQ(strct.getUint(257), (uint64_t)((int64_t) -23));
	EXPECT_EQ(strct.getUint(259), (uint64_t)((int64_t) -24));
	EXPECT_EQ(strct.getUint(261), (uint64_t)((int64_t) -25));
	EXPECT_EQ(strct.getUint(263), (uint64_t)((int64_t) -26));

	EXPECT_EQ(strct.getUint(9999, 9999), 9999);

	EXPECT_THROW(strct.getUint(1024), Common::Exception);
}

GTEST_TEST(GFF4StructRef, getSint) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4RefValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	EXPECT_EQ(strct.getSint(256), 23);
	EXPECT_EQ(strct.getSint(258), 24);
	EXPECT_EQ(strct.getSint(260), 25);
	EXPECT_EQ(strct.getSint(262), 26);

	EXPECT_EQ(strct.getSint(257), -23);
	EXPECT_EQ(strct.getSint(259), -24);
	EXPECT_EQ(strct.getSint(261), -25);
	EXPECT_EQ(strct.getSint(263), -26);

	EXPECT_EQ(strct.getSint(9999, 9999), 9999);

	EXPECT_THROW(strct.getSint(1024), Common::Exception);
}

GTEST_TEST(GFF4StructRef, getBool) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4RefValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	EXPECT_EQ(strct.getBool(256), true);
	EXPECT_EQ(strct.getBool(258), true);
	EXPECT_EQ(strct.getBool(260), true);
	EXPECT_EQ(strct.getBool(262), true);

	EXPECT_EQ(strct.getBool(257), true);
	EXPECT_EQ(strct.getBool(259), true);
	EXPECT_EQ(strct.getBool(261), true);
	EXPECT_EQ(strct.getBool(263), true);

	EXPECT_EQ(strct.getBool(9999, false), false);

	EXPECT_THROW(strct.getBool(1024), Common::Exception);
}

GTEST_TEST(GFF4StructRef, getDouble) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4RefValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	EXPECT_DOUBLE_EQ(strct.getDouble(512), 27.1f);
	EXPECT_DOUBLE_EQ(strct.getDouble(513), 27.2);

	// Nintendo fixed point
	EXPECT_NEAR(strct.getDouble(514), 5.23, 0.00005);

	EXPECT_DOUBLE_EQ(strct.getDouble(9999, 27.3), 27.3);

	EXPECT_THROW(strct.getDouble(1024), Common::Exception);
}

GTEST_TEST(GFF4StructRef, getFloat) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4RefValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	EXPECT_FLOAT_EQ(strct.getFloat(512), 27.1f);
	EXPECT_FLOAT_EQ(strct.getFloat(513), 27.2f);

	// Nintendo fixed point
	EXPECT_NEAR(strct.getFloat(514), 5.23f, 0.00005);

	EXPECT_FLOAT_EQ(strct.getFloat(9999, 27.3f), 27.3f);

	EXPECT_THROW(strct.getFloat(1024), Common::Exception);
}

GTEST_TEST(GFF4StructRef, getString) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4RefValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	EXPECT_STREQ(strct.getString(1024).c_str(), "Barfoo");
	EXPECT_STREQ(strct.getString(1026).c_str(), "Foobar");

	EXPECT_STREQ(strct.getString(9999).c_str(), "");

	EXPECT_THROW(strct.getString(512), Common::Exception);
}

GTEST_TEST(GFF4StructRef, getStringEncoding) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4RefValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	EXPECT_STREQ(strct.getString(1024, Common::kEncodingUTF16LE).c_str(), "Barfoo");
	EXPECT_STREQ(strct.getString(1026, Common::kEncodingASCII).c_str(), "Foobar");

	EXPECT_STREQ(strct.getString(9999, Common::kEncodingUTF8).c_str(), "");

	EXPECT_THROW(strct.getString(512, Common::kEncodingUTF8), Common::Exception);
}

GTEST_TEST(GFF4StructRef, getTalkString) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4RefValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	uint32_t strRef;
	Common::UString str;
	EXPECT_TRUE(strct.getTalkString(1025, strRef, str));

	EXPECT_EQ(strRef, 51);
	EXPECT_STREQ(str.c_str(), "Quuuux");

	EXPECT_FALSE(strct.getTalkString(9999, strRef, str));

	EXPECT_THROW(strct.getTalkString(512, strRef, str), Common::Exception);
}

GTEST_TEST(GFF4StructRef, getTalkStringEncoding) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4RefValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	uint32_t strRef;
	Common::UString str;
	EXPECT_TRUE(strct.getTalkString(1025, Common::kEncodingUTF16LE, strRef, str));

	EXPECT_EQ(strRef, 51);
	EXPECT_STREQ(str.c_str(), "Quuuux");

	EXPECT_FALSE(strct.getTalkString(9999, Common::kEncodingUTF16LE, strRef, str));

	EXPECT_THROW(strct.getTalkString(512, Common::kEncodingUTF16LE, strRef, str), Common::Exception);
}

GTEST_TEST(GFF4StructRef, getVector3Double) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4RefValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	double v1, v2, v3;
	EXPECT_TRUE(strct.getVector3(768, v1, v2, v3));

	EXPECT_DOUBLE_EQ(v1, 28.1f);
	EXPECT_DOUBLE_EQ(v2, 28.2f);
	EXPECT_DOUBLE_EQ(v3, 28.3f);

	EXPECT_FALSE(strct.getVector3(9999, v1, v2, v3));

	EXPECT_THROW(strct.getVector3(1024, v1, v2, v3), Common::Exception);
}

GTEST_TEST(GFF4StructRef, getVector3Float) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4RefValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	float v1, v2, v3;
	EXPECT_TRUE(strct.getVector3(768, v1, v2, v3));

	EXPECT_FLOAT_EQ(v1, 28.1f);
	EXPECT_FLOAT_EQ(v2, 28.2f);
	EXPECT_FLOAT_EQ(v3, 28.3f);

	EXPECT_FALSE(strct.getVector3(9999, v1, v2, v3));

	EXPECT_THROW(strct.getVector3(1024, v1, v2, v3), Common::Exception);
}

GTEST_TEST(GFF4StructRef, getVector4Double) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4RefValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	double v1, v2, v3, v4;

	EXPECT_TRUE(strct.getVector4(769, v1, v2, v3, v4));
	EXPECT_DOUBLE_EQ(v1, 29.1f);
	EXPECT_DOUBLE_EQ(v2, 29.2f);
	EXPECT_DOUBLE_EQ(v3, 29.3f);
	EXPECT_DOUBLE_EQ(v4, 29.4f);

	EXPECT_TRUE(strct.getVector4(770, v1, v2, v3, v4));
	EXPECT_DOUBLE_EQ(v1, 30.1f);
	EXPECT_DOUBLE_EQ(v2, 30.2f);
	EXPECT_DOUBLE_EQ(v3, 30.3f);
	EXPECT_DOUBLE_EQ(v4, 30.4f);

	EXPECT_TRUE(strct.getVector4(771, v1, v2, v3, v4));
	EXPECT_DOUBLE_EQ(v1, 31.1f);
	EXPECT_DOUBLE_EQ(v2, 31.2f);
	EXPECT_DOUBLE_EQ(v3, 31.3f);
	EXPECT_DOUBLE_EQ(v4, 31.4f);

	EXPECT_FALSE(strct.getVector4(9999, v1, v2, v3, v4));

	EXPECT_THROW(strct.getVector4(1024, v1, v2, v3, v4), Common::Exception);
}

GTEST_TEST(GFF4StructRef, getVector4Float) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4RefValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	float v1, v2, v3, v4;

	EXPECT_TRUE(strct.getVector4(769, v1, v2, v3, v4));
	EXPECT_FLOAT_EQ(v1, 29.1f);
	EXPECT_FLOAT_EQ(v2, 29.2f);
	EXPECT_FLOAT_EQ(v3, 29.3f);
	EXPECT_FLOAT_EQ(v4, 29.4f);

	EXPECT_TRUE(strct.getVector4(770, v1, v2, v3, v4));
	EXPECT_FLOAT_EQ(v1, 30.1f);
	EXPECT_FLOAT_EQ(v2, 30.2f);
	EXPECT_FLOAT_EQ(v3, 30.3f);
	EXPECT_FLOAT_EQ(v4, 30.4f);

	EXPECT_TRUE(strct.getVector4(771, v1, v2, v3, v4));
	EXPECT_FLOAT_EQ(v1, 31.1f);
	EXPECT_FLOAT_EQ(v2, 31.2f);
	EXPECT_FLOAT_EQ(v3, 31.3f);
	EXPECT_FLOAT_EQ(v4, 31.4f);

	EXPECT_FALSE(strct.getVector4(9999, v1, v2, v3, v4));

	EXPECT_THROW(strct.getVector4(1024, v1, v2, v3, v4), Common::Exception);
}

GTEST_TEST(GFF4StructRef, getMatrix4x4Double) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4RefValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	double m[16];
	EXPECT_TRUE(strct.getMatrix4x4(772, m));

	EXPECT_DOUBLE_EQ(m[ 0], 40.0f);
	EXPECT_DOUBLE_EQ(m[ 1], 40.1f);
	EXPECT_DOUBLE_EQ(m[ 2], 40.2f);
	EXPECT_DOUBLE_EQ(m[ 3], 40.3f);
	EXPECT_DOUBLE_EQ(m[ 4], 41.0f);
	EXPECT_DOUBLE_EQ(m[ 5], 41.1f);
	EXPECT_DOUBLE_EQ(m[ 6], 41.2f);
	EXPECT_DOUBLE_EQ(m[ 7], 41.3f);
	EXPECT_DOUBLE_EQ(m[ 8], 42.0f);
	EXPECT_DOUBLE_EQ(m[ 9], 42.1f);
	EXPECT_DOUBLE_EQ(m[10], 42.2f);
	EXPECT_DOUBLE_EQ(m[11], 42.3f);
	EXPECT_DOUBLE_EQ(m[12], 43.0f);
	EXPECT_DOUBLE_EQ(m[13], 43.1f);
	EXPECT_DOUBLE_EQ(m[14], 43.2f);
	EXPECT_DOUBLE_EQ(m[15], 43.3f);

	EXPECT_FALSE(strct.getMatrix4x4(9999, m));

	EXPECT_THROW(strct.getMatrix4x4(1024, m), Common::Exception);
}

GTEST_TEST(GFF4StructRef, getMatrix4x4Float) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4RefValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	float m[16];
	EXPECT_TRUE(strct.getMatrix4x4(772, m));

	EXPECT_FLOAT_EQ(m[ 0], 40.0f);
	EXPECT_FLOAT_EQ(m[ 1], 40.1f);
	EXPECT_FLOAT_EQ(m[ 2], 40.2f);
	EXPECT_FLOAT_EQ(m[ 3], 40.3f);
	EXPECT_FLOAT_EQ(m[ 4], 41.0f);
	EXPECT_FLOAT_EQ(m[ 5], 41.1f);
	EXPECT_FLOAT_EQ(m[ 6], 41.2f);
	EXPECT_FLOAT_EQ(m[ 7], 41.3f);
	EXPECT_FLOAT_EQ(m[ 8], 42.0f);
	EXPECT_FLOAT_EQ(m[ 9], 42.1f);
	EXPECT_FLOAT_EQ(m[10], 42.2f);
	EXPECT_FLOAT_EQ(m[11], 42.3f);
	EXPECT_FLOAT_EQ(m[12], 43.0f);
	EXPECT_FLOAT_EQ(m[13], 43.1f);
	EXPECT_FLOAT_EQ(m[14], 43.2f);
	EXPECT_FLOAT_EQ(m[15], 43.3f);

	EXPECT_FALSE(strct.getMatrix4x4(9999, m));

	EXPECT_THROW(strct.getMatrix4x4(1024, m), Common::Exception);
}

GTEST_TEST(GFF4StructRef, getMatrix4x4Matrix) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4RefValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	glm::mat4 m;
	EXPECT_TRUE(strct.getMatrix4x4(772, m));

	EXPECT_FLOAT_EQ(m[0][0], 40.0f);
	EXPECT_FLOAT_EQ(m[0][1], 40.1f);
	EXPECT_FLOAT_EQ(m[0][2], 40.2f);
	EXPECT_FLOAT_EQ(m[0][3], 40.3f);
	EXPECT_FLOAT_EQ(m[1][0], 41.0f);
	EXPECT_FLOAT_EQ(m[1][1], 41.1f);
	EXPECT_FLOAT_EQ(m[1][2], 41.2f);
	EXPECT_FLOAT_EQ(m[1][3], 41.3f);
	EXPECT_FLOAT_EQ(m[2][0], 42.0f);
	EXPECT_FLOAT_EQ(m[2][1], 42.1f);
	EXPECT_FLOAT_EQ(m[2][2], 42.2f);
	EXPECT_FLOAT_EQ(m[2][3], 42.3f);
	EXPECT_FLOAT_EQ(m[3][0], 43.0f);
	EXPECT_FLOAT_EQ(m[3][1], 43.1f);
	EXPECT_FLOAT_EQ(m[3][2], 43.2f);
	EXPECT_FLOAT_EQ(m[3][3], 43.3f);

	EXPECT_FALSE(strct.getMatrix4x4(9999, m));

	EXPECT_THROW(strct.getMatrix4x4(1024, m), Common::Exception);
}

GTEST_TEST(GFF4StructRef, getVectorMatrixDouble) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4RefValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	std::vector<double> v;

	EXPECT_TRUE(strct.getVectorMatrix(768, v));
	ASSERT_EQ(v.size(), 3);
	EXPECT_DOUBLE_EQ(v[0], 28.1f);
	EXPECT_DOUBLE_EQ(v[1], 28.2f);
	EXPECT_DOUBLE_EQ(v[2], 28.3f);

	EXPECT_TRUE(strct.getVectorMatrix(769, v));
	ASSERT_EQ(v.size(), 4);
	EXPECT_DOUBLE_EQ(v[0], 29.1f);
	EXPECT_DOUBLE_EQ(v[1], 29.2f);
	EXPECT_DOUBLE_EQ(v[2], 29.3f);
	EXPECT_DOUBLE_EQ(v[3], 29.4f);

	EXPECT_TRUE(strct.getVectorMatrix(770, v));
	ASSERT_EQ(v.size(), 4);
	EXPECT_DOUBLE_EQ(v[0], 30.1f);
	EXPECT_DOUBLE_EQ(v[1], 30.2f);
	EXPECT_DOUBLE_EQ(v[2], 30.3f);
	EXPECT_DOUBLE_EQ(v[3], 30.4f);

	EXPECT_TRUE(strct.getVectorMatrix(771, v));
	ASSERT_EQ(v.size(), 4);
	EXPECT_DOUBLE_EQ(v[0], 31.1f);
	EXPECT_DOUBLE_EQ(v[1], 31.2f);
	EXPECT_DOUBLE_EQ(v[2], 31.3f);
	EXPECT_DOUBLE_EQ(v[3], 31.4f);

	EXPECT_TRUE(strct.getVectorMatrix(772, v));
	ASSERT_EQ(v.size(), 16);
	EXPECT_DOUBLE_EQ(v[ 0], 40.0f);
	EXPECT_DOUBLE_EQ(v[ 1], 40.1f);
	EXPECT_DOUBLE_EQ(v[ 2], 40.2f);
	EXPECT_DOUBLE_EQ(v[ 3], 40.3f);
	EXPECT_DOUBLE_EQ(v[ 4], 41.0f);
	EXPECT_DOUBLE_EQ(v[ 5], 41.1f);
	EXPECT_DOUBLE_EQ(v[ 6], 41.2f);
	EXPECT_DOUBLE_EQ(v[ 7], 41.3f);
	EXPECT_DOUBLE_EQ(v[ 8], 42.0f);
	EXPECT_DOUBLE_EQ(v[ 9], 42.1f);
	EXPECT_DOUBLE_EQ(v[10], 42.2f);
	EXPECT_DOUBLE_EQ(v[11], 42.3f);
	EXPECT_DOUBLE_EQ(v[12], 43.0f);
	EXPECT_DOUBLE_EQ(v[13], 43.1f);
	EXPECT_DOUBLE_EQ(v[14], 43.2f);
	EXPECT_DOUBLE_EQ(v[15], 43.3f);

	EXPECT_FALSE(strct.getVectorMatrix(9999, v));

	EXPECT_THROW(strct.getVectorMatrix(1024, v), Common::Exception);
}

GTEST_TEST(GFF4StructRef, getVectorMatrixFloat) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4RefValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	std::vector<float> v;

	EXPECT_TRUE(strct.getVectorMatrix(768, v));
	ASSERT_EQ(v.size(), 3);
	EXPECT_FLOAT_EQ(v[0], 28.1f);
	EXPECT_FLOAT_EQ(v[1], 28.2f);
	EXPECT_FLOAT_EQ(v[2], 28.3f);

	EXPECT_TRUE(strct.getVectorMatrix(769, v));
	ASSERT_EQ(v.size(), 4);
	EXPECT_FLOAT_EQ(v[0], 29.1f);
	EXPECT_FLOAT_EQ(v[1], 29.2f);
	EXPECT_FLOAT_EQ(v[2], 29.3f);
	EXPECT_FLOAT_EQ(v[3], 29.4f);

	EXPECT_TRUE(strct.getVectorMatrix(770, v));
	ASSERT_EQ(v.size(), 4);
	EXPECT_FLOAT_EQ(v[0], 30.1f);
	EXPECT_FLOAT_EQ(v[1], 30.2f);
	EXPECT_FLOAT_EQ(v[2], 30.3f);
	EXPECT_FLOAT_EQ(v[3], 30.4f);

	EXPECT_TRUE(strct.getVectorMatrix(771, v));
	ASSERT_EQ(v.size(), 4);
	EXPECT_FLOAT_EQ(v[0], 31.1f);
	EXPECT_FLOAT_EQ(v[1], 31.2f);
	EXPECT_FLOAT_EQ(v[2], 31.3f);
	EXPECT_FLOAT_EQ(v[3], 31.4f);

	EXPECT_TRUE(strct.getVectorMatrix(772, v));
	ASSERT_EQ(v.size(), 16);
	EXPECT_FLOAT_EQ(v[ 0], 40.0f);
	EXPECT_FLOAT_EQ(v[ 1], 40.1f);
	EXPECT_FLOAT_EQ(v[ 2], 40.2f);
	EXPECT_FLOAT_EQ(v[ 3], 40.3f);
	EXPECT_FLOAT_EQ(v[ 4], 41.0f);
	EXPECT_FLOAT_EQ(v[ 5], 41.1f);
	EXPECT_FLOAT_EQ(v[ 6], 41.2f);
	EXPECT_FLOAT_EQ(v[ 7], 41.3f);
	EXPECT_FLOAT_EQ(v[ 8], 42.0f);
	EXPECT_FLOAT_EQ(v[ 9], 42.1f);
	EXPECT_FLOAT_EQ(v[10], 42.2f);
	EXPECT_FLOAT_EQ(v[11], 42.3f);
	EXPECT_FLOAT_EQ(v[12], 43.0f);
	EXPECT_FLOAT_EQ(v[13], 43.1f);
	EXPECT_FLOAT_EQ(v[14], 43.2f);
	EXPECT_FLOAT_EQ(v[15], 43.3f);

	EXPECT_FALSE(strct.getVectorMatrix(9999, v));

	EXPECT_THROW(strct.getVectorMatrix(1024, v), Common::Exception);
}


GTEST_TEST(GFF4StructRef, getUintList) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4RefValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	std::vector<uint64_t> list;

	EXPECT_TRUE(strct.getUint(256, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], 23);

	EXPECT_TRUE(strct.getUint(258, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], 24);

	EXPECT_TRUE(strct.getUint(260, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], 25);

	EXPECT_TRUE(strct.getUint(262, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], 26);

	EXPECT_TRUE(strct.getUint(257, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], (uint64_t)((int64_t) -23));

	EXPECT_TRUE(strct.getUint(259, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], (uint64_t)((int64_t) -24));

	EXPECT_TRUE(strct.getUint(261, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], (uint64_t)((int64_t) -25));

	EXPECT_TRUE(strct.getUint(263, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], (uint64_t)((int64_t) -26));

	EXPECT_FALSE(strct.getUint(9999, list));

	EXPECT_THROW(strct.getUint(1024, list), Common::Exception);
}

GTEST_TEST(GFF4StructRef, getSintList) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4RefValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	std::vector<int64_t> list;

	EXPECT_TRUE(strct.getSint(256, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], 23);

	EXPECT_TRUE(strct.getSint(258, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], 24);

	EXPECT_TRUE(strct.getSint(260, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], 25);

	EXPECT_TRUE(strct.getSint(262, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], 26);

	EXPECT_TRUE(strct.getSint(257, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], -23);

	EXPECT_TRUE(strct.getSint(259, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], -24);

	EXPECT_TRUE(strct.getSint(261, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], -25);

	EXPECT_TRUE(strct.getSint(263, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], -26);

	EXPECT_FALSE(strct.getSint(9999, list));

	EXPECT_THROW(strct.getSint(1024, list), Common::Exception);
}

GTEST_TEST(GFF4StructRef, getBoolList) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4RefValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	std::vector<bool> list;

	EXPECT_TRUE(strct.getBool(256, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], true);

	EXPECT_TRUE(strct.getBool(258, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], true);

	EXPECT_TRUE(strct.getBool(260, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], true);

	EXPECT_TRUE(strct.getBool(262, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], true);

	EXPECT_TRUE(strct.getBool(257, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], true);

	EXPECT_TRUE(strct.getBool(259, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], true);

	EXPECT_TRUE(strct.getBool(261, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], true);

	EXPECT_TRUE(strct.getBool(263, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], true);

	EXPECT_FALSE(strct.getBool(9999, list));

	EXPECT_THROW(strct.getBool(1024, list), Common::Exception);
}

GTEST_TEST(GFF4StructRef, getDoubleList) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4RefValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	std::vector<double> list;

	EXPECT_TRUE(strct.getDouble(512, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_DOUBLE_EQ(list[0], 27.1f);

	EXPECT_TRUE(strct.getDouble(513, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_DOUBLE_EQ(list[0], 27.2);

	// Nintendo fixed point
	EXPECT_TRUE(strct.getDouble(514, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_NEAR(list[0], 5.23, 0.00005);

	EXPECT_FALSE(strct.getDouble(9999, list));

	EXPECT_THROW(strct.getDouble(1024, list), Common::Exception);
}

GTEST_TEST(GFF4StructRef, getFloatList) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4RefValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	std::vector<float> list;

	EXPECT_TRUE(strct.getFloat(512, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_FLOAT_EQ(list[0], 27.1f);

	EXPECT_TRUE(strct.getFloat(513, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_FLOAT_EQ(list[0], 27.2f);

	// Nintendo fixed point
	EXPECT_TRUE(strct.getFloat(514, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_NEAR(list[0], 5.23f, 0.00005);

	EXPECT_FALSE(strct.getFloat(9999, list));

	EXPECT_THROW(strct.getFloat(1024, list), Common::Exception);
}

GTEST_TEST(GFF4StructRef, getStringList) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4RefValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	std::vector<Common::UString> list;

	EXPECT_TRUE(strct.getString(1024, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_STREQ(list[0].c_str(), "Barfoo");

	EXPECT_TRUE(strct.getString(1026, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_STREQ(list[0].c_str(), "Foobar");

	EXPECT_FALSE(strct.getString(9999, list));

	EXPECT_THROW(strct.getString(512, list), Common::Exception);
}

GTEST_TEST(GFF4StructRef, getStringEncodingList) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4RefValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	std::vector<Common::UString> list;

	EXPECT_TRUE(strct.getString(1024, Common::kEncodingUTF16LE, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_STREQ(list[0].c_str(), "Barfoo");

	EXPECT_TRUE(strct.getString(1026, Common::kEncodingASCII, list));
	ASSERT_EQ(list.size(), 1);
	EXPECT_STREQ(list[0].c_str(), "Foobar");

	EXPECT_FALSE(strct.getString(9999, Common::kEncodingUTF8, list));

	EXPECT_THROW(strct.getString(512, Common::kEncodingUTF8, list), Common::Exception);
}

GTEST_TEST(GFF4StructRef, getTalkStringList) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4RefValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	std::vector<uint32_t> strRefs;
	std::vector<Common::UString> strs;

	EXPECT_TRUE(strct.getTalkString(1025, strRefs, strs));

	ASSERT_EQ(strRefs.size(), 1);
	ASSERT_EQ(strs.size(), 1);

	EXPECT_EQ(strRefs[0], 51);
	EXPECT_STREQ(strs[0].c_str(), "Quuuux");

	EXPECT_FALSE(strct.getTalkString(9999, strRefs, strs));

	EXPECT_THROW(strct.getTalkString(512, strRefs, strs), Common::Exception);
}

GTEST_TEST(GFF4StructRef, getTalkStringEncodingList) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4RefValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	std::vector<uint32_t> strRefs;
	std::vector<Common::UString> strs;

	EXPECT_TRUE(strct.getTalkString(1025, Common::kEncodingUTF16LE, strRefs, strs));

	ASSERT_EQ(strRefs.size(), 1);
	ASSERT_EQ(strs.size(), 1);

	EXPECT_EQ(strRefs[0], 51);
	EXPECT_STREQ(strs[0].c_str(), "Quuuux");

	EXPECT_FALSE(strct.getTalkString(9999, Common::kEncodingUTF16LE, strRefs, strs));

	EXPECT_THROW(strct.getTalkString(512, Common::kEncodingUTF16LE, strRefs, strs), Common::Exception);
}

GTEST_TEST(GFF4StructRef, getMatrix4x4MatrixList) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4RefValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	std::vector<glm::mat4> list;

	EXPECT_TRUE(strct.getMatrix4x4(772, list));
	ASSERT_EQ(list.size(), 1);

	EXPECT_FLOAT_EQ(list[0][0][0], 40.0f);
	EXPECT_FLOAT_EQ(list[0][0][1], 40.1f);
	EXPECT_FLOAT_EQ(list[0][0][2], 40.2f);
	EXPECT_FLOAT_EQ(list[0][0][3], 40.3f);
	EXPECT_FLOAT_EQ(list[0][1][0], 41.0f);
	EXPECT_FLOAT_EQ(list[0][1][1], 41.1f);
	EXPECT_FLOAT_EQ(list[0][1][2], 41.2f);
	EXPECT_FLOAT_EQ(list[0][1][3], 41.3f);
	EXPECT_FLOAT_EQ(list[0][2][0], 42.0f);
	EXPECT_FLOAT_EQ(list[0][2][1], 42.1f);
	EXPECT_FLOAT_EQ(list[0][2][2], 42.2f);
	EXPECT_FLOAT_EQ(list[0][2][3], 42.3f);
	EXPECT_FLOAT_EQ(list[0][3][0], 43.0f);
	EXPECT_FLOAT_EQ(list[0][3][1], 43.1f);
	EXPECT_FLOAT_EQ(list[0][3][2], 43.2f);
	EXPECT_FLOAT_EQ(list[0][3][3], 43.3f);

	EXPECT_FALSE(strct.getMatrix4x4(9999, list));

	EXPECT_THROW(strct.getMatrix4x4(1024, list), Common::Exception);
}

GTEST_TEST(GFF4StructRef, getVectorMatrixDoubleList) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4RefValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	std::vector< std::vector<double> > v;

	EXPECT_TRUE(strct.getVectorMatrix(768, v));
	ASSERT_EQ(v.size(), 1);
	ASSERT_EQ(v[0].size(), 3);
	EXPECT_DOUBLE_EQ(v[0][0], 28.1f);
	EXPECT_DOUBLE_EQ(v[0][1], 28.2f);
	EXPECT_DOUBLE_EQ(v[0][2], 28.3f);

	EXPECT_TRUE(strct.getVectorMatrix(769, v));
	ASSERT_EQ(v.size(), 1);
	ASSERT_EQ(v[0].size(), 4);
	EXPECT_DOUBLE_EQ(v[0][0], 29.1f);
	EXPECT_DOUBLE_EQ(v[0][1], 29.2f);
	EXPECT_DOUBLE_EQ(v[0][2], 29.3f);
	EXPECT_DOUBLE_EQ(v[0][3], 29.4f);

	EXPECT_TRUE(strct.getVectorMatrix(770, v));
	ASSERT_EQ(v.size(), 1);
	ASSERT_EQ(v[0].size(), 4);
	EXPECT_DOUBLE_EQ(v[0][0], 30.1f);
	EXPECT_DOUBLE_EQ(v[0][1], 30.2f);
	EXPECT_DOUBLE_EQ(v[0][2], 30.3f);
	EXPECT_DOUBLE_EQ(v[0][3], 30.4f);

	EXPECT_TRUE(strct.getVectorMatrix(771, v));
	ASSERT_EQ(v.size(), 1);
	ASSERT_EQ(v[0].size(), 4);
	EXPECT_DOUBLE_EQ(v[0][0], 31.1f);
	EXPECT_DOUBLE_EQ(v[0][1], 31.2f);
	EXPECT_DOUBLE_EQ(v[0][2], 31.3f);
	EXPECT_DOUBLE_EQ(v[0][3], 31.4f);

	EXPECT_TRUE(strct.getVectorMatrix(772, v));
	ASSERT_EQ(v.size(), 1);
	ASSERT_EQ(v[0].size(), 16);
	EXPECT_DOUBLE_EQ(v[0][ 0], 40.0f);
	EXPECT_DOUBLE_EQ(v[0][ 1], 40.1f);
	EXPECT_DOUBLE_EQ(v[0][ 2], 40.2f);
	EXPECT_DOUBLE_EQ(v[0][ 3], 40.3f);
	EXPECT_DOUBLE_EQ(v[0][ 4], 41.0f);
	EXPECT_DOUBLE_EQ(v[0][ 5], 41.1f);
	EXPECT_DOUBLE_EQ(v[0][ 6], 41.2f);
	EXPECT_DOUBLE_EQ(v[0][ 7], 41.3f);
	EXPECT_DOUBLE_EQ(v[0][ 8], 42.0f);
	EXPECT_DOUBLE_EQ(v[0][ 9], 42.1f);
	EXPECT_DOUBLE_EQ(v[0][10], 42.2f);
	EXPECT_DOUBLE_EQ(v[0][11], 42.3f);
	EXPECT_DOUBLE_EQ(v[0][12], 43.0f);
	EXPECT_DOUBLE_EQ(v[0][13], 43.1f);
	EXPECT_DOUBLE_EQ(v[0][14], 43.2f);
	EXPECT_DOUBLE_EQ(v[0][15], 43.3f);

	EXPECT_FALSE(strct.getVectorMatrix(9999, v));

	EXPECT_THROW(strct.getVectorMatrix(1024, v), Common::Exception);
}

GTEST_TEST(GFF4StructRef, getVectorMatrixFloatList) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4RefValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	std::vector< std::vector<float> > v;

	EXPECT_TRUE(strct.getVectorMatrix(768, v));
	ASSERT_EQ(v.size(), 1);
	ASSERT_EQ(v[0].size(), 3);
	EXPECT_FLOAT_EQ(v[0][0], 28.1f);
	EXPECT_FLOAT_EQ(v[0][1], 28.2f);
	EXPECT_FLOAT_EQ(v[0][2], 28.3f);

	EXPECT_TRUE(strct.getVectorMatrix(769, v));
	ASSERT_EQ(v.size(), 1);
	ASSERT_EQ(v[0].size(), 4);
	EXPECT_FLOAT_EQ(v[0][0], 29.1f);
	EXPECT_FLOAT_EQ(v[0][1], 29.2f);
	EXPECT_FLOAT_EQ(v[0][2], 29.3f);
	EXPECT_FLOAT_EQ(v[0][3], 29.4f);

	EXPECT_TRUE(strct.getVectorMatrix(770, v));
	ASSERT_EQ(v.size(), 1);
	ASSERT_EQ(v[0].size(), 4);
	EXPECT_FLOAT_EQ(v[0][0], 30.1f);
	EXPECT_FLOAT_EQ(v[0][1], 30.2f);
	EXPECT_FLOAT_EQ(v[0][2], 30.3f);
	EXPECT_FLOAT_EQ(v[0][3], 30.4f);

	EXPECT_TRUE(strct.getVectorMatrix(771, v));
	ASSERT_EQ(v.size(), 1);
	ASSERT_EQ(v[0].size(), 4);
	EXPECT_FLOAT_EQ(v[0][0], 31.1f);
	EXPECT_FLOAT_EQ(v[0][1], 31.2f);
	EXPECT_FLOAT_EQ(v[0][2], 31.3f);
	EXPECT_FLOAT_EQ(v[0][3], 31.4f);

	EXPECT_TRUE(strct.getVectorMatrix(772, v));
	ASSERT_EQ(v.size(), 1);
	ASSERT_EQ(v[0].size(), 16);
	EXPECT_FLOAT_EQ(v[0][ 0], 40.0f);
	EXPECT_FLOAT_EQ(v[0][ 1], 40.1f);
	EXPECT_FLOAT_EQ(v[0][ 2], 40.2f);
	EXPECT_FLOAT_EQ(v[0][ 3], 40.3f);
	EXPECT_FLOAT_EQ(v[0][ 4], 41.0f);
	EXPECT_FLOAT_EQ(v[0][ 5], 41.1f);
	EXPECT_FLOAT_EQ(v[0][ 6], 41.2f);
	EXPECT_FLOAT_EQ(v[0][ 7], 41.3f);
	EXPECT_FLOAT_EQ(v[0][ 8], 42.0f);
	EXPECT_FLOAT_EQ(v[0][ 9], 42.1f);
	EXPECT_FLOAT_EQ(v[0][10], 42.2f);
	EXPECT_FLOAT_EQ(v[0][11], 42.3f);
	EXPECT_FLOAT_EQ(v[0][12], 43.0f);
	EXPECT_FLOAT_EQ(v[0][13], 43.1f);
	EXPECT_FLOAT_EQ(v[0][14], 43.2f);
	EXPECT_FLOAT_EQ(v[0][15], 43.3f);

	EXPECT_FALSE(strct.getVectorMatrix(9999, v));

	EXPECT_THROW(strct.getVectorMatrix(1024, v), Common::Exception);
}

GTEST_TEST(GFF4StructRef, getData) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4RefValues));
	const Aurora::GFF4Struct &strct = gff4.getTopLevel();

	Common::SeekableReadStream *data1 = strct.getData(256);
	ASSERT_NE(data1, static_cast<Common::SeekableReadStream *>(0));
	ASSERT_EQ(data1->size(), 1);

	EXPECT_EQ(data1->readByte(), 23);
	delete data1;

	Common::SeekableReadStream *data2 = strct.getData(9999);
	ASSERT_EQ(data2, static_cast<Common::SeekableReadStream *>(0));
}

// --- GFF4, structs ---

static const byte kGFF4Structs[] = {
	0x47,0x46,0x46,0x20,0x56,0x34,0x2E,0x30,0x50,0x43,0x20,0x20,0x54,0x45,0x53,0x54,
	0x56,0x31,0x2E,0x30,0x03,0x00,0x00,0x00,0xA0,0x00,0x00,0x00,0x53,0x43,0x54,0x31,
	0x04,0x00,0x00,0x00,0x4C,0x00,0x00,0x00,0x07,0x00,0x00,0x00,0x53,0x43,0x54,0x32,
	0x02,0x00,0x00,0x00,0x7C,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x53,0x43,0x54,0x33,
	0x01,0x00,0x00,0x00,0x94,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x01,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x00,0x00,0x01,0x00,0x00,0x40,
	0x01,0x00,0x00,0x00,0x02,0x01,0x00,0x00,0x01,0x00,0x00,0x40,0x03,0x00,0x00,0x00,
	0x03,0x01,0x00,0x00,0x01,0x00,0x00,0x40,0x05,0x00,0x00,0x00,0x00,0x02,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x02,0x00,0x00,0x02,0x00,0x00,0x40,
	0x01,0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D
};

GTEST_TEST(GFF4StructStructs, getStruct) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4Structs));
	const Aurora::GFF4Struct &strct0 = gff4.getTopLevel();

	EXPECT_EQ(strct0.getLabel(), MKTAG('S', 'C', 'T', '1'));
	EXPECT_EQ(strct0.getUint(256), 23);

	const Aurora::GFF4Struct *strct1 = strct0.getStruct(257);
	const Aurora::GFF4Struct *strct2 = strct0.getStruct(258);
	const Aurora::GFF4Struct *strct3 = strct0.getStruct(259);

	ASSERT_NE(strct1, static_cast<const Aurora::GFF4Struct *>(0));
	EXPECT_EQ(strct1->getLabel(), MKTAG('S', 'C', 'T', '2'));
	EXPECT_EQ(strct1->getUint(512), 24);

	ASSERT_NE(strct2, static_cast<const Aurora::GFF4Struct *>(0));
	EXPECT_EQ(strct2->getLabel(), MKTAG('S', 'C', 'T', '2'));
	EXPECT_EQ(strct2->getUint(512), 26);

	ASSERT_NE(strct3, static_cast<const Aurora::GFF4Struct *>(0));
	EXPECT_EQ(strct3->getLabel(), MKTAG('S', 'C', 'T', '2'));
	EXPECT_EQ(strct3->getUint(512), 28);

	const Aurora::GFF4Struct *strct4 = strct1->getStruct(513);
	const Aurora::GFF4Struct *strct5 = strct2->getStruct(513);
	const Aurora::GFF4Struct *strct6 = strct3->getStruct(513);

	ASSERT_NE(strct4, static_cast<const Aurora::GFF4Struct *>(0));
	EXPECT_EQ(strct4->getLabel(), MKTAG('S', 'C', 'T', '3'));
	EXPECT_EQ(strct4->getUint(768), 25);

	ASSERT_NE(strct5, static_cast<const Aurora::GFF4Struct *>(0));
	EXPECT_EQ(strct5->getLabel(), MKTAG('S', 'C', 'T', '3'));
	EXPECT_EQ(strct5->getUint(768), 27);

	ASSERT_NE(strct6, static_cast<const Aurora::GFF4Struct *>(0));
	EXPECT_EQ(strct6->getLabel(), MKTAG('S', 'C', 'T', '3'));
	EXPECT_EQ(strct6->getUint(768), 29);

	EXPECT_THROW(strct0.getStruct(256), Common::Exception);
}

// --- GFF4, lists ---

static const byte kGFF4Lists[] = {
	0x47,0x46,0x46,0x20,0x56,0x34,0x2E,0x30,0x50,0x43,0x20,0x20,0x54,0x45,0x53,0x54,
	0x56,0x31,0x2E,0x30,0x03,0x00,0x00,0x00,0x88,0x00,0x00,0x00,0x53,0x43,0x54,0x31,
	0x02,0x00,0x00,0x00,0x4C,0x00,0x00,0x00,0x2A,0x00,0x00,0x00,0x53,0x43,0x54,0x32,
	0x02,0x00,0x00,0x00,0x64,0x00,0x00,0x00,0x0B,0x00,0x00,0x00,0x53,0x43,0x54,0x33,
	0x01,0x00,0x00,0x00,0x7C,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x01,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x00,0x00,0x01,0x00,0x00,0xC0,
	0x01,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x01,0x02,0x00,0x00,0x02,0x00,0x00,0xC0,0x01,0x00,0x00,0x00,0x00,0x03,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x17,0x05,0x00,0x00,0x00,0x03,0x00,0x00,
	0x00,0x18,0x0E,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x19,0x1A,0x1B,0x19,0x00,0x00,
	0x00,0x02,0x00,0x00,0x00,0x1C,0x1D,0x1E,0x24,0x00,0x00,0x00,0x02,0x00,0x00,0x00,
	0x1F,0x20
};

GTEST_TEST(GFF4StructLists, getList) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4Lists));
	const Aurora::GFF4Struct &strct0 = gff4.getTopLevel();

	EXPECT_EQ(strct0.getLabel(), MKTAG('S', 'C', 'T', '1'));
	EXPECT_EQ(strct0.getUint(256), 23);

	const Aurora::GFF4List &list0 = strct0.getList(257);
	ASSERT_EQ(list0.size(), 3);

	ASSERT_NE(list0[0], static_cast<const Aurora::GFF4Struct *>(0));
	EXPECT_EQ(list0[0]->getLabel(), MKTAG('S', 'C', 'T', '2'));
	EXPECT_EQ(list0[0]->getUint(512), 24);

	ASSERT_NE(list0[1], static_cast<const Aurora::GFF4Struct *>(0));
	EXPECT_EQ(list0[1]->getLabel(), MKTAG('S', 'C', 'T', '2'));
	EXPECT_EQ(list0[1]->getUint(512), 27);

	ASSERT_NE(list0[2], static_cast<const Aurora::GFF4Struct *>(0));
	EXPECT_EQ(list0[2]->getLabel(), MKTAG('S', 'C', 'T', '2'));
	EXPECT_EQ(list0[2]->getUint(512), 30);

	const Aurora::GFF4List &list1 = list0[0]->getList(513);
	const Aurora::GFF4List &list2 = list0[1]->getList(513);
	const Aurora::GFF4List &list3 = list0[2]->getList(513);

	ASSERT_EQ(list1.size(), 2);
	ASSERT_NE(list1[0], static_cast<const Aurora::GFF4Struct *>(0));
	EXPECT_EQ(list1[0]->getLabel(), MKTAG('S', 'C', 'T', '3'));
	EXPECT_EQ(list1[0]->getUint(768), 25);
	ASSERT_NE(list1[1], static_cast<const Aurora::GFF4Struct *>(0));
	EXPECT_EQ(list1[1]->getLabel(), MKTAG('S', 'C', 'T', '3'));
	EXPECT_EQ(list1[1]->getUint(768), 26);

	ASSERT_EQ(list2.size(), 2);
	ASSERT_NE(list2[0], static_cast<const Aurora::GFF4Struct *>(0));
	EXPECT_EQ(list2[0]->getLabel(), MKTAG('S', 'C', 'T', '3'));
	EXPECT_EQ(list2[0]->getUint(768), 28);
	ASSERT_NE(list2[1], static_cast<const Aurora::GFF4Struct *>(0));
	EXPECT_EQ(list2[1]->getLabel(), MKTAG('S', 'C', 'T', '3'));
	EXPECT_EQ(list2[1]->getUint(768), 29);

	ASSERT_EQ(list3.size(), 2);
	ASSERT_NE(list3[0], static_cast<const Aurora::GFF4Struct *>(0));
	EXPECT_EQ(list3[0]->getLabel(), MKTAG('S', 'C', 'T', '3'));
	EXPECT_EQ(list3[0]->getUint(768), 31);
	ASSERT_NE(list3[1], static_cast<const Aurora::GFF4Struct *>(0));
	EXPECT_EQ(list3[1]->getLabel(), MKTAG('S', 'C', 'T', '3'));
	EXPECT_EQ(list3[1]->getUint(768), 32);

	EXPECT_THROW(strct0.getList(256), Common::Exception);
	EXPECT_THROW(strct0.getStruct(257), Common::Exception);
}

// --- GFF4, structs, with references ---

static const byte kGFF4StructsRef[] = {
	0x47,0x46,0x46,0x20,0x56,0x34,0x2E,0x30,0x50,0x43,0x20,0x20,0x54,0x45,0x53,0x54,
	0x56,0x31,0x2E,0x30,0x03,0x00,0x00,0x00,0xA0,0x00,0x00,0x00,0x53,0x43,0x54,0x31,
	0x04,0x00,0x00,0x00,0x4C,0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x53,0x43,0x54,0x32,
	0x02,0x00,0x00,0x00,0x7C,0x00,0x00,0x00,0x05,0x00,0x00,0x00,0x53,0x43,0x54,0x33,
	0x01,0x00,0x00,0x00,0x94,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x01,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x00,0x00,0x01,0x00,0x00,0x40,
	0x01,0x00,0x00,0x00,0x02,0x01,0x00,0x00,0x01,0x00,0x00,0x40,0x06,0x00,0x00,0x00,
	0x03,0x01,0x00,0x00,0x01,0x00,0x00,0x40,0x0B,0x00,0x00,0x00,0x00,0x02,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x02,0x00,0x00,0x02,0x00,0x00,0x60,
	0x01,0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x17,0x18,0x10,0x00,0x00,0x00,0x19,0x10,0x00,0x00,0x00,0x1A,0x10,0x00,0x00,0x00,
	0x1B
};

GTEST_TEST(GFF4StructStructsRef, getStruct) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4StructsRef));
	const Aurora::GFF4Struct &strct0 = gff4.getTopLevel();

	EXPECT_EQ(strct0.getLabel(), MKTAG('S', 'C', 'T', '1'));
	EXPECT_EQ(strct0.getUint(256), 23);

	const Aurora::GFF4Struct *strct1 = strct0.getStruct(257);
	const Aurora::GFF4Struct *strct2 = strct0.getStruct(258);
	const Aurora::GFF4Struct *strct3 = strct0.getStruct(259);

	ASSERT_NE(strct1, static_cast<const Aurora::GFF4Struct *>(0));
	EXPECT_EQ(strct1->getLabel(), MKTAG('S', 'C', 'T', '2'));
	EXPECT_EQ(strct1->getUint(512), 24);

	ASSERT_NE(strct2, static_cast<const Aurora::GFF4Struct *>(0));
	EXPECT_EQ(strct2->getLabel(), MKTAG('S', 'C', 'T', '2'));
	EXPECT_EQ(strct2->getUint(512), 25);

	ASSERT_NE(strct3, static_cast<const Aurora::GFF4Struct *>(0));
	EXPECT_EQ(strct3->getLabel(), MKTAG('S', 'C', 'T', '2'));
	EXPECT_EQ(strct3->getUint(512), 26);

	const Aurora::GFF4Struct *strct4 = strct1->getStruct(513);
	const Aurora::GFF4Struct *strct5 = strct2->getStruct(513);
	const Aurora::GFF4Struct *strct6 = strct3->getStruct(513);

	ASSERT_NE(strct4, static_cast<const Aurora::GFF4Struct *>(0));
	EXPECT_EQ(strct4->getLabel(), MKTAG('S', 'C', 'T', '3'));
	EXPECT_EQ(strct4->getUint(768), 27);

	ASSERT_NE(strct5, static_cast<const Aurora::GFF4Struct *>(0));
	EXPECT_EQ(strct5->getLabel(), MKTAG('S', 'C', 'T', '3'));
	EXPECT_EQ(strct5->getUint(768), 27);

	ASSERT_NE(strct6, static_cast<const Aurora::GFF4Struct *>(0));
	EXPECT_EQ(strct6->getLabel(), MKTAG('S', 'C', 'T', '3'));
	EXPECT_EQ(strct6->getUint(768), 27);

	EXPECT_EQ(strct4, strct5);
	EXPECT_EQ(strct4, strct6);

	EXPECT_EQ(strct4->getRefCount(), 3);
}

// --- GFF4, lists, with references ---

static const byte kGFF4ListsRef[] = {
	0x47,0x46,0x46,0x20,0x56,0x34,0x2E,0x30,0x50,0x43,0x20,0x20,0x54,0x45,0x53,0x54,
	0x56,0x31,0x2E,0x30,0x03,0x00,0x00,0x00,0x88,0x00,0x00,0x00,0x53,0x43,0x54,0x31,
	0x02,0x00,0x00,0x00,0x4C,0x00,0x00,0x00,0x3C,0x00,0x00,0x00,0x53,0x43,0x54,0x32,
	0x02,0x00,0x00,0x00,0x64,0x00,0x00,0x00,0x11,0x00,0x00,0x00,0x53,0x43,0x54,0x33,
	0x01,0x00,0x00,0x00,0x7C,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x01,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x00,0x00,0x01,0x00,0x00,0xC0,
	0x01,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x01,0x02,0x00,0x00,0x02,0x00,0x00,0xE0,0x01,0x00,0x00,0x00,0x00,0x03,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x17,0x05,0x00,0x00,0x00,0x03,0x00,0x00,
	0x00,0x18,0x0E,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x3C,0x00,0x00,0x00,0x3C,0x00,
	0x00,0x00,0x19,0x1F,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x3C,0x00,0x00,0x00,0x3C,
	0x00,0x00,0x00,0x1A,0x30,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x3C,0x00,0x00,0x00,
	0x3C,0x00,0x00,0x00,0x1B
};

GTEST_TEST(GFF4StructListsRef, getList) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4ListsRef));
	const Aurora::GFF4Struct &strct0 = gff4.getTopLevel();

	EXPECT_EQ(strct0.getLabel(), MKTAG('S', 'C', 'T', '1'));
	EXPECT_EQ(strct0.getUint(256), 23);

	const Aurora::GFF4List &list0 = strct0.getList(257);
	ASSERT_EQ(list0.size(), 3);

	ASSERT_NE(list0[0], static_cast<const Aurora::GFF4Struct *>(0));
	EXPECT_EQ(list0[0]->getLabel(), MKTAG('S', 'C', 'T', '2'));
	EXPECT_EQ(list0[0]->getUint(512), 24);

	ASSERT_NE(list0[1], static_cast<const Aurora::GFF4Struct *>(0));
	EXPECT_EQ(list0[1]->getLabel(), MKTAG('S', 'C', 'T', '2'));
	EXPECT_EQ(list0[1]->getUint(512), 25);

	ASSERT_NE(list0[2], static_cast<const Aurora::GFF4Struct *>(0));
	EXPECT_EQ(list0[2]->getLabel(), MKTAG('S', 'C', 'T', '2'));
	EXPECT_EQ(list0[2]->getUint(512), 26);

	const Aurora::GFF4List &list1 = list0[0]->getList(513);
	const Aurora::GFF4List &list2 = list0[1]->getList(513);
	const Aurora::GFF4List &list3 = list0[2]->getList(513);

	ASSERT_EQ(list1.size(), 2);
	ASSERT_NE(list1[0], static_cast<const Aurora::GFF4Struct *>(0));
	EXPECT_EQ(list1[0]->getLabel(), MKTAG('S', 'C', 'T', '3'));
	EXPECT_EQ(list1[0]->getUint(768), 27);
	ASSERT_NE(list1[1], static_cast<const Aurora::GFF4Struct *>(0));
	EXPECT_EQ(list1[1]->getLabel(), MKTAG('S', 'C', 'T', '3'));
	EXPECT_EQ(list1[1]->getUint(768), 27);

	ASSERT_EQ(list2.size(), 2);
	ASSERT_NE(list2[0], static_cast<const Aurora::GFF4Struct *>(0));
	EXPECT_EQ(list2[0]->getLabel(), MKTAG('S', 'C', 'T', '3'));
	EXPECT_EQ(list2[0]->getUint(768), 27);
	ASSERT_NE(list2[1], static_cast<const Aurora::GFF4Struct *>(0));
	EXPECT_EQ(list2[1]->getLabel(), MKTAG('S', 'C', 'T', '3'));
	EXPECT_EQ(list2[1]->getUint(768), 27);

	ASSERT_EQ(list3.size(), 2);
	ASSERT_NE(list3[0], static_cast<const Aurora::GFF4Struct *>(0));
	EXPECT_EQ(list3[0]->getLabel(), MKTAG('S', 'C', 'T', '3'));
	EXPECT_EQ(list3[0]->getUint(768), 27);
	ASSERT_NE(list3[1], static_cast<const Aurora::GFF4Struct *>(0));
	EXPECT_EQ(list3[1]->getLabel(), MKTAG('S', 'C', 'T', '3'));
	EXPECT_EQ(list3[1]->getUint(768), 27);

	EXPECT_EQ(list1[0], list1[1]);
	EXPECT_EQ(list1[0], list2[0]);
	EXPECT_EQ(list1[0], list2[1]);
	EXPECT_EQ(list1[0], list3[0]);
	EXPECT_EQ(list1[0], list3[1]);

	EXPECT_EQ(list1[0]->getRefCount(), 6);
}

// --- GFF4, generics ---

static const byte kGFF4Generic[] = {
	0x47,0x46,0x46,0x20,0x56,0x34,0x2E,0x30,0x50,0x43,0x20,0x20,0x54,0x45,0x53,0x54,
	0x56,0x31,0x2E,0x30,0x02,0x00,0x00,0x00,0x6C,0x00,0x00,0x00,0x53,0x43,0x54,0x31,
	0x03,0x00,0x00,0x00,0x3C,0x00,0x00,0x00,0x0E,0x00,0x00,0x00,0x53,0x43,0x54,0x32,
	0x01,0x00,0x00,0x00,0x60,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x01,0x00,0x00,
	0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x00,0x00,0xFF,0xFF,0x00,0x00,
	0x05,0x00,0x00,0x00,0x02,0x01,0x00,0x00,0xFF,0xFF,0x00,0xA0,0x0A,0x00,0x00,0x00,
	0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x17,0x01,0x00,0x00,0x40,0x18,0x0E,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x2A,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2B,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x2C,0x00,0x00,0x00,0x19,0x1A,0x1B
};

GTEST_TEST(GFF4StructGeneric, getGeneric) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4Generic));
	const Aurora::GFF4Struct &strct0 = gff4.getTopLevel();

	EXPECT_EQ(strct0.getLabel(), MKTAG('S', 'C', 'T', '1'));

	const Aurora::GFF4Struct *generic1 = strct0.getGeneric(256);
	ASSERT_NE(generic1, static_cast<const Aurora::GFF4Struct *>(0));
	EXPECT_EQ(generic1->getUint(0), 23);

	const Aurora::GFF4Struct *generic2 = strct0.getGeneric(257);
	ASSERT_NE(generic2, static_cast<const Aurora::GFF4Struct *>(0));

	const Aurora::GFF4Struct *strct1 = generic2->getStruct(0);
	ASSERT_NE(strct1, static_cast<const Aurora::GFF4Struct *>(0));
	EXPECT_EQ(strct1->getUint(512), 24);

	const Aurora::GFF4Struct *generic3 = strct0.getGeneric(258);
	ASSERT_NE(generic3, static_cast<const Aurora::GFF4Struct *>(0));
	EXPECT_EQ(generic3->getUint(0), 25);
	EXPECT_EQ(generic3->getUint(1), 26);
	EXPECT_EQ(generic3->getUint(2), 27);

	EXPECT_THROW(generic1->getGeneric(0), Common::Exception);
}

// --- GFF4, shared strings ---

static const byte kGFF4Shared[] = {
	0x47,0x46,0x46,0x20,0x56,0x34,0x2E,0x31,0x50,0x43,0x20,0x20,0x54,0x45,0x53,0x54,
	0x56,0x31,0x2E,0x30,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x58,0x00,0x00,0x00,
	0x4C,0x00,0x00,0x00,0x53,0x54,0x43,0x54,0x02,0x00,0x00,0x00,0x34,0x00,0x00,0x00,
	0x0C,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x0E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x01,0x01,0x00,0x00,0x11,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x17,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x46,0x6F,0x6F,0x62,0x61,0x72,0x00
};

GTEST_TEST(GFF4StructShared, getString) {
	Aurora::GFF4File gff4(new Common::MemoryReadStream(kGFF4Shared));
	const Aurora::GFF4Struct &strct0 = gff4.getTopLevel();

	EXPECT_STREQ(strct0.getString(256).c_str(), "Foobar");

	uint32_t strRef;
	Common::UString tlkString;
	EXPECT_TRUE(strct0.getTalkString(257, strRef, tlkString));

	EXPECT_EQ(strRef, 23);
	EXPECT_STREQ(tlkString.c_str(), "Foobar");
}
