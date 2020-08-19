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
 *  Decompress LZMA, using liblzma.
 */

// We need to include our types.h before lzma.h to stop it redefining macros
#include "src/common/types.h"
#include <lzma.h>

#include <memory>

#include <boost/scope_exit.hpp>

#include "src/common/lzma.h"
#include "src/common/error.h"
#include "src/common/memreadstream.h"

namespace Common {

/* Custom allocator to work around an issue when linking with liblzma that was
 * built with a different libc than the one we are build with. This happens
 * on our AppVeyor builds, for example.
 *
 * The underlying issue is that liblzma expects us to free() memory that was
 * allocated within liblzma in lzma_properties_decode(). liblzma (at the time
 * of writing) provides no wrapper function for free either. So if our free()
 * uses different structures from the malloc() in liblzma, this will crash
 * and burn.
 *
 * So instead, we provide lzma_properties_decode() with a custom allocator,
 * which uses our malloc(), so we can also use our free().
 */

static void * LZMA_API_CALL lzmaAlloc(void *UNUSED(opaque), size_t nmemb, size_t size) {
	return malloc(nmemb * size);
}

static void LZMA_API_CALL lzmaFree(void *UNUSED(opaque), void *ptr) {
	free(ptr);
}

// Allocators in liblzma < 5.1.3alpha were not const. We allow liblzma >= 5.0.3.
static lzma_allocator kLZMAAllocator = {
	&lzmaAlloc, &lzmaFree, 0
};

byte *decompressLZMA1(const byte *data, size_t inputSize, size_t outputSize, bool noEndMarker) {
	lzma_filter filters[2] = {
		{ LZMA_FILTER_LZMA1, 0 },
		{ LZMA_VLI_UNKNOWN , 0 }
	};

	if (!lzma_filter_decoder_is_supported(filters[0].id))
		throw Exception("LZMA1 compression not supported");

	uint32_t propsSize;
	if (lzma_properties_size(&propsSize, &filters[0]) != LZMA_OK)
		throw Exception("Can't get LZMA1 properties size");

	if (propsSize > inputSize)
		throw Exception("LZMA1 properties size larger than input data");

	if (lzma_properties_decode(&filters[0], &kLZMAAllocator, data, propsSize) != LZMA_OK)
		throw Exception("Failed to decode LZMA1 properties");

	data      += propsSize;
	inputSize -= propsSize;

	lzma_stream strm = LZMA_STREAM_INIT;
	BOOST_SCOPE_EXIT( (&strm) (&filters) ) {
		kLZMAAllocator.free(0, filters[0].options);
		lzma_end(&strm);
	} BOOST_SCOPE_EXIT_END

	lzma_ret lzmaRet = LZMA_OK;

	if ((lzmaRet = lzma_raw_decoder(&strm, filters)) != LZMA_OK)
		throw Exception("Failed to create raw LZMA1 decoder: %d", (int) lzmaRet);

	std::unique_ptr<byte[]> outputData = std::make_unique<byte[]>(outputSize);

	strm.next_in   = data;
	strm.avail_in  = inputSize;
	strm.next_out  = outputData.get();
	strm.avail_out = outputSize;

	lzmaRet = lzma_code(&strm, LZMA_FINISH);

	if (noEndMarker && (lzmaRet == LZMA_OK) && (strm.avail_in == 0) && (strm.avail_out == 0))
		return outputData.release();

	if ((lzmaRet != LZMA_STREAM_END) || (strm.avail_out != 0)) {
		if (lzmaRet == LZMA_OK)
			throw Exception("Failed to uncompress LZMA1 data: premature end of output buffer");

		if (strm.avail_out != 0)
			throw Exception("Failed to uncompress LZMA1 data: output buffer not completely filled");

		throw Exception("Failed to uncompress LZMA1 data: %d", (int) lzmaRet);
	}

	return outputData.release();
}

SeekableReadStream *decompressLZMA1(ReadStream &input, size_t inputSize, size_t outputSize, bool noEndMarker) {
	std::unique_ptr<byte[]> inputData = std::make_unique<byte[]>(inputSize);
	if (input.read(inputData.get(), inputSize) != inputSize)
		throw Exception(kReadError);

	const byte *outputData = decompressLZMA1(inputData.get(), inputSize, outputSize, noEndMarker);

	return new MemoryReadStream(outputData, outputSize, true);
}

} // End of namespace Common
