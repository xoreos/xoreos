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

#ifndef COMMON_HUFFMAN_H
#define COMMON_HUFFMAN_H

#include <cstddef>

#include <vector>
#include <list>

#include "src/common/types.h"

namespace Common {

class BitStream;

struct HuffmanTable {
	uint8_t maxLength; ///< Maximal code length. If 0, it's searched for.
	size_t  codeCount; ///< Number of codes.

	const uint32_t *codes;   ///< The actual codes.
	const uint8_t  *lengths; ///< The lengths of the individual codes.
	const uint32_t *symbols; ///< The symbols, 0 if identical to the codes.
};

/** Decode a Huffman'd bitstream. */
class Huffman {
public:
	/** Construct a Huffman decoder.
	 *
	 *  @param maxLength Maximal code length. If 0, it's searched for.
	 *  @param codeCount Number of codes.
	 *  @param codes The actual codes.
	 *  @param lengths Lengths of the individual codes.
	 *  @param symbols The symbols. If 0, assume they are identical to the code indices.
	 */
	Huffman(uint8_t maxLength, size_t codeCount, const uint32_t *codes,
	        const uint8_t *lengths, const uint32_t *symbols = 0);
	Huffman(const HuffmanTable &table);
	~Huffman();

	/** Modify the codes' symbols. */
	void setSymbols(const uint32_t *symbols = 0);

	/** Return the next symbol in the bitstream. */
	uint32_t getSymbol(BitStream &bits) const;

private:
	struct Symbol {
		uint32_t code;
		uint32_t symbol;

		Symbol(uint32_t c, uint32_t s);
	};

	typedef std::list<Symbol>     CodeList;
	typedef std::vector<CodeList> CodeLists;
	typedef std::vector<Symbol *> SymbolList;

	/** Lists of codes and their symbols, sorted by code length. */
	CodeLists _codes;

	/** Sorted list of pointers to the symbols. */
	SymbolList _symbols;

	void init(uint8_t maxLength, size_t codeCount, const uint32_t *codes,
	          const uint8_t *lengths, const uint32_t *symbols);
};

} // End of namespace Common

#endif // COMMON_HUFFMAN_H
