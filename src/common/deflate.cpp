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

static void initZStream(z_stream &strm, int windowBits, size_t size, const byte *data) {
	/* Initialize the zlib data stream for decompression with our input data.
	 *
	 * This ugly const cast is necessary because the zlib API wants a non-const
	 * next_in pointer by default. Unless we define ZLIB_CONST, but that only
	 * appeared in zlib 1.2.5.3. Not really worth bumping our required zlib
	 * version for, IMHO. */
	strm.zalloc   = Z_NULL;
	strm.zfree    = Z_NULL;
	strm.opaque   = Z_NULL;

	strm.avail_in = size;
	strm.next_in  = const_cast<byte *>(data);

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

} // End of namespace Common
