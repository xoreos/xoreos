/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file common/huffman.cpp
 *  Decompressing Huffman codes.
 */

#include <cassert>

#include "common/huffman.h"
#include "common/util.h"
#include "common/error.h"
#include "common/bitstream.h"

namespace Common {

Huffman::Symbol::Symbol(uint32 c, uint32 s) : code(c), symbol(s) {
}


Huffman::Huffman(BitStream *bits, uint8 maxLength, uint32 codeCount,
		uint32 *codes, uint8 *lengths, uint32 *symbols) : _bits(0) {

	assert(maxLength > 0);
	assert(codeCount > 0);

	_bits = bits;

	_codes.resize(maxLength);

	for (uint32 i = 0; i < codeCount; i++) {
		// The symbol. If none were specified, just assume it's identical to the code index
		uint32 symbol = symbols ? symbols[i] : i;

		// Put the code and symbol into the correct list
		_codes[lengths[i] - 1].push_back(Symbol(codes[i], symbol));
	}
}

Huffman::~Huffman() {
	delete _bits;
}

uint32 Huffman::getSymbol() {
	uint32 code = 0;

	for (CodeLists::const_iterator codes = _codes.begin(); codes != _codes.end(); ++codes) {
		code = (code << 1) | _bits->getBits();

		for (CodeList::const_iterator cCode = codes->begin(); cCode != codes->end(); ++cCode)
			if (code == cCode->code)
				return cCode->symbol;
	}

	throw Exception("Unknown Huffman code");
}

} // End of namespace Common
