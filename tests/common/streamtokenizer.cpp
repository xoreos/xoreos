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
 *  Unit tests for our stream tokenizer.
 */

#include <vector>

#include "gtest/gtest.h"

#include "src/common/streamtokenizer.h"
#include "src/common/util.h"
#include "src/common/memreadstream.h"

static void compareList(const char * const *list, size_t n,
                        const std::vector<Common::UString> &tokens, size_t t = 0) {

	ASSERT_EQ(tokens.size(), n);

	for (size_t i = 0; i < n; i++)
		EXPECT_STREQ(tokens[i].c_str(), list[i]) << "At case " << t << ", index " << i;
}

GTEST_TEST(StreamTokenizer, getToken) {
	static const char * const kTokens[] = { "foo", "foobar", "bar" };

	static const char *kData = "foo,foobar,bar";
	Common::MemoryReadStream stream(kData);

	Common::StreamTokenizer tokenizer;
	tokenizer.addSeparator(',');

	for (size_t i = 0; i < ARRAYSIZE(kTokens); i++)
		EXPECT_STREQ(tokenizer.getToken(stream).c_str(), kTokens[i]) << "At index " << i;
}

GTEST_TEST(StreamTokenizer, getTokens) {
	static const char * const kTokens       [] = { "foo", "foobar", "bar", "", "" };
	static const char * const kTokensDefault[] = { "foo", "foobar", "bar", "default", "default" };

	static const char *kData = "foo,foobar,bar";
	Common::MemoryReadStream stream(kData);

	Common::StreamTokenizer tokenizer;
	tokenizer.addSeparator(',');

	std::vector<Common::UString> tokens;
	ASSERT_EQ(tokenizer.getTokens(stream, tokens), 3);

	compareList(kTokens, 3, tokens, 0);

	stream.seek(0);
	tokens.clear();
	ASSERT_EQ(tokenizer.getTokens(stream, tokens, 5), 3);

	compareList(kTokens, 5, tokens, 1);

	stream.seek(0);
	tokens.clear();
	ASSERT_EQ(tokenizer.getTokens(stream, tokens, 2, 2), 2);

	compareList(kTokens, 2, tokens, 2);

	stream.seek(0);
	tokens.clear();
	ASSERT_EQ(tokenizer.getTokens(stream, tokens, 5, 5, "default"), 3);

	compareList(kTokensDefault, 5, tokens, 3);
}

GTEST_TEST(StreamTokenizer, consecutiveHeed) {
	static const char * const kTokens[] = { "foo", "", "", "foobar", "", "", "bar" };

	static const char *kData = "foo,,,foobar,.,bar";
	Common::MemoryReadStream stream(kData);

	Common::StreamTokenizer tokenizer(Common::StreamTokenizer::kRuleHeed);
	tokenizer.addSeparator(',');
	tokenizer.addSeparator('.');

	std::vector<Common::UString> tokens;
	ASSERT_EQ(tokenizer.getTokens(stream, tokens), ARRAYSIZE(kTokens));

	compareList(kTokens, ARRAYSIZE(kTokens), tokens);
}

GTEST_TEST(StreamTokenizer, consecutiveIgnoreSame) {
	static const char * const kTokens[] = { "foo", "foobar", "", "", "bar" };

	static const char *kData = "foo,,,foobar,.,bar";
	Common::MemoryReadStream stream(kData);

	Common::StreamTokenizer tokenizer(Common::StreamTokenizer::kRuleIgnoreSame);
	tokenizer.addSeparator(',');
	tokenizer.addSeparator('.');

	std::vector<Common::UString> tokens;
	ASSERT_EQ(tokenizer.getTokens(stream, tokens), ARRAYSIZE(kTokens));

	compareList(kTokens, ARRAYSIZE(kTokens), tokens);
}

GTEST_TEST(StreamTokenizer, consecutiveIgnoreAll) {
	static const char * const kTokens[] = { "foo", "foobar", "bar" };

	static const char *kData = "foo,,,foobar,.,bar";
	Common::MemoryReadStream stream(kData);

	Common::StreamTokenizer tokenizer(Common::StreamTokenizer::kRuleIgnoreAll);
	tokenizer.addSeparator(',');
	tokenizer.addSeparator('.');

	std::vector<Common::UString> tokens;
	ASSERT_EQ(tokenizer.getTokens(stream, tokens), ARRAYSIZE(kTokens));

	compareList(kTokens, ARRAYSIZE(kTokens), tokens);
}

GTEST_TEST(StreamTokenizer, findFirstToken) {
	static const char *kData = " foo bar";
	Common::MemoryReadStream stream(kData);

	Common::StreamTokenizer tokenizer;
	tokenizer.addSeparator(' ');

	EXPECT_STREQ(tokenizer.getToken(stream).c_str(), "");
	EXPECT_STREQ(tokenizer.getToken(stream).c_str(), "foo");
	EXPECT_STREQ(tokenizer.getToken(stream).c_str(), "bar");

	stream.seek(0);
	tokenizer.findFirstToken(stream);

	EXPECT_STREQ(tokenizer.getToken(stream).c_str(), "foo");
	EXPECT_STREQ(tokenizer.getToken(stream).c_str(), "bar");
}

GTEST_TEST(StreamTokenizer, skipToken) {
	static const char *kData = "foo,foobar,bar,quux1,quux2,baz";
	Common::MemoryReadStream stream(kData);

	Common::StreamTokenizer tokenizer;
	tokenizer.addSeparator(',');

	EXPECT_STREQ(tokenizer.getToken(stream).c_str(), "foo");

	tokenizer.skipToken(stream);

	EXPECT_STREQ(tokenizer.getToken(stream).c_str(), "bar");

	tokenizer.skipToken(stream, 2);

	EXPECT_STREQ(tokenizer.getToken(stream).c_str(), "baz");
}

GTEST_TEST(StreamTokenizer, ignore) {
	static const char * const kTokens[] = { "foo", "foobar", "bar" };

	static const char *kData = "foo#,fo#obar,#bar";
	Common::MemoryReadStream stream(kData);

	Common::StreamTokenizer tokenizer;
	tokenizer.addSeparator(',');
	tokenizer.addIgnore('#');

	std::vector<Common::UString> tokens;
	ASSERT_EQ(tokenizer.getTokens(stream, tokens), 3);

	compareList(kTokens, 3, tokens);
}

GTEST_TEST(StreamTokenizer, quote) {
	static const char * const kTokens[] = { "foo", "foo,bar", "bar" };

	static const char *kData = "foo,foo\",\"bar,bar";
	Common::MemoryReadStream stream(kData);

	Common::StreamTokenizer tokenizer;
	tokenizer.addSeparator(',');
	tokenizer.addQuote('\"');

	std::vector<Common::UString> tokens;
	ASSERT_EQ(tokenizer.getTokens(stream, tokens), 3);

	compareList(kTokens, 3, tokens);
}

GTEST_TEST(StreamTokenizer, quoteMultiple) {
	static const char * const kTokens[] = { "foo", "foo,bar", "bar" };

	static const char *kData = "foo,foo',\"bar,bar";
	Common::MemoryReadStream stream(kData);

	Common::StreamTokenizer tokenizer;
	tokenizer.addSeparator(',');
	tokenizer.addQuote('\'');
	tokenizer.addQuote('\"');

	std::vector<Common::UString> tokens;
	ASSERT_EQ(tokenizer.getTokens(stream, tokens), 3);

	compareList(kTokens, 3, tokens);
}

GTEST_TEST(StreamTokenizer, nextChunk) {
	static const char * const kTokens1[] = { "foo", "foobar", "bar" };
	static const char * const kTokens2[] = { "quux", "baz" };

	static const char *kData = "foo,foobar,bar\nquux,baz";
	Common::MemoryReadStream stream(kData);

	Common::StreamTokenizer tokenizer;
	tokenizer.addSeparator(',');
	tokenizer.addChunkEnd('\n');

	std::vector<Common::UString> tokens;
	ASSERT_EQ(tokenizer.getTokens(stream, tokens), 3);

	compareList(kTokens1, 3, tokens, 0);

	tokens.clear();
	tokenizer.nextChunk(stream);
	ASSERT_EQ(tokenizer.getTokens(stream, tokens), 2);

	compareList(kTokens2, 2, tokens, 1);
}

GTEST_TEST(StreamTokenizer, skipChunk) {
	static const char * const kTokens[] = { "quux", "baz" };

	static const char *kData = "foo,foobar,bar\nquux,baz";
	Common::MemoryReadStream stream(kData);

	Common::StreamTokenizer tokenizer;
	tokenizer.addSeparator(',');
	tokenizer.addChunkEnd('\n');

	tokenizer.skipChunk(stream);
	tokenizer.nextChunk(stream);

	std::vector<Common::UString> tokens;
	ASSERT_EQ(tokenizer.getTokens(stream, tokens), 2);

	compareList(kTokens, 2, tokens);
}
