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
#include "common/ustring.h"
#include "common/stream.h"
#include "common/util.h"

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

} // End of namespace Common
