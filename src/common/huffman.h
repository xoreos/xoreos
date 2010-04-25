/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
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
	Huffman(uint8 maxLength, uint32 codeCount, uint32 *codes, uint8 *lengths, uint32 *symbols = 0);
	~Huffman();

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

	/** Lists of codes and their symbols, sorted by code length. */
	CodeLists _codes;
};

} // End of namespace Common

#endif // COMMON_HUFFMAN_H
