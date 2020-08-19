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
 *  Unit tests for our Huffman decoder.
 */

#include "gtest/gtest.h"

#include "src/common/huffman.h"
#include "src/common/error.h"
#include "src/common/util.h"
#include "src/common/memreadstream.h"
#include "src/common/bitstream.h"

static const uint32_t kCodes  [] = {  0,   4,   5,   6,   7  };
static const uint8_t  kLengths[] = {  1,   3,   3,   3,   3  };
static const uint32_t kSymbols[] = { 'A', 'B', 'C', 'D', 'E' };
static const uint8_t  kMaxLength = 3;

static const byte   kHuffmanData[] = { 0x45, 0x67 };

static const uint32_t kDeHuffmanDataSymbols[] = { 'A', 'B', 'A', 'C', 'A', 'D', 'A', 'E' };
static const uint32_t kDeHuffmanDataCodes  [] = {  0,   1,   0,   2,   0,   3,   0,   4  };

GTEST_TEST(Huffman, getSymbolSymbols) {
	Common::MemoryReadStream byteStream(kHuffmanData);
	Common::BitStream8MSB    bitStream (byteStream);

	Common::Huffman huffman(kMaxLength, ARRAYSIZE(kCodes), kCodes, kLengths, kSymbols);

	for (size_t i = 0; i < ARRAYSIZE(kDeHuffmanDataSymbols); i++)
		EXPECT_EQ(huffman.getSymbol(bitStream), kDeHuffmanDataSymbols[i]) << "At index " << i;
}

GTEST_TEST(Huffman, getSymbolCodes) {
	Common::MemoryReadStream byteStream(kHuffmanData);
	Common::BitStream8MSB    bitStream (byteStream);

	Common::Huffman huffman(kMaxLength, ARRAYSIZE(kCodes), kCodes, kLengths, 0);

	for (size_t i = 0; i < ARRAYSIZE(kDeHuffmanDataCodes); i++)
		EXPECT_EQ(huffman.getSymbol(bitStream), kDeHuffmanDataCodes[i]) << "At index " << i;
}

GTEST_TEST(Huffman, eos) {
	Common::MemoryReadStream byteStream(kHuffmanData);
	Common::BitStream8MSB    bitStream (byteStream);

	Common::Huffman huffman(kMaxLength, ARRAYSIZE(kCodes), kCodes, kLengths, kSymbols);

	for (size_t i = 0; i < ARRAYSIZE(kDeHuffmanDataSymbols); i++)
		huffman.getSymbol(bitStream);

	EXPECT_THROW(huffman.getSymbol(bitStream), Common::Exception);
}

GTEST_TEST(Huffman, searchMaxLength) {
	Common::MemoryReadStream byteStream(kHuffmanData);
	Common::BitStream8MSB    bitStream (byteStream);

	Common::Huffman huffman(0, ARRAYSIZE(kCodes), kCodes, kLengths, kSymbols);

	for (size_t i = 0; i < ARRAYSIZE(kDeHuffmanDataSymbols); i++)
		EXPECT_EQ(huffman.getSymbol(bitStream), kDeHuffmanDataSymbols[i]) << "At index " << i;
}

GTEST_TEST(Huffman, setSymbols) {
	Common::MemoryReadStream byteStream(kHuffmanData);
	Common::BitStream8MSB    bitStream (byteStream);

	Common::Huffman huffman(kMaxLength, ARRAYSIZE(kCodes), kCodes, kLengths, kSymbols);

	for (size_t i = 0; i < ARRAYSIZE(kDeHuffmanDataSymbols); i++)
		EXPECT_EQ(huffman.getSymbol(bitStream), kDeHuffmanDataSymbols[i]) << "At index " << i;

	huffman.setSymbols(0);
	bitStream.rewind();

	for (size_t i = 0; i < ARRAYSIZE(kDeHuffmanDataCodes); i++)
		EXPECT_EQ(huffman.getSymbol(bitStream), kDeHuffmanDataCodes[i]) << "At index " << i;

	huffman.setSymbols(kSymbols);
	bitStream.rewind();

	for (size_t i = 0; i < ARRAYSIZE(kDeHuffmanDataSymbols); i++)
		EXPECT_EQ(huffman.getSymbol(bitStream), kDeHuffmanDataSymbols[i]) << "At index " << i;
}

GTEST_TEST(Huffman, invalidSymbol) {
	static const uint32_t kInvalidCodes  [] = { 0, 4, 5, 6 };
	static const uint8_t  kInvalidLengths[] = { 1, 3, 3, 3 };
	static const uint8_t  kInvalidMaxLength = 3;

	static const byte   kInvalidHuffmanData[] = { 0x77 };

	Common::MemoryReadStream byteStream(kInvalidHuffmanData);
	Common::BitStream8MSB    bitStream (byteStream);

	Common::Huffman huffman(kInvalidMaxLength, ARRAYSIZE(kInvalidCodes), kInvalidCodes, kInvalidLengths, 0);

	EXPECT_EQ(huffman.getSymbol(bitStream), 0);

	EXPECT_THROW(huffman.getSymbol(bitStream), Common::Exception);
}
