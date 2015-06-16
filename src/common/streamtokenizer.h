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

	/** Add a character on where to split. */
	void addSeparator(uint32 c);
	/** Add a character able to enclose separators. */
	void addQuote    (uint32 c);
	/** Add a character marking the end of a chunk. */
	void addChunkEnd (uint32 c);
	/** Add a character to ignore. */
	void addIgnore   (uint32 c);

	/** Parse a token out of the stream. */
	UString getToken(SeekableReadStream &stream);

	/** Parse tokens out of the stream.
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

	/** Skip a number of tokens. */
	void skipToken(SeekableReadStream &stream, size_t n = 1);

	/** Skip to the end of the chunk. */
	void skipChunk(SeekableReadStream &stream);

	/** Skip past end of chunk characters. */
	void nextChunk(SeekableReadStream &stream);

private:
	ConsecutiveSeparatorRule _conSepRule;

	std::list<uint32> _separators;
	std::list<uint32> _quotes;
	std::list<uint32> _chunkEnds;
	std::list<uint32> _ignores;

	static bool isIn(uint32 c, const std::list<uint32> &list);

	bool isChunkEnd(SeekableReadStream &stream);
};

} // End of namespace Common

#endif // COMMON_STREAMTOKENIZER_H
