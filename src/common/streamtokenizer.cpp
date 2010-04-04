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

namespace Common {

static bool isIn(const CharList &list, char c) {
	for (CharList::const_iterator it = list.begin(); it != list.end(); ++it)
		if (c == *it)
			return true;

	return false;
}

static const uint32 kSpliceSize = 32;

bool parseToken(Common::SeekableReadStream &stream, std::string &string,
                const CharList &splitChars, const CharList &endChars,
                const CharList &quoteChars, const CharList &ignoreChars) {

	// Init
	uint32 spliceCount    = 1;
	uint32 lastSpliceSize = 0;
	std::list<char *> splices;

	bool endCharFound = false;
	bool inQuote      = false;

	splices.push_back(new char[kSpliceSize]);

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

		char *curSplice = splices.back();

		curSplice[lastSpliceSize++] = c;

		// We need to allocate a new splice
		if (lastSpliceSize >= kSpliceSize) {
			splices.push_back(new char[kSpliceSize]);

			lastSpliceSize = 0;
			spliceCount++;
		}
	}

	// The number of characters we found in the end
	string.resize(((spliceCount - 1) * kSpliceSize) + lastSpliceSize);

	// Run through all splices
	uint32 spliceN = 0;
	uint32 charN   = 0;
	for (std::list<char *>::iterator splice = splices.begin(); splice != splices.end(); ++splice, spliceN++) {
		uint32 count = (spliceN == (spliceCount - 1)) ? lastSpliceSize : kSpliceSize;

		// Add it to our string
		for (uint32 i = 0; i < count; i++)
			string[charN++] = (*splice)[i];

		// And free the splice
		delete[] *splice;
	}

	// Is the string actually empty?
	if ((string.size() > 0) && (string[0] == '\0'))
		string.clear();

	// And return if we're at the token list's end
	return !endCharFound && !stream.eos();
}

} // End of namespace Common
