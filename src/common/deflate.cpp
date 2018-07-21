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
 *  Compress (deflate) and decompress (inflate) using zlib's DEFLATE algorithm.
 */

#include <zlib.h>

#include <boost/scope_exit.hpp>

#include "src/common/deflate.h"
#include "src/common/error.h"
#include "src/common/scopedptr.h"
#include "src/common/ptrvector.h"
#include "src/common/memreadstream.h"

namespace Common {

static void setZStreamInput(z_stream &strm, size_t size, const byte *data) {
	/* Set the input data for the zlib data stream.
	 *
	 * This ugly const cast is necessary because the zlib API wants a non-const
	 * next_in pointer by default. Unless we define ZLIB_CONST, but that only
	 * appeared in zlib 1.2.5.3. Not really worth bumping our required zlib
	 * version for, IMHO. */

	strm.avail_in = size;
	strm.next_in  = const_cast<byte *>(data);
}

static void initZStream(z_stream &strm, int windowBits, size_t size, const byte *data) {
	/* Initialize the zlib data stream for decompression with our input data. */

	strm.zalloc   = Z_NULL;
	strm.zfree    = Z_NULL;
	strm.opaque   = Z_NULL;

	setZStreamInput(strm, size, data);

	int zResult = inflateInit2(&strm, windowBits);
	if (zResult != Z_OK)
		throw Exception("Could not initialize zlib inflate: %s (%d)", zError(zResult), zResult);
}

byte *decompressDeflate(const byte *data, size_t inputSize,
                        size_t outputSize, int windowBits) {

	ScopedArray<byte> decompressedData(new byte[outputSize]);

	z_stream strm;
	BOOST_SCOPE_EXIT( (&strm) ) {
			inflateEnd(&strm);
	} BOOST_SCOPE_EXIT_END

	initZStream(strm, windowBits, inputSize, data);

	// Set the output data pointer and size
	strm.avail_out = outputSize;
	strm.next_out  = decompressedData.get();

	// Decompress. Z_FINISH, because we want to decompress the whole thing in one go.
	int zResult = inflate(&strm, Z_FINISH);

	// Was the end of the input stream correctly reached?
	if ((zResult != Z_STREAM_END) || (strm.avail_out != 0)) {
		if (zResult == Z_OK)
			throw Exception("Failed to inflate: premature end of output buffer");

		if (strm.avail_out != 0)
			throw Exception("Failed to inflate: output buffer not completely filled");

		throw Exception("Failed to inflate: %s (%d)", zError(zResult), zResult);
	}

	return decompressedData.release();
}

byte *decompressDeflateWithoutOutputSize(const byte *data, size_t inputSize, size_t &outputSize,
                                         int windowBits, unsigned int frameSize) {
	z_stream strm;
	BOOST_SCOPE_EXIT( (&strm) ) {
			inflateEnd(&strm);
	} BOOST_SCOPE_EXIT_END

	initZStream(strm, windowBits, inputSize, data);

	Common::PtrVector<byte, Common::DeallocatorArray> buffers;

	int zResult = 0;
	do {
		buffers.push_back(new byte[frameSize]);

		// Set the output data pointer and size
		strm.avail_out = frameSize;
		strm.next_out = buffers.back();

		// Decompress. Z_SYNC_FLUSH, because we want to decompress partwise.
		zResult = inflate(&strm, Z_SYNC_FLUSH);
		if (zResult != Z_STREAM_END && zResult != Z_OK)
			throw Exception("Failed to inflate: %s (%d)", zError(zResult), zResult);
	} while (strm.avail_in != 0);

	if (zResult != Z_STREAM_END)
		throw Exception("Failed to inflate: %s (%d)", zError(zResult), zResult);

	ScopedArray<byte> decompressedData(new byte[strm.total_out]);
	for (size_t i = 0; i < buffers.size(); ++i) {
		if (i == buffers.size() - 1)
			std::memcpy(decompressedData.get() + i * frameSize, buffers[i], strm.total_out % frameSize);
		else
			std::memcpy(decompressedData.get() + i * frameSize, buffers[i], frameSize);
	}

	outputSize = strm.total_out;
	return decompressedData.release();
}

SeekableReadStream *decompressDeflate(ReadStream &input, size_t inputSize,
                                      size_t outputSize, int windowBits) {

	ScopedArray<byte> compressedData(new byte[inputSize]);
	if (input.read(compressedData.get(), inputSize) != inputSize)
		throw Exception(kReadError);

	const byte *decompressedData = decompressDeflate(compressedData.get(), inputSize, outputSize, windowBits);

	return new MemoryReadStream(decompressedData, outputSize, true);
}

SeekableReadStream *decompressDeflateWithoutOutputSize(ReadStream &input, size_t inputSize,
                                                       int windowBits, unsigned int frameSize) {
	ScopedArray<byte> compressedData(new byte[inputSize]);
	if (input.read(compressedData.get(), inputSize) != inputSize)
		throw Exception(kReadError);

	size_t size = 0;
	byte *decompressedData = decompressDeflateWithoutOutputSize(compressedData.get(), inputSize, size, windowBits, frameSize);

	return new MemoryReadStream(decompressedData, size, true);
}

size_t decompressDeflateChunk(SeekableReadStream &input, int windowBits,
                              byte *output, size_t outputSize, unsigned int frameSize) {

	z_stream strm;
	BOOST_SCOPE_EXIT( (&strm) ) {
			inflateEnd(&strm);
	} BOOST_SCOPE_EXIT_END

	initZStream(strm, windowBits, 0, 0);

	strm.avail_out = outputSize;
	strm.next_out  = output;

	ScopedArray<byte> inputData(new byte[frameSize]);

	/* As long as the zlib stream has not ended, the chunk end was not reached.
	 * Read a frame from the input buffer and decompress. */

	int zResult = 0;
	do {
		if (strm.avail_in == 0) {
			const size_t inputSize = MIN<size_t>(input.size() - input.pos(), frameSize);
			if (inputSize == 0)
				throw Exception("Failed to inflate: input buffer empty, stream not ended");

			if (input.read(inputData.get(), inputSize) != inputSize)
				throw Exception(kReadError);

			setZStreamInput(strm, inputSize, inputData.get());
		}

		// Decompress. Z_SYNC_FLUSH, because we want to decompress partwise.
		zResult = inflate(&strm, Z_SYNC_FLUSH);
		if (zResult != Z_STREAM_END && zResult != Z_OK)
			throw Exception("Failed to inflate: %s (%d)", zError(zResult), zResult);

	} while (zResult != Z_STREAM_END);

	/* Since we don't know where the chunk ends beforehand, we probably have
	 * read past the chunk. Now that we know where the zlib stream ends, we
	 * know where the chunk ended, so we can seek back to that place. */
	input.seek(- static_cast<ptrdiff_t>(strm.avail_in), SeekableReadStream::kOriginCurrent);

	return strm.total_out;
}

} // End of namespace Common
