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
 *  Parse tokens out of a stream.
 */

#ifndef COMMON_STREAMTOKENIZER_H
#define COMMON_STREAMTOKENIZER_H

#include <list>
#include <vector>

#include "src/common/types.h"
#include "src/common/ustring.h"

namespace Common {

class SeekableReadStream;

/** Tokenizes a stream.
 *
 *  @note Only works with clean (non-extended ASCII) and UTF-8 streams right now.
 */
class StreamTokenizer {
public:
	/** What to do when consecutive separator are found. */
	enum ConsecutiveSeparatorRule {
		kRuleIgnoreSame, ///< Ignore the repeated separator, but only if it's the same.
		kRuleIgnoreAll,  ///< Ignore all repeated separators.
		kRuleHeed        ///< Heed each separator.
	};

	StreamTokenizer(ConsecutiveSeparatorRule conSepRule = kRuleHeed);

	/** Add a character on where to split tokens.
	 *
	 *  For example, with the separator character ',', the string
	 *  "foo,bar" will be split into two tokens: "foo" and "bar".
	 *
	 *  Several different characters can act as separator characters at the
	 *  same time.
	 *
	 *  The ConsecutiveSeparatorRule value signals how consecutive separator
	 *  characters are handled.
	 */
	void addSeparator(uint32_t c);

	/** Add a character marking the end of a chunk.
	 *
	 *  A chunk end is essentially a higher-order separator. Parsing tokens
	 *  will stop at chunk end characters and will not move past them. Only a
	 *  call to nextChunk() will move past a chunk end character.
	 */
	void addChunkEnd (uint32_t c);

	/** Add a character able to enclose (quote) separators and chunk ends.
	 *
	 *  For example, with the quote character '\'' and separator character
	 *  ',', the string "foo\',\'bar,foo" will be split into two tokens:
	 *  "foo,bar" and "bar".
	 *
	 *  Every quote character is handled as if it's the same! So with the
	 *  quote characters '\'' and '\"', the string "foo\',\"bar,foo" will
	 *  also yield the two tokens "foo,bar" and "bar.
	 */
	void addQuote    (uint32_t c);

	/** Add a character to ignore.
	 *
	 *  A character that is ignored will never be added to the token.
	 *  For example, with the ignore character '#' and the separator
	 *  character ',', the string "fo#o,#bar" will be splut into two
	 *  tokens: "foo" and "bar".
	 */
	void addIgnore   (uint32_t c);

	/** Parse a token out of the stream.
	 *
	 *  Go through the stream, character by character, collecting characters
	 *  for a token. Collection will stop on any of these conditions:
	 *
	 *  - We reached the end of the stream
	 *  - We reached a separator character
	 *  - We reached a chunk end character
	 *
	 *  When we find a separator character, the stream will be positioned
	 *  after this character (potentially skipping over following separators
	 *  depending on the ConsecutiveSeparatorRule value).
	 *
	 *  When we find a chunk end character, the stream will be positioned
	 *  before this character. Only a call to nextChunk() will move the
	 *  stream past it.
	 */
	UString getToken(SeekableReadStream &stream);

	/** Parse tokens out of the stream.
	 *
	 *  This method calls getToken() repeatedly and collects all tokens
	 *  into a list.
	 *
	 *  @param  stream The stream to parse out of.
	 *  @param  list The list to parse into.
	 *  @param  min Minimum number of tokens to parse.
	 *  @param  max Maximum number of tokens to parse.
	 *  @param  def Non-existing tokens are assigned this value.
	 *  @return The number of existing tokens parsed.
	 */
	size_t getTokens(SeekableReadStream &stream, std::vector<UString> &list,
			size_t min = 0, size_t max = SIZE_MAX, const UString &def = "");

	/** Find the first token character, skipping past separators.
	 *
	 *  Position the stream at the first character that is neither
	 *  a separator or an ignored characted. This is useful if the
	 *  first token of a chunk might be indented with separator
	 *  characters.
	 */
	void findFirstToken(SeekableReadStream &stream);

	/** Skip a number of tokens. */
	void skipToken(SeekableReadStream &stream, size_t n = 1);

	/** Skip to the end of the chunk.
	 *
	 *  The stream will be positioned before the next end chunk.
	 */
	void skipChunk(SeekableReadStream &stream);

	/** Skip past end of chunk characters.
	 *
	 *  If the next character is a chunk end character, position the
	 *  stream directly past it. If the next character is not a chunk
	 *  end character, do nothing.
	 */
	void nextChunk(SeekableReadStream &stream);

private:
	ConsecutiveSeparatorRule _conSepRule;

	std::list<uint32_t> _separators;
	std::list<uint32_t> _quotes;
	std::list<uint32_t> _chunkEnds;
	std::list<uint32_t> _ignores;

	static bool isIn(uint32_t c, const std::list<uint32_t> &list);

	bool isChunkEnd(SeekableReadStream &stream);
};

} // End of namespace Common

#endif // COMMON_STREAMTOKENIZER_H
