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
 *  Unit tests for our TalkTable_TLK class.
 */

#include "gtest/gtest.h"

#include "src/common/util.h"
#include "src/common/encoding.h"
#include "src/common/memreadstream.h"

#include "src/aurora/types.h"
#include "src/aurora/talktable.h"
#include "src/aurora/talktable_tlk.h"

// --- TLK V3.0 ---

static const byte kTLKV30[] = {
	0x54,0x4C,0x4B,0x20,0x56,0x33,0x2E,0x30,0x17,0x00,0x00,0x00,0x03,0x00,0x00,0x00,
	0x8C,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x03,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x00,0x00,
	0x71,0x75,0x75,0x78,0x5F,0x73,0x6E,0x64,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x02,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xCD,0xCC,0x8C,0x3F,0x07,0x00,0x00,0x00,0x62,0x61,0x72,0x66,0x6F,0x6F,0x5F,0x73,
	0x6E,0x64,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x03,0x00,0x00,0x00,
	0x06,0x00,0x00,0x00,0x06,0x00,0x00,0x00,0x9A,0x99,0x99,0x3F,0x46,0x6F,0x6F,0x62,
	0x61,0x72,0x42,0x61,0x72,0x66,0x6F,0x6F
};

GTEST_TEST(TalkTable_TLK30, getLanguageID) {
	Common::MemoryReadStream *stream = new Common::MemoryReadStream(kTLKV30);
	Aurora::TalkTable_TLK tlk(stream, Common::kEncodingUTF8);

	EXPECT_EQ(tlk.getLanguageID(), 23);
}

GTEST_TEST(TalkTable_TLK30, getLanguageIDStatic) {
	Common::MemoryReadStream stream(kTLKV30);

	EXPECT_EQ(Aurora::TalkTable_TLK::getLanguageID(stream), 23);
}

GTEST_TEST(TalkTable_TLK30, hasEntry) {
	Common::MemoryReadStream *stream = new Common::MemoryReadStream(kTLKV30);
	Aurora::TalkTable_TLK tlk(stream, Common::kEncodingUTF8);

	EXPECT_TRUE(tlk.hasEntry(0));
	EXPECT_TRUE(tlk.hasEntry(1));
	EXPECT_TRUE(tlk.hasEntry(2));

	EXPECT_FALSE(tlk.hasEntry(3));
}

GTEST_TEST(TalkTable_TLK30, getString) {
	Common::MemoryReadStream *stream = new Common::MemoryReadStream(kTLKV30);
	Aurora::TalkTable_TLK tlk(stream, Common::kEncodingUTF8);

	EXPECT_STREQ(tlk.getString(0).c_str(), "Foobar");
	EXPECT_STREQ(tlk.getString(1).c_str(), "");
	EXPECT_STREQ(tlk.getString(2).c_str(), "Barfoo");

	EXPECT_STREQ(tlk.getString(3).c_str(), "");
	EXPECT_STREQ(tlk.getString(5000).c_str(), "");
}

GTEST_TEST(TalkTable_TLK30, getSoundResRef) {
	Common::MemoryReadStream *stream = new Common::MemoryReadStream(kTLKV30);
	Aurora::TalkTable_TLK tlk(stream, Common::kEncodingUTF8);

	EXPECT_STREQ(tlk.getSoundResRef(0).c_str(), "");
	EXPECT_STREQ(tlk.getSoundResRef(1).c_str(), "quux_snd");
	EXPECT_STREQ(tlk.getSoundResRef(2).c_str(), "barfoo_snd");

	EXPECT_STREQ(tlk.getSoundResRef(3).c_str(), "");
	EXPECT_STREQ(tlk.getSoundResRef(5000).c_str(), "");
}

GTEST_TEST(TalkTable_TLK30, getSoundID) {
	Common::MemoryReadStream *stream = new Common::MemoryReadStream(kTLKV30);
	Aurora::TalkTable_TLK tlk(stream, Common::kEncodingUTF8);

	EXPECT_EQ(tlk.getSoundID(0), Aurora::kFieldIDInvalid);
	EXPECT_EQ(tlk.getSoundID(1), Aurora::kFieldIDInvalid);
	EXPECT_EQ(tlk.getSoundID(2), Aurora::kFieldIDInvalid);

	EXPECT_EQ(tlk.getSoundID(3), Aurora::kFieldIDInvalid);
	EXPECT_EQ(tlk.getSoundID(5000), Aurora::kFieldIDInvalid);
}

GTEST_TEST(TalkTable_TLK30, fromGeneric) {
	Common::MemoryReadStream *stream = new Common::MemoryReadStream(kTLKV30);

	Aurora::TalkTable *tlk = Aurora::TalkTable::load(stream, Common::kEncodingUTF8);
	EXPECT_NE(dynamic_cast<Aurora::TalkTable_TLK *>(tlk), static_cast<Aurora::TalkTable_TLK *>(0));

	delete tlk;
}

// --- TLK V4.0 ---

static const byte kTLKV40[] = {
	0x54,0x4C,0x4B,0x20,0x56,0x34,0x2E,0x30,0x17,0x00,0x00,0x00,0x03,0x00,0x00,0x00,
	0x18,0x00,0x00,0x00,0x36,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x36,0x00,0x00,0x00,
	0x06,0x00,0x05,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x07,0x00,0x00,0x00,
	0x3C,0x00,0x00,0x00,0x06,0x00,0x46,0x6F,0x6F,0x62,0x61,0x72,0x42,0x61,0x72,0x66,
	0x6F,0x6F
};

GTEST_TEST(TalkTable_TLK40, getLanguageID) {
	Common::MemoryReadStream *stream = new Common::MemoryReadStream(kTLKV40);
	Aurora::TalkTable_TLK tlk(stream, Common::kEncodingUTF8);

	EXPECT_EQ(tlk.getLanguageID(), 23);
}

GTEST_TEST(TalkTable_TLK40, getLanguageIDStatic) {
	Common::MemoryReadStream stream(kTLKV40);

	EXPECT_EQ(Aurora::TalkTable_TLK::getLanguageID(stream), 23);
}

GTEST_TEST(TalkTable_TLK40, hasEntry) {
	Common::MemoryReadStream *stream = new Common::MemoryReadStream(kTLKV40);
	Aurora::TalkTable_TLK tlk(stream, Common::kEncodingUTF8);

	EXPECT_TRUE(tlk.hasEntry(0));
	EXPECT_TRUE(tlk.hasEntry(1));
	EXPECT_TRUE(tlk.hasEntry(2));

	EXPECT_FALSE(tlk.hasEntry(3));
}

GTEST_TEST(TalkTable_TLK40, getString) {
	Common::MemoryReadStream *stream = new Common::MemoryReadStream(kTLKV40);
	Aurora::TalkTable_TLK tlk(stream, Common::kEncodingUTF8);

	EXPECT_STREQ(tlk.getString(0).c_str(), "Foobar");
	EXPECT_STREQ(tlk.getString(1).c_str(), "");
	EXPECT_STREQ(tlk.getString(2).c_str(), "Barfoo");

	EXPECT_STREQ(tlk.getString(3).c_str(), "");
	EXPECT_STREQ(tlk.getString(5000).c_str(), "");
}

GTEST_TEST(TalkTable_TLK40, getSoundResRef) {
	Common::MemoryReadStream *stream = new Common::MemoryReadStream(kTLKV40);
	Aurora::TalkTable_TLK tlk(stream, Common::kEncodingUTF8);

	EXPECT_STREQ(tlk.getSoundResRef(0).c_str(), "");
	EXPECT_STREQ(tlk.getSoundResRef(1).c_str(), "");
	EXPECT_STREQ(tlk.getSoundResRef(2).c_str(), "");

	EXPECT_STREQ(tlk.getSoundResRef(3).c_str(), "");
	EXPECT_STREQ(tlk.getSoundResRef(5000).c_str(), "");
}

GTEST_TEST(TalkTable_TLK40, getSoundID) {
	Common::MemoryReadStream *stream = new Common::MemoryReadStream(kTLKV40);
	Aurora::TalkTable_TLK tlk(stream, Common::kEncodingUTF8);

	EXPECT_EQ(tlk.getSoundID(0), Aurora::kFieldIDInvalid);
	EXPECT_EQ(tlk.getSoundID(1), 5);
	EXPECT_EQ(tlk.getSoundID(2), 7);

	EXPECT_EQ(tlk.getSoundID(3), Aurora::kFieldIDInvalid);
	EXPECT_EQ(tlk.getSoundID(5000), Aurora::kFieldIDInvalid);
}

GTEST_TEST(TalkTable_TLK40, fromGeneric) {
	Common::MemoryReadStream *stream = new Common::MemoryReadStream(kTLKV40);

	Aurora::TalkTable *tlk = Aurora::TalkTable::load(stream, Common::kEncodingUTF8);
	EXPECT_NE(dynamic_cast<Aurora::TalkTable_TLK *>(tlk), static_cast<Aurora::TalkTable_TLK *>(0));

	delete tlk;
}
