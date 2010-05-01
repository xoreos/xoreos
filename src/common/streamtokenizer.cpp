/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file common/streamtokenizer.cpp
 *  Parse tokens out of a stream.
 */

#include <list>

#include "common/streamtokenizer.h"
#include "common/stream.h"
#include "common/util.h"
#include "common/error.h"

namespace Common {

static bool isIn(const CharList &list, uint32 c) {
	for (CharList::const_iterator it = list.begin(); it != list.end(); ++it)
		if (c == *it)
			return true;

	return false;
}

static const uint32 kSpliceSize = 32;

bool parseToken(SeekableReadStream &stream, Common::UString &string,
                const CharList &splitChars, const CharList &endChars,
                const CharList &quoteChars, const CharList &ignoreChars) {

	// Init
	bool endCharFound = false;
	bool inQuote      = false;

	// Run through the stream, character by character
	while (!stream.eos()) {
		char c = (char) stream.readByte();

		if (isIn(endChars, c)) {
			// This is a end character, stop
			endCharFound = true;
			break;
		}

		if (isIn(quoteChars, c)) {
			// This is a quote character, set state
			inQuote = !inQuote;
			continue;
		}

		if (!inQuote && isIn(splitChars, c))
			// We're not in a quote and this is a split character
			break;

		if (isIn(ignoreChars, c))
			// This is a character to be ignored, do so
			continue;

		// A normal character, add it to our buffer
		string += c;
	}

	// Is the string actually empty?
	if ((string.size() > 0) && (*string.begin() == '\0'))
		string.clear();

	// And return if we're at the token list's end
	return !endCharFound && !stream.eos();
}


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

Common::UString StreamTokenizer::getToken(SeekableReadStream &stream) {
	// Init
	bool   chunkEnd     = false;
	bool   inQuote      = false;
	bool   hasSeparator = false;
	uint32 separator    = 0;

	Common::UString token;

	// Run through the stream, character by character
	while (!stream.eos()) {
		char c = (char) stream.readByte();

		if (isIn(c, _chunkEnds)) {
			// This is a end character, seek back and break
			stream.seek(-1, SEEK_CUR);
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

		while (!stream.eos()) {
			uint32 c = stream.readByte();

			// Use the rule to determine when we should abort skipping consecutive separators
			if (((_conSepRule == kRuleIgnoreSame) && (c != separator)) ||
			    ((_conSepRule == kRuleIgnoreAll ) && !isIn(c, _separators))) {

				stream.seek(-1, SEEK_CUR);
				break;
			}
		}

	}

	// And return the token
	return token;
}

int StreamTokenizer::getTokens(SeekableReadStream &stream, std::vector<Common::UString> &list,
		int min, int max, const Common::UString &def) {

	assert((min >= 0) && ((max == -1) || (max >= min)));

	list.clear();
	list.reserve(min);

	int realTokenCount;
	for (realTokenCount = 0; !isChunkEnd(stream) && ((max < 0) || (realTokenCount < max)); realTokenCount++) {
		Common::UString token = getToken(stream);

		list.push_back(token);
	}

	while (list.size() < ((uint32) min))
		list.push_back(def);

	return realTokenCount;
}

void StreamTokenizer::skipToken(SeekableReadStream &stream, uint32 n) {
	while (n-- > 0)
		Common::UString token = getToken(stream);
}

void StreamTokenizer::skipChunk(SeekableReadStream &stream) {
	assert(!_chunkEnds.empty());

	while (!stream.eos() && !stream.err()) {
		if (isIn(stream.readByte(), _chunkEnds)) {
			stream.seek(-1, SEEK_CUR);
			break;
		}
	}

	if (stream.err())
		throw Exception(kReadError);
}

void StreamTokenizer::nextChunk(SeekableReadStream &stream) {
	skipChunk(stream);

	if (stream.eos() || stream.err())
		return;

	if (!isIn(stream.readByte(), _chunkEnds))
		stream.seek(-1, SEEK_CUR);
}

bool StreamTokenizer::isChunkEnd(SeekableReadStream &stream) {
	if (stream.eos())
		return true;

	bool chunkEnd = isIn(stream.readByte(), _chunkEnds);

	stream.seek(-1, SEEK_CUR);

	return chunkEnd;
}

} // End of namespace Common
