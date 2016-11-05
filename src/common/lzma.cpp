/* Phaethon - A FLOSS resource explorer for BioWare's Aurora engine games
 *
 * Phaethon is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * Phaethon is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * Phaethon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Phaethon. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file
 *  Decompress LZMA, using liblzma.
 */

// We need to include our types.h before lzma.h to stop it redefining macros
#include "src/common/types.h"
#include <lzma.h>

#include "src/common/lzma.h"
#include "src/common/scopedptr.h"
#include "src/common/error.h"
#include "src/common/memreadstream.h"

namespace Common {

byte *decompressLZMA1(const byte *data, size_t inputSize, size_t outputSize) {
	lzma_filter filters[2];
	filters[0].id      = LZMA_FILTER_LZMA1;
	filters[0].options = 0;
	filters[1].id      = LZMA_VLI_UNKNOWN;
	filters[1].options = 0;

	if (!lzma_filter_decoder_is_supported(filters[0].id))
		throw Exception("LZMA1 compression not supported");

	uint32 propsSize;
	if (lzma_properties_size(&propsSize, &filters[0]) != LZMA_OK)
		throw Exception("Can't get LZMA1 properties size");

	if (propsSize > inputSize)
		throw Exception("LZMA1 properties size larger than input data");

	if (lzma_properties_decode(&filters[0], 0, data, propsSize) != LZMA_OK)
		throw Exception("Failed to decode LZMA properties");

	data      += propsSize;
	inputSize -= propsSize;

	ScopedArray<byte> outputData(new byte[outputSize]);

	size_t posIn = 0, posOut = 0;
	lzma_ret decodeRet = lzma_raw_buffer_decode(filters, 0,
			data            , &posIn , inputSize,
			outputData.get(), &posOut, outputSize);

	/* Ignore LZMA_DATA_ERROR and LZMA_BUF_ERROR thrown from the uncompressor.
	 * These are also thrown when there is no end marker (which happens in data
	 * found in BZF files). Unfortunately, LZMA provides no way to differentiate
	 * between "real" buffer/data errors and missing end markers. So we have
	 * no way to detect actual truncated input data, for example. */

	if ((decodeRet != LZMA_OK) && (decodeRet != LZMA_DATA_ERROR) && (decodeRet != LZMA_BUF_ERROR))
		throw Exception("Failed to uncompress LZMA data: %d", (int) decodeRet);

	return outputData.release();
}

SeekableReadStream *decompressLZMA1(ReadStream &input, size_t inputSize, size_t outputSize) {
	ScopedArray<byte> inputData(new byte[inputSize]);
	if (input.read(inputData.get(), inputSize) != inputSize)
		throw Exception(kReadError);

	const byte *outputData = decompressLZMA1(inputData.get(), inputSize, outputSize);

	return new MemoryReadStream(outputData, outputSize, true);
}

} // End of namespace Common
