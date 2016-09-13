/* xoreos-tools - Tools to help with xoreos development
 *
 * xoreos-tools is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * xoreos-tools is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * xoreos-tools is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xoreos-tools. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file
 *  Compress (deflate) and decompress (inflate) using zlib's DEFLATE algorithm.
 */

#include <zlib.h>

#include "src/common/deflate.h"
#include "src/common/error.h"
#include "src/common/memreadstream.h"

namespace Common {

byte *decompressDeflate(const byte *data, size_t inputSize,
                        size_t outputSize, int windowBits) {

	byte *decompressedData = new byte[outputSize];

	/* Initialize the zlib data stream for decompression with our input data.
	 *
	 * This ugly const cast is necessary because the zlib API wants a non-const
	 * next_in pointer by default. Unless we define ZLIB_CONST, but that only
	 * appeared in zlib 1.2.5.3. Not really worth bumping our required zlib
	 * version for, IMHO. */

	z_stream strm;
	strm.zalloc   = Z_NULL;
	strm.zfree    = Z_NULL;
	strm.opaque   = Z_NULL;
	strm.avail_in = inputSize;
	strm.next_in  = const_cast<byte *>(data);

	int zResult = inflateInit2(&strm, windowBits);
	if (zResult != Z_OK) {
		inflateEnd(&strm);

		delete[] decompressedData;
		throw Exception("Could not initialize zlib inflate: %s (%d)", zError(zResult), zResult);
	}

	// Set the output data pointer and size
	strm.avail_out = outputSize;
	strm.next_out  = decompressedData;

	// Decompress. Z_FINISH, because we want to decompress the whole thing in one go.
	zResult = inflate(&strm, Z_FINISH);

	// Was the end of the input stream correctly reached?
	if ((zResult != Z_STREAM_END) || (strm.avail_out != 0)) {
		inflateEnd(&strm);
		delete[] decompressedData;

		if (zResult == Z_OK)
			throw Exception("Failed to inflate: premature end of output buffer");

		if (strm.avail_out != 0)
			throw Exception("Failed to inflate: output buffer not completely filled");

		throw Exception("Failed to inflate: %s (%d)", zError(zResult), zResult);
	}

	inflateEnd(&strm);
	return decompressedData;
}

SeekableReadStream *decompressDeflate(ReadStream &input, size_t inputSize,
                                      size_t outputSize, int windowBits) {

	byte *compressedData = new byte[inputSize];
	const byte *decompressedData = 0;

	try {
		if (input.read(compressedData, inputSize) != inputSize)
			throw Exception(kReadError);

		decompressedData = decompressDeflate(compressedData, inputSize, outputSize, windowBits);

	} catch (...) {
		delete[] compressedData;
		throw;
	}

	delete[] compressedData;
	return new MemoryReadStream(decompressedData, outputSize, true);
}

} // End of namespace Common
