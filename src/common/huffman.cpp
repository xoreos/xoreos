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
 *  Decompressing Huffman codes.
 */

#include <cassert>

#include "src/common/huffman.h"
#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/bitstream.h"

namespace Common {

Huffman::Symbol::Symbol(uint32_t c, uint32_t s) : code(c), symbol(s) {
}


Huffman::Huffman(const HuffmanTable &table) {
	init(table.maxLength, table.codeCount, table.codes, table.lengths, table.symbols);
}

Huffman::Huffman(uint8_t maxLength, size_t codeCount, const uint32_t *codes,
                 const uint8_t *lengths, const uint32_t *symbols) {

	init(maxLength, codeCount, codes, lengths, symbols);
}

void Huffman::init(uint8_t maxLength, size_t codeCount, const uint32_t *codes,
                   const uint8_t *lengths, const uint32_t *symbols) {

	assert(codeCount > 0);

	assert(codes);
	assert(lengths);

	if (maxLength == 0)
		for (size_t i = 0; i < codeCount; i++)
			maxLength = MAX(maxLength, lengths[i]);

	assert(maxLength <= 32);

	_codes.resize(maxLength);
	_symbols.resize(codeCount);

	for (size_t i = 0; i < codeCount; i++) {
		// The symbol. If none were specified, just assume it's identical to the code index
		uint32_t symbol = symbols ? symbols[i] : i;

		// Put the code and symbol into the correct list
		_codes[lengths[i] - 1].push_back(Symbol(codes[i], symbol));

		// And put the pointer to the symbol/code struct into the symbol list.
		_symbols[i] = &_codes[lengths[i] - 1].back();
	}
}

Huffman::~Huffman() {
}

void Huffman::setSymbols(const uint32_t *symbols) {
	for (size_t i = 0; i < _symbols.size(); i++)
		_symbols[i]->symbol = symbols ? *symbols++ : i;
}

uint32_t Huffman::getSymbol(BitStream &bits) const {
	uint32_t code = 0;

	for (size_t i = 0; i < _codes.size(); i++) {
		bits.addBit(code, i);

		for (CodeList::const_iterator cCode = _codes[i].begin(); cCode != _codes[i].end(); ++cCode)
			if (code == cCode->code)
				return cCode->symbol;
	}

	throw Exception("Unknown Huffman code");
}

} // End of namespace Common
