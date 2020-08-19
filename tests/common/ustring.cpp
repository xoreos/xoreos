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
 *  Unit tests for our UString class.
 */

#include "gtest/gtest.h"

#include "src/common/util.h"
#include "src/common/ustring.h"

static const char kTestString1[] = "Foobar Barfoo";
static const char kTestString2[] = "Barfoo Foobar";

static const char kTestStringSub1[] = { kTestString1[0], kTestString1[1], kTestString1[2], 0 };

static const char kTestStringLower1[] = "foobar barfoo";
static const char kTestStringUpper1[] = "FOOBAR BARFOO";

// Foobar, o and a with diaereses
static const byte   kTestStringUTF8 [10] = { 'F', 0xC3, 0xB6, 0xC3, 0xB6, 'b', 0xC3, 0xA4, 'r', 0 };
static const uint32_t kTestStringUTF32[ 7] = { 'F', 0xF6,       0xF6,       'b', 0xE4,       'r', 0 };

// FOOBAR, O and A with diaereses
static const byte kTestStringUpperUTF8[10] = { 'F', 0xC3, 0x96, 0xC3, 0x96, 'B', 0xC3, 0x84, 'R', 0 };

GTEST_TEST(UString, constructorDefault) {
	const Common::UString str;

	EXPECT_STREQ(str.c_str(), "");

	EXPECT_TRUE(str.empty());
	EXPECT_EQ(str.size(), 0);

	EXPECT_EQ(str.begin(), str.end());
}

GTEST_TEST(UString, constructorCString) {
	const Common::UString str(kTestString1);

	EXPECT_STREQ(str.c_str(), kTestString1);
	EXPECT_STRNE(str.c_str(), kTestString2);

	EXPECT_FALSE(str.empty());
	ASSERT_EQ(str.size(), ARRAYSIZE(kTestString1) - 1);

	EXPECT_EQ(*str.begin(), kTestString1[0]);
	EXPECT_EQ(*(--str.end()), kTestString1[ARRAYSIZE(kTestString1) - 2]);
}

GTEST_TEST(UString, constructorCopy) {
	const Common::UString str1(kTestString1);
	const Common::UString str2(str1);
	const Common::UString str3 = str1;

	EXPECT_EQ(str1, str2);
	EXPECT_EQ(str1, str3);

	EXPECT_STREQ(str1.c_str(), str2.c_str());
	EXPECT_STREQ(str1.c_str(), str3.c_str());
}

GTEST_TEST(UString, constructorCopyLength) {
	const Common::UString str(kTestString1, ARRAYSIZE(kTestStringSub1) - 1);

	EXPECT_EQ(str.size(), ARRAYSIZE(kTestStringSub1) - 1);
	EXPECT_STREQ(str.c_str(), kTestStringSub1);
}

GTEST_TEST(UString, constructorNCopies) {
	const Common::UString str('x', 5);

	EXPECT_EQ(str.size(), 5);
	EXPECT_STREQ(str.c_str(), "xxxxx");
}

GTEST_TEST(UString, constructorCopyIterators) {
	const Common::UString str1(kTestString1);
	const Common::UString str2(str1.begin(), str1.end());

	EXPECT_EQ(str1, str2);
	EXPECT_STREQ(str1.c_str(), str2.c_str());
}

GTEST_TEST(UString, iteratorsASCII) {
	const Common::UString str(kTestString1);

	ASSERT_EQ(str.size(), ARRAYSIZE(kTestString1) - 1);

	Common::UString::iterator it = str.begin();

	for (size_t i = 0; i < (ARRAYSIZE(kTestString1) - 1); i++, ++it) {
		EXPECT_NE(it, str.end()) << "At index " << i;
		EXPECT_EQ(*it, kTestString1[i]) << "At index " << i;
	}

	EXPECT_EQ(it, str.end());
}

GTEST_TEST(UString, iteratorsUTF8) {
	const Common::UString str(reinterpret_cast<const char *>(kTestStringUTF8));

	ASSERT_EQ(str.size(), ARRAYSIZE(kTestStringUTF32) - 1);

	Common::UString::iterator it = str.begin();

	for (size_t i = 0; i < (ARRAYSIZE(kTestStringUTF32) - 1); i++, ++it) {
		EXPECT_NE(it, str.end()) << "At index " << i;
		EXPECT_EQ(*it, kTestStringUTF32[i]) << "At index " << i;
	}

	EXPECT_EQ(it, str.end());
}

GTEST_TEST(UString, caseInsensitiveASCII) {
	const Common::UString str(kTestString1);

	EXPECT_TRUE(str.equalsIgnoreCase(kTestStringLower1));
	EXPECT_TRUE(str.equalsIgnoreCase(kTestStringUpper1));
}

GTEST_TEST(UString, caseInsensitiveUTF8) {
	const Common::UString str1(reinterpret_cast<const char *>(kTestStringUTF8));
	const Common::UString str2(reinterpret_cast<const char *>(kTestStringUpperUTF8));

	// TODO: We don't support case insensitive comparison of non-ASCII strings!
	EXPECT_FALSE(str1.equalsIgnoreCase(str2));
}

GTEST_TEST(UString, clear) {
	Common::UString str(kTestString1);

	EXPECT_FALSE(str.empty());
	EXPECT_NE(str.size(), 0);

	str.clear();

	EXPECT_TRUE(str.empty());
	EXPECT_EQ(str.size(), 0);
}

GTEST_TEST(UString, swap) {
	Common::UString str1(kTestString1);
	Common::UString str2(kTestString2);

	str1.swap(str2);

	EXPECT_STREQ(str1.c_str(), kTestString2);
	EXPECT_STREQ(str2.c_str(), kTestString1);
}

GTEST_TEST(UString, findFirstChar) {
	const Common::UString str("Foobar Barfoo");

	Common::UString::iterator f = str.findFirst('F');
	Common::UString::iterator b = str.findFirst('B');
	Common::UString::iterator o = str.findFirst('o');
	Common::UString::iterator q = str.findFirst('Q');

	ASSERT_NE(f, str.end());
	ASSERT_NE(b, str.end());
	ASSERT_NE(o, str.end());
	EXPECT_EQ(q, str.end());

	EXPECT_EQ(f, str.begin());
	EXPECT_NE(b, str.begin());
	EXPECT_NE(o, str.begin());

	EXPECT_EQ(*f, 'F');
	EXPECT_EQ(*b, 'B');
	EXPECT_EQ(*o, 'o');

	ASSERT_NE(++f, str.end());
	ASSERT_NE(++b, str.end());
	ASSERT_NE(++o, str.end());

	EXPECT_EQ(*f, 'o');
	EXPECT_EQ(*b, 'a');
	EXPECT_EQ(*o, 'o');

	ASSERT_NE(++o, str.end());
	EXPECT_EQ(*o, 'b');
}

GTEST_TEST(UString, findFirstStr) {
	const Common::UString str("Foobar Barfoo");

	Common::UString::iterator foo = str.findFirst("Foo");
	Common::UString::iterator bar = str.findFirst("Bar");
	Common::UString::iterator oo  = str.findFirst("oo");
	Common::UString::iterator qux = str.findFirst("Qux");

	ASSERT_NE(foo, str.end());
	ASSERT_NE(bar, str.end());
	ASSERT_NE(oo , str.end());
	EXPECT_EQ(qux, str.end());

	EXPECT_EQ(foo, str.begin());
	EXPECT_NE(oo , str.begin());
	EXPECT_NE(bar, str.begin());

	EXPECT_EQ(*foo, 'F');
	EXPECT_EQ(*bar, 'B');
	EXPECT_EQ(*oo , 'o');

	ASSERT_NE(++foo, str.end());
	ASSERT_NE(++bar, str.end());
	ASSERT_NE(++oo , str.end());

	EXPECT_EQ(*foo, 'o');
	EXPECT_EQ(*bar, 'a');
	EXPECT_EQ(*oo , 'o');

	ASSERT_NE(++oo, str.end());
	EXPECT_EQ(*oo, 'b');
}

GTEST_TEST(UString, findLastChar) {
	const Common::UString str("Foobar Barfoo");

	Common::UString::iterator f = str.findLast('F');
	Common::UString::iterator b = str.findLast('B');
	Common::UString::iterator o = str.findLast('o');
	Common::UString::iterator q = str.findLast('Q');

	ASSERT_NE(f, str.end());
	ASSERT_NE(b, str.end());
	ASSERT_NE(o, str.end());
	EXPECT_EQ(q, str.end());

	EXPECT_EQ(f, str.begin());
	EXPECT_NE(b, str.begin());
	EXPECT_NE(o, str.begin());

	EXPECT_EQ(*f, 'F');
	EXPECT_EQ(*b, 'B');
	EXPECT_EQ(*o, 'o');

	ASSERT_NE(++f, str.end());
	ASSERT_NE(++b, str.end());
	EXPECT_EQ(++o, str.end());

	EXPECT_EQ(*f, 'o');
	EXPECT_EQ(*b, 'a');
}

GTEST_TEST(UString, beginsWith) {
	const Common::UString str("Foobar Barfoo");

	EXPECT_TRUE (str.beginsWith("Foo"));
	EXPECT_FALSE(str.beginsWith("Bar"));
}

GTEST_TEST(UString, endsWith) {
	const Common::UString str("Foobar Barfoo");

	EXPECT_TRUE (str.endsWith("foo"));
	EXPECT_FALSE(str.endsWith("bar"));
}

GTEST_TEST(UString, containsChar) {
	const Common::UString str("Foobar Barfoo");

	EXPECT_TRUE (str.contains('F'));
	EXPECT_TRUE (str.contains('f'));
	EXPECT_TRUE (str.contains('B'));
	EXPECT_TRUE (str.contains('b'));
	EXPECT_FALSE(str.contains('Q'));
}

GTEST_TEST(UString, containsString) {
	const Common::UString str("Foobar Barfoo");

	EXPECT_TRUE (str.contains("Foo"));
	EXPECT_TRUE (str.contains("foo"));
	EXPECT_TRUE (str.contains("Bar"));
	EXPECT_TRUE (str.contains("bar"));
	EXPECT_FALSE(str.contains("Qux"));
}

GTEST_TEST(UString, truncateInt) {
	Common::UString str("Foobar Barfoo");

	str.truncate(6);

	EXPECT_FALSE(str.empty());
	EXPECT_EQ(str.size(), 6);
	EXPECT_STREQ(str.c_str(), "Foobar");

	str.truncate(0);

	EXPECT_TRUE(str.empty());
	EXPECT_EQ(str.size(), 0);
	EXPECT_STREQ(str.c_str(), "");
}

GTEST_TEST(UString, truncateIterator) {
	Common::UString str("Foobar Barfoo");

	str.truncate(str.findFirst(' '));

	EXPECT_FALSE(str.empty());
	EXPECT_EQ(str.size(), 6);
	EXPECT_STREQ(str.c_str(), "Foobar");

	str.truncate(str.begin());

	EXPECT_TRUE(str.empty());
	EXPECT_EQ(str.size(), 0);
	EXPECT_STREQ(str.c_str(), "");
}

GTEST_TEST(UString, trim) {
	Common::UString str1("  Foobar  ");
	Common::UString str2("  Foobar  ");
	Common::UString str3("  Foobar  ");

	str1.trimLeft();
	str2.trimRight();
	str3.trim();

	EXPECT_STREQ(str1.c_str(), "Foobar  ");
	EXPECT_STREQ(str2.c_str(), "  Foobar");
	EXPECT_STREQ(str3.c_str(), "Foobar");
}

GTEST_TEST(UString, replaceAllChars) {
	Common::UString str("Foobar Barfoo");

	str.replaceAll('o', 'x');

	EXPECT_STREQ(str.c_str(), "Fxxbar Barfxx");
}

GTEST_TEST(UString, replaceAllSubstrings) {
	Common::UString str("Foobar Barfoo");

	str.replaceAll("ar", "ay");

	EXPECT_STREQ(str.c_str(), "Foobay Bayfoo");
}

GTEST_TEST(UString, upper) {
	Common::UString str(kTestString1);

	EXPECT_STREQ(str.toUpper().c_str(), kTestStringUpper1);

	str.makeUpper();

	EXPECT_STREQ(str.c_str(), kTestStringUpper1);
}

GTEST_TEST(UString, lower) {
	Common::UString str(kTestString1);

	EXPECT_STREQ(str.toLower().c_str(), kTestStringLower1);

	str.makeLower();

	EXPECT_STREQ(str.c_str(), kTestStringLower1);
}

GTEST_TEST(UString, position) {
	const Common::UString str(kTestString1);

	EXPECT_EQ(str.getPosition(0), str.begin());
	EXPECT_EQ(str.getPosition(str.begin()), 0);

	EXPECT_EQ(str.getPosition(1), ++str.begin());
	EXPECT_EQ(str.getPosition(++str.begin()), 1);

	EXPECT_EQ(str.getPosition(str.size() - 1), --str.end());
	EXPECT_EQ(str.getPosition(--str.end()), str.size() - 1);
}

GTEST_TEST(UString, insertChar) {
	Common::UString str("Fobar");

	str.insert(++str.begin(), 'r');

	EXPECT_STREQ(str.c_str(), "Frobar");

	str.insert(str.end(), 'k');

	EXPECT_STREQ(str.c_str(), "Frobark");
}

GTEST_TEST(UString, insertString) {
	Common::UString str("Fobar");

	str.insert(++str.begin(), "rob");

	EXPECT_STREQ(str.c_str(), "Frobobar");

	str.insert(str.end(), "ki");

	EXPECT_STREQ(str.c_str(), "Frobobarki");
}

GTEST_TEST(UString, replaceChar) {
	Common::UString str("Foobar");

	str.replace(++str.begin(), 'r');

	EXPECT_STREQ(str.c_str(), "Frobar");

	str.replace(str.end(), 'k');

	EXPECT_STREQ(str.c_str(), "Frobark");
}

GTEST_TEST(UString, replaceString) {
	Common::UString str("Foobar");

	str.replace(++str.begin(), "ii");

	EXPECT_STREQ(str.c_str(), "Fiibar");

	str.replace(--str.end(), "kii");

	EXPECT_STREQ(str.c_str(), "Fiibakii");
}

GTEST_TEST(UString, erase) {
	Common::UString str("Foobar Barfoo");

	str.erase(++str.begin());

	EXPECT_STREQ(str.c_str(), "Fobar Barfoo");

	Common::UString::iterator it1 = str.begin();
	++it1;
	++it1;

	Common::UString::iterator it2 = it1;
	++it2;
	++it2;
	++it2;

	str.erase(it1, it2);
	EXPECT_STREQ(str.c_str(), "Fo Barfoo");
}

GTEST_TEST(UString, split) {
	const Common::UString str("Foobar Barfoo");

	Common::UString::iterator it = str.begin();
	++it; ++it; ++it; ++it; ++it; ++it;

	Common::UString left, right;
	str.split(it, left, right, false);

	EXPECT_STREQ(left.c_str(), "Foobar");
	EXPECT_STREQ(right.c_str(), " Barfoo");

	str.split(it, left, right, true);

	EXPECT_STREQ(left.c_str(), "Foobar");
	EXPECT_STREQ(right.c_str(), "Barfoo");
}

GTEST_TEST(UString, substr) {
	const Common::UString str("Foobar Barfoo");

	Common::UString::iterator it = str.begin();
	++it; ++it; ++it; ++it; ++it; ++it; ++it;

	EXPECT_STREQ(str.substr(str.begin(), it).c_str(), "Foobar ");
	EXPECT_STREQ(str.substr(it, str.end()).c_str(), "Barfoo");
}

GTEST_TEST(UString, format) {
	const Common::UString str = Common::UString::format("%s|%s|%d", "Foo", "Bar", 23);

	EXPECT_STREQ(str.c_str(), "Foo|Bar|23");
}

GTEST_TEST(UString, splitVector) {
	const Common::UString str("Foobar Barfoo Quux");

	std::vector<Common::UString> splits;
	const size_t maxLength = Common::UString::split(str, ' ', splits);

	EXPECT_EQ(maxLength, 6);
	ASSERT_EQ(splits.size(), 3);

	EXPECT_STREQ(splits[0].c_str(), "Foobar");
	EXPECT_STREQ(splits[1].c_str(), "Barfoo");
	EXPECT_STREQ(splits[2].c_str(), "Quux");
}

GTEST_TEST(UString, splitTextTokens) {
	const Common::UString str("foobar<token1><token2>bar<token3>blah<<token4>quux<token5");

	std::vector<Common::UString> tokens;
	Common::UString::splitTextTokens(str, tokens);

	ASSERT_EQ(tokens.size(), 9);

	EXPECT_STREQ(tokens[0].c_str(), "foobar");
	EXPECT_STREQ(tokens[1].c_str(), "<token1>");
	EXPECT_STREQ(tokens[2].c_str(), "");
	EXPECT_STREQ(tokens[3].c_str(), "<token2>");
	EXPECT_STREQ(tokens[4].c_str(), "bar");
	EXPECT_STREQ(tokens[5].c_str(), "<token3>");
	EXPECT_STREQ(tokens[6].c_str(), "blah<");
	EXPECT_STREQ(tokens[7].c_str(), "<token4>");
	EXPECT_STREQ(tokens[8].c_str(), "quux<token5");
}

GTEST_TEST(UString, charClasses) {
	EXPECT_TRUE (Common::UString::isASCII('F'));
	EXPECT_FALSE(Common::UString::isASCII(0xF6));

	EXPECT_TRUE (Common::UString::isSpace(' '));
	EXPECT_FALSE(Common::UString::isSpace('x'));

	EXPECT_TRUE (Common::UString::isDigit('0'));
	EXPECT_FALSE(Common::UString::isDigit('x'));

	EXPECT_TRUE (Common::UString::isAlpha('x'));
	EXPECT_FALSE(Common::UString::isAlpha('.'));
	EXPECT_FALSE(Common::UString::isAlpha('0'));

	EXPECT_TRUE (Common::UString::isAlNum('x'));
	EXPECT_TRUE (Common::UString::isAlNum('0'));
	EXPECT_FALSE(Common::UString::isAlNum('.'));

	EXPECT_TRUE (Common::UString::isCntrl(0x10));
	EXPECT_FALSE(Common::UString::isCntrl('x'));
}

GTEST_TEST(UString, fromUTF16) {
	EXPECT_EQ(Common::UString::fromUTF16(0x00F6), 0xF6);
}

GTEST_TEST(UString, append) {
	Common::UString str("Foobar");

	str += ' ';
	str += "Barfoo";
	str += ' ';
	str += Common::UString("Quux");

	EXPECT_STREQ(str.c_str(), "Foobar Barfoo Quux");
}
