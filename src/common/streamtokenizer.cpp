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
	_separators.push_back(c);
}

void StreamTokenizer::addQuote(uint32 c) {
	_quotes.push_back(c);
}

void StreamTokenizer::addChunkEnd(uint32 c) {
	_chunkEnds.push_back(c);
}

void StreamTokenizer::addIgnore(uint32 c) {
	_ignores.push_back(c);
}

UString StreamTokenizer::getToken(SeekableReadStream &stream) {
	// Init
	bool   chunkEnd     = false;
	bool   inQuote      = false;
	bool   hasSeparator = false;
	uint32 separator    = 0;

	UString token;

	// Run through the stream, character by character
	uint32 c;
	while ((c = stream.readChar()) != ReadStream::kEOF) {

		if (isIn(c, _chunkEnds)) {
			// This is a end character, seek back and break
			stream.seek(-1, SeekableReadStream::kOriginCurrent);
			chunkEnd = true;
			break;
		}

		if (isIn(c, _quotes)) {
			// This is a quote character, set state
			inQuote = !inQuote;
			continue;
		}

		if (!inQuote && isIn(c, _separators)) {
			// We're not in a quote and this is a separator

			if (!token.empty()) {
				// We have a token

				hasSeparator = true;
				separator = c;
				break;
			}

			// We don't yet have a token, let the consecutive separator rule decide what to do

			if (_conSepRule == kRuleHeed) {
				// We heed every separator

				hasSeparator = true;
				separator = c;
				break;
			}

			if ((_conSepRule == kRuleIgnoreSame) && hasSeparator && (separator != ((byte) c))) {
				// We ignore only consecutive separators that are the same
				hasSeparator = true;
				separator = c;
				break;
			}

			// We ignore all consecutive separators
			hasSeparator = true;
			separator = c;
			continue;
		}

		if (isIn(c, _ignores))
			// This is a character to be ignored, do so
			continue;

		// A normal character, add it to our token
		token += c;
	}

	// Is the string actually empty?
	if (!token.empty() && (*token.begin() == '\0'))
		token.clear();

	if (!chunkEnd && (_conSepRule != kRuleHeed)) {
		// We have to look for consecutive separators

		while ((c = stream.readChar()) != ReadStream::kEOF) {

			// Use the rule to determine when we should abort skipping consecutive separators
			if (((_conSepRule == kRuleIgnoreSame) && (c != separator)) ||
			    ((_conSepRule == kRuleIgnoreAll ) && !isIn(c, _separators))) {

				stream.seek(-1, SeekableReadStream::kOriginCurrent);
				break;
			}
		}

	}

	// And return the token
	return token;
}

size_t StreamTokenizer::getTokens(SeekableReadStream &stream, std::vector<UString> &list,
		size_t min, size_t max, const UString &def) {

	assert(max >= min);

	list.clear();
	list.reserve(min);

	size_t realTokenCount;
	for (realTokenCount = 0; !isChunkEnd(stream) && (realTokenCount < max); realTokenCount++) {
		UString token = getToken(stream);

		if (!token.empty() || (_conSepRule != kRuleIgnoreAll))
			list.push_back(token);
	}

	while (list.size() < min)
		list.push_back(def);

	return realTokenCount;
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
