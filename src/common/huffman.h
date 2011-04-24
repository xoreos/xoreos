/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file common/huffman.h
 *  Decompressing Huffman codes.
 */

#ifndef COMMON_HUFFMAN_H
#define COMMON_HUFFMAN_H

#include <vector>
#include <list>

#include "common/types.h"

namespace Common {

class BitStream;

/** Decode a Huffman'd bitstream. */
class Huffman {
public:
	/** Construct a Huffman decoder.
	 *
	 *  @param maxLength Maximal code length.
	 *  @param codeCount Number of codes.
	 *  @param codes The actual codes.
	 *  @param lengths Lengths of the individual codes.
	 *  @param symbols The symbols. If 0, assume they are identical to the code indices.
	 */
	Huffman(uint8 maxLength, uint32 codeCount, const uint32 *codes, const uint8 *lengths, const uint32 *symbols = 0);
	~Huffman();

	/** Modify the codes' symbols. */
	void setSymbols(const uint32 *symbols = 0);

	/** Return the next symbol in the bitstream. */
	uint32 getSymbol(BitStream &bits);

private:
	struct Symbol {
		uint32 code;
		uint32 symbol;

		Symbol(uint32 c, uint32 s);
	};

	typedef std::list<Symbol>     CodeList;
	typedef std::vector<CodeList> CodeLists;
	typedef std::vector<Symbol *> SymbolList;

	/** Lists of codes and their symbols, sorted by code length. */
	CodeLists _codes;

	/** Sorted list of pointers to the symbols. */
	SymbolList _symbols;
};

} // End of namespace Common

#endif // COMMON_HUFFMAN_H
