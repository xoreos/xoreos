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

#include <string>
#include <vector>

#include "common/types.h"

namespace Common {

class SeekableReadStream;

typedef std::vector<char> CharList;

/** Parse tokens oput of a stream.
 *
 *  @param  stream The stream to parse out of.
 *  @param  string The string to parse into.
 *  @param  splitChars A vector containing all characters on where to split.
 *  @param  endChars A vector containing all characters signaling a token list end.
 *  @param  quoteChars A vector containing all characters able to surround a split character
 *                     within a token.
 *  @param  ignoreChars A vector containing all characters to be ignored.
 *  @return true if there's still tokens left in the list (no end character was reached), false
 *          otherwise.
 */
bool parseToken(SeekableReadStream &stream, std::string &string,
                const CharList &splitChars, const CharList &endChars,
                const CharList &quoteChars, const CharList &ignoreChars);

} // End of namespace Common

#endif // COMMON_STREAMTOKENIZER_H
