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

#include <cassert>

#include "src/common/streamtokenizer.h"
#include "src/common/readstream.h"
#include "src/common/error.h"

namespace Common {

StreamTokenizer::StreamTokenizer(ConsecutiveSeparatorRule conSepRule) : _conSepRule(conSepRule) {
}

bool StreamTokenizer::isIn(uint32 c, const std::list<uint32> &list) {
	for (std::list<uint32>::const_iterator it = list.begin(); it != list.end(); ++it)
		if (*it == c)
			return true;

	return false;
}

void StreamTokenizer::addSeparator(uint32 c) {
	assert(!isIn(c, _separators) && !isIn(c, _quotes) && !isIn(c, _chunkEnds) && !isIn(c, _ignores));

	_separators.push_back(c);
}

void StreamTokenizer::addQuote(uint32 c) {
	assert(!isIn(c, _separators) && !isIn(c, _quotes) && !isIn(c, _chunkEnds) && !isIn(c, _ignores));

	_quotes.push_back(c);
}

void StreamTokenizer::addChunkEnd(uint32 c) {
	assert(!isIn(c, _separators) && !isIn(c, _quotes) && !isIn(c, _chunkEnds) && !isIn(c, _ignores));

	_chunkEnds.push_back(c);
}

void StreamTokenizer::addIgnore(uint32 c) {
	assert(!isIn(c, _separators) && !isIn(c, _quotes) && !isIn(c, _chunkEnds) && !isIn(c, _ignores));

	_ignores.push_back(c);
}

UString StreamTokenizer::getToken(SeekableReadStream &stream) {
	bool   chunkEnd  = false;
	bool   inQuote   = false;
	uint32 separator = 0xFFFFFFFF;

	uint32 c;
	UString token;

	/* Run through the stream, character by character, checking their
	 * "character classes" and collecting characters for a token. */
	while ((c = stream.readChar()) != ReadStream::kEOF) {
		// Character classes
		const bool isSeparatorChar = isIn(c, _separators);
		const bool isQuoteChar     = isIn(c, _quotes);
		const bool isChunkEndChar  = isIn(c, _chunkEnds);
		const bool isIgnoreChar    = isIn(c, _ignores);

		/* Handle ignored characters.
		 *
		 * All characters in the ignored characters list will be ignored
		 * completely. They will never be added to the token.
		 */
		if (isIgnoreChar)
			continue;

		/* Handle quote characters.
		 *
		 * A quote character toggles the "we're in quotes state". Any
		 * character that's found while in this state will be added to
		 * the token, even if it is a separator or chunk end character.
		 */
		if (isQuoteChar) {
			inQuote = !inQuote;
			continue;
		}

		if (inQuote) {
			token += c;
			continue;
		}

		/* Handle chunk end characters.
		 *
		 * When we've reached the end of the chunk, seek back by one
		 * character, so that the stream is positioned right before
		 * the chunk end characters. Then break to stop collecting.
		 */
		if (isChunkEndChar) {
			stream.seek(-1, SeekableReadStream::kOriginCurrent);
			chunkEnd = true;
			break;
		}

		/* Handle separator characters.
		 *
		 * When we've found a separator character, remember which it was
		 * (we will need it to check if we should skip following separators).
		 * Then break to stop collecting.
		 */
		if (isSeparatorChar) {
			separator = c;
			break;
		}

		/* At this point, we have a character that's not in any of the
		 * special character classes and is not in quotes. This is a normal
		 * character we'll just add to the token. Then we'll continue
		 * with the next character.
		 */

		token += c;
	}

	/* Since we're technically operating on streams of arbitrary binary data,
	 * we might have collected \0 characters. Cut off the token at that point.
	 */
	Common::UString::iterator nullChar = token.findFirst('\0');
	if (nullChar != token.end())
		token.truncate(nullChar);

	/* If we stopped collecting at a chunk end, there's nothing left to do.
	 * Just return the token.
	 */
	if (chunkEnd)
		return token;

	/* However, if we stopped collecting at a separator see if we should skip
	 * following consecutive separators.
	 *
	 * Depending on the value ConsecutiveSeparatorRule, there's different ways
	 * to go about this:
	 * - kRuleHeed: Never skip consecutive separators
	 * - kRuleIgnoreSame: Ignore all consecutive separators that are the same
	 * - kRuleIgnoreAll: Ignore all consecutive separators
	 *
	 * So we're going to consume characters out of the stream until either:
	 * - we've reached a character that is not a separator
	 * - the rule says we shouldn't skip this separator
	 *
	 * In either case, the stream is positioned right after the last separator
	 * that should be skipped.
	 */
	if (_conSepRule != kRuleHeed) {
		while ((c = stream.readChar()) != ReadStream::kEOF) {
			const bool isSeparator = isIn(c, _separators);

			bool shouldSkip = isSeparator;
			if ((_conSepRule == kRuleIgnoreSame) && (c != separator))
				shouldSkip = false;

			if (!shouldSkip) {
				stream.seek(-1, SeekableReadStream::kOriginCurrent);
				break;
			}
		}
	}

	// Finally, we can return the token
	return token;
}

size_t StreamTokenizer::getTokens(SeekableReadStream &stream, std::vector<UString> &list,
		size_t min, size_t max, const UString &def) {

	assert(max >= min);

	list.clear();
	list.reserve(min);

	size_t realTokenCount = 0;
	while (!isChunkEnd(stream) && (realTokenCount < max)) {
		UString token = getToken(stream);

		if (!token.empty() || (_conSepRule != kRuleIgnoreAll)) {
			list.push_back(token);
			realTokenCount++;
		}
	}

	while (list.size() < min)
		list.push_back(def);

	return realTokenCount;
}

void StreamTokenizer::findFirstToken(SeekableReadStream &stream) {
	uint32 c;
	while ((c = stream.readChar()) != ReadStream::kEOF) {
		if (!isIn(c, _separators) && !(isIn(c, _ignores))) {
			stream.seek(-1, SeekableReadStream::kOriginCurrent);
			break;
		}
	}
}

void StreamTokenizer::skipToken(SeekableReadStream &stream, size_t n) {
	while (n-- > 0)
		UString token = getToken(stream);
}

void StreamTokenizer::skipChunk(SeekableReadStream &stream) {
	assert(!_chunkEnds.empty());

	uint32 c;
	while ((c = stream.readChar()) != ReadStream::kEOF) {
		if (isIn(c, _chunkEnds)) {
			stream.seek(-1, SeekableReadStream::kOriginCurrent);
			break;
		}
	}
}

void StreamTokenizer::nextChunk(SeekableReadStream &stream) {
	skipChunk(stream);

	uint32 c = stream.readChar();
	if (c == ReadStream::kEOF)
		return;

	if (!isIn(c, _chunkEnds))
		stream.seek(-1, SeekableReadStream::kOriginCurrent);
}

bool StreamTokenizer::isChunkEnd(SeekableReadStream &stream) {
	uint32 c = stream.readChar();
	if (c == ReadStream::kEOF)
		return true;

	bool chunkEnd = isIn(c, _chunkEnds);

	stream.seek(-1, SeekableReadStream::kOriginCurrent);

	return chunkEnd;
}

} // End of namespace Common
