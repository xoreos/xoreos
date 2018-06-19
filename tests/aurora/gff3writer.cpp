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
 *  Unit tests for our GFF3 file writer class.
 */

#include <vector>

#include "gtest/gtest.h"

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/memreadstream.h"
#include "src/common/memwritestream.h"
#include "src/common/writefile.h"

#include "src/aurora/gff3writer.h"
#include "src/aurora/gff3file.h"
#include "src/aurora/locstring.h"

GTEST_TEST(GFF3Writer, WriteEmptyStruct) {
	Aurora::GFF3Writer writer(MKTAG('G', 'F', 'F', ' '), MKTAG('V', '3', '.', '2'));
	writer.getTopLevel()->addStruct("EmptyStruct");

	Common::MemoryWriteStreamDynamic *writeStream = new Common::MemoryWriteStreamDynamic(true);
	writer.write(*writeStream);

	Aurora::GFF3File gff(new Common::MemoryReadStream(writeStream->getData(), writeStream->size()));

	EXPECT_TRUE(gff.getTopLevel().hasField("EmptyStruct"));
	EXPECT_EQ(gff.getID(), MKTAG('G', 'F', 'F', ' '));

	delete writeStream;
}

GTEST_TEST(GFF3Writer, WriteMultipleEmptyStructs) {
	Aurora::GFF3Writer writer(MKTAG('G', 'F', 'F', ' '));
	writer.getTopLevel()->addStruct("EmptyStruct1");
	writer.getTopLevel()->addStruct("EmptyStruct2");
	Aurora::GFF3WriterStructPtr strct = writer.getTopLevel()->addStruct("EmptyStruct3");
	strct->addStruct("EmptyStruct3_1");

	Common::MemoryWriteStreamDynamic *writeStream = new Common::MemoryWriteStreamDynamic(true);
	writer.write(*writeStream);

	Aurora::GFF3File gff(new Common::MemoryReadStream(writeStream->getData(), writeStream->size()));

	EXPECT_EQ(gff.getID(), MKTAG('G', 'F', 'F', ' '));

	EXPECT_TRUE(gff.getTopLevel().hasField("EmptyStruct1"));
	EXPECT_TRUE(gff.getTopLevel().hasField("EmptyStruct2"));
	EXPECT_TRUE(gff.getTopLevel().hasField("EmptyStruct3"));
	EXPECT_TRUE(gff.getTopLevel().getStruct("EmptyStruct3").hasField("EmptyStruct3_1"));

	delete writeStream;
}

GTEST_TEST(GFF3Writer, WriteSingleStruct) {
	LangMan.addLanguage(Aurora::kLanguageEnglish, 0, Common::kEncodingASCII);
	LangMan.addLanguage(Aurora::kLanguageGerman, 1, Common::kEncodingASCII);

	Aurora::GFF3Writer writer(MKTAG('G', 'F', 'F', ' '));
	writer.getTopLevel()->addByte("FieldByte", 8);
	writer.getTopLevel()->addChar("FieldChar", 'C');
	writer.getTopLevel()->addUint16("FieldUint16", 16);
	writer.getTopLevel()->addUint32("FieldUint32", 33);
	writer.getTopLevel()->addUint64("FieldUint64", 5000000000);
	writer.getTopLevel()->addSint16("FieldSint16", -16);
	writer.getTopLevel()->addSint32("FieldSint32", 34);
	writer.getTopLevel()->addSint64("FieldSint64", -5000000000);
	writer.getTopLevel()->addFloat("FieldFloat", 34.5f);
	writer.getTopLevel()->addDouble("FieldDouble", 34.55);
	writer.getTopLevel()->addExoString("FieldExoString", "NiceString");
	writer.getTopLevel()->addStrRef("FieldStrRef", 20);
	writer.getTopLevel()->addResRef("FieldResRef", "file.txt");
	writer.getTopLevel()->addVoid("FieldVoid", reinterpret_cast<const byte *>("![DATA]!"), 8);
	writer.getTopLevel()->addVector("FieldVector", glm::vec3(1.0f, 2.0f, 2.5f));
	writer.getTopLevel()->addOrientation("FieldOrientation", glm::vec4(1.0f, 4.0f, 2.5f, 1.5f));

	Aurora::LocString locString;
	locString.setString(Aurora::kLanguageEnglish, Aurora::kLanguageGenderMale, "Localized Test String");
	locString.setString(Aurora::kLanguageGerman, Aurora::kLanguageGenderMale, "Lokalisierter Test String");
	writer.getTopLevel()->addLocString("FieldLocString", locString);

	Common::MemoryWriteStreamDynamic *writeStream = new Common::MemoryWriteStreamDynamic(true);
	writer.write(*writeStream);

	Aurora::GFF3File gff(new Common::MemoryReadStream(writeStream->getData(), writeStream->size()));

	EXPECT_EQ(gff.getID(), MKTAG('G', 'F', 'F', ' '));
	EXPECT_EQ(gff.getVersion(), MKTAG('V', '3', '.', '2'));

	EXPECT_TRUE(gff.getTopLevel().hasField("FieldByte"));
	EXPECT_TRUE(gff.getTopLevel().hasField("FieldChar"));
	EXPECT_TRUE(gff.getTopLevel().hasField("FieldUint16"));
	EXPECT_TRUE(gff.getTopLevel().hasField("FieldUint32"));
	EXPECT_TRUE(gff.getTopLevel().hasField("FieldUint64"));
	EXPECT_TRUE(gff.getTopLevel().hasField("FieldSint16"));
	EXPECT_TRUE(gff.getTopLevel().hasField("FieldSint32"));
	EXPECT_TRUE(gff.getTopLevel().hasField("FieldSint64"));
	EXPECT_TRUE(gff.getTopLevel().hasField("FieldFloat"));
	EXPECT_TRUE(gff.getTopLevel().hasField("FieldDouble"));
	EXPECT_TRUE(gff.getTopLevel().hasField("FieldExoString"));
	EXPECT_TRUE(gff.getTopLevel().hasField("FieldStrRef"));
	EXPECT_TRUE(gff.getTopLevel().hasField("FieldResRef"));
	EXPECT_TRUE(gff.getTopLevel().hasField("FieldVoid"));
	EXPECT_TRUE(gff.getTopLevel().hasField("FieldVector"));
	EXPECT_TRUE(gff.getTopLevel().hasField("FieldOrientation"));

	EXPECT_EQ(gff.getTopLevel().getUint("FieldByte"), 8);
	EXPECT_EQ(gff.getTopLevel().getChar("FieldChar"), 'C');
	EXPECT_EQ(gff.getTopLevel().getUint("FieldUint16"), 16);
	EXPECT_EQ(gff.getTopLevel().getUint("FieldUint32"), 33);
	EXPECT_EQ(gff.getTopLevel().getUint("FieldUint64"), 5000000000);
	EXPECT_EQ(gff.getTopLevel().getSint("FieldSint16"), -16);
	EXPECT_EQ(gff.getTopLevel().getSint("FieldSint32"), 34);
	EXPECT_EQ(gff.getTopLevel().getSint("FieldSint64"), -5000000000);
	EXPECT_EQ(gff.getTopLevel().getDouble("FieldFloat"), 34.5f);
	EXPECT_EQ(gff.getTopLevel().getDouble("FieldDouble"), 34.55);
	EXPECT_EQ(gff.getTopLevel().getUint("FieldStrRef"), 20);
	EXPECT_EQ(gff.getTopLevel().getString("FieldResRef"), "file.txt");
	EXPECT_STREQ(gff.getTopLevel().getString("FieldExoString").c_str(), "NiceString");

	Common::SeekableReadStream *data = gff.getTopLevel().getData("FieldVoid");
	char voidData[8];
	data->read(voidData, 8);
	EXPECT_STREQ(Common::UString(voidData, 8).c_str(), "![DATA]!");
	delete data;

	float x, y, z, w;
	gff.getTopLevel().getVector("FieldVector", x, y, z);
	EXPECT_EQ(x, 1.0f);
	EXPECT_EQ(y, 2.0f);
	EXPECT_EQ(z, 2.5f);

	gff.getTopLevel().getOrientation("FieldOrientation", x, y, z, w);
	EXPECT_EQ(x, 1.0f);
	EXPECT_EQ(y, 4.0f);
	EXPECT_EQ(z, 2.5f);
	EXPECT_EQ(w, 1.5f);

	Aurora::LocString locString2;
	gff.getTopLevel().getLocString("FieldLocString", locString2);
	EXPECT_STREQ(locString2.getString(Aurora::kLanguageEnglish).c_str(), "Localized Test String");
	EXPECT_STREQ(locString2.getString(Aurora::kLanguageGerman).c_str(), "Lokalisierter Test String");

	delete writeStream;
}

GTEST_TEST(GFF3Writer, WriteList) {
	Aurora::GFF3Writer writer(MKTAG('G', 'F', 'F', ' '));
	Aurora::GFF3WriterListPtr list1 = writer.getTopLevel()->addList("FieldList1");
	list1->addStruct("EmptyStruct1");
	list1->addStruct("EmptyStruct2");
	Aurora::GFF3WriterStructPtr empty3 = list1->addStruct("EmptyStruct3");
	Aurora::GFF3WriterListPtr list2 = empty3->addList("FieldList2");
	list2->addStruct("EmptyStruct4");

	Common::MemoryWriteStreamDynamic *writeStream = new Common::MemoryWriteStreamDynamic(true);
	writer.write(*writeStream);

	Aurora::GFF3File gff(new Common::MemoryReadStream(writeStream->getData(), writeStream->size()));

	EXPECT_EQ(gff.getID(), MKTAG('G', 'F', 'F', ' '));
	EXPECT_EQ(gff.getVersion(), MKTAG('V', '3', '.', '2'));

	EXPECT_TRUE(gff.getTopLevel().hasField("FieldList1"));
	EXPECT_EQ(gff.getTopLevel().getList("FieldList1").size(), 3);
	EXPECT_TRUE(gff.getTopLevel().getList("FieldList1")[2]->hasField("FieldList2"));
	EXPECT_EQ(gff.getTopLevel().getList("FieldList1")[2]->getList("FieldList2").size(), 1);

	delete writeStream;
}

GTEST_TEST(GFF3Writer, WriteNestedStructs) {
	Aurora::GFF3Writer writer(MKTAG('G', 'F', 'F', ' '));
	Aurora::GFF3WriterStructPtr struct1 = writer.getTopLevel()->addStruct("Struct1");
	Aurora::GFF3WriterStructPtr struct2 = writer.getTopLevel()->addStruct("Struct2");
	Aurora::GFF3WriterStructPtr struct3 = struct2->addStruct("Struct3");

	struct3->addUint64("FieldUint64_1", 214453251);
	struct3->addUint64("FieldUint64_2", 343251);

	struct1->addSint16("FieldSint16", 1);
	struct1->addSint32("FieldSint32", 32);

	struct2->addSint16("FieldSint16_1", 3);
	struct2->addSint16("FieldSint16_2", -3);

	Common::MemoryWriteStreamDynamic *writeStream = new Common::MemoryWriteStreamDynamic(true);
	writer.write(*writeStream);

	Aurora::GFF3File gff(new Common::MemoryReadStream(writeStream->getData(), writeStream->size()));

	EXPECT_EQ(gff.getID(), MKTAG('G', 'F', 'F', ' '));
	EXPECT_EQ(gff.getVersion(), MKTAG('V', '3', '.', '2'));

	EXPECT_TRUE(gff.getTopLevel().hasField("Struct1"));
	EXPECT_TRUE(gff.getTopLevel().getStruct("Struct1").hasField("FieldSint16"));
	EXPECT_TRUE(gff.getTopLevel().getStruct("Struct1").hasField("FieldSint32"));
	EXPECT_TRUE(gff.getTopLevel().hasField("Struct2"));
	EXPECT_TRUE(gff.getTopLevel().getStruct("Struct2").hasField("FieldSint16_1"));
	EXPECT_TRUE(gff.getTopLevel().getStruct("Struct2").hasField("FieldSint16_2"));
	EXPECT_TRUE(gff.getTopLevel().getStruct("Struct2").hasField("Struct3"));
	EXPECT_TRUE(gff.getTopLevel().getStruct("Struct2").getStruct("Struct3").hasField("FieldUint64_1"));
	EXPECT_TRUE(gff.getTopLevel().getStruct("Struct2").getStruct("Struct3").hasField("FieldUint64_2"));

	EXPECT_EQ(gff.getTopLevel().getStruct("Struct1").getSint("FieldSint16"), 1);
	EXPECT_EQ(gff.getTopLevel().getStruct("Struct1").getSint("FieldSint32"), 32);
	EXPECT_EQ(gff.getTopLevel().getStruct("Struct2").getSint("FieldSint16_1"), 3);
	EXPECT_EQ(gff.getTopLevel().getStruct("Struct2").getSint("FieldSint16_2"), -3);
	EXPECT_EQ(gff.getTopLevel().getStruct("Struct2").getStruct("Struct3").getUint("FieldUint64_1"), 214453251);
	EXPECT_EQ(gff.getTopLevel().getStruct("Struct2").getStruct("Struct3").getUint("FieldUint64_2"), 343251);

	delete writeStream;
}
