/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file common/streamtokenizer.h
 *  Parse tokens out of a stream.
 */

#ifndef COMMON_STREAMTOKENIZER_H
#define COMMON_STREAMTOKENIZER_H

#include <list>
#include <vector>

#include "common/types.h"
#include "common/ustring.h"

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
	Common::UString getToken(SeekableReadStream &stream);

	/** Parse tokens out of the stream.
	 *
	 *  @param  stream The stream to parse out of.
	 *  @param  list The list to parse into.
	 *  @param  min Minimum number of tokens to parse.
	 *  @param  max Maximum number of tokens to parse.
	 *  @param  def Non-existing tokens are assigned this value.
	 *  @return The number of existing tokens parsed.
	 */
	int getTokens(SeekableReadStream &stream, std::vector<Common::UString> &list,
			int min = 0, int max = -1, const Common::UString &def = "");

	/** Skip a number of tokens. */
	void skipToken(SeekableReadStream &stream, uint32 n = 1);

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
