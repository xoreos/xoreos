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

/* Based on UEViewer (<https://www.gildor.org/en/projects/umodel>) code, which is
 * released under the terms of the MIT license.
 *
 * The original copyright note in UEViewer reads as follows:
 *
 * Copyright 2022, Konstantin Nosov
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/** @file
 *  Decompress LZX, using libmspack.
 */

#include <memory>

#include <boost/scope_exit.hpp>

#include "src/common/lzx.h"
#include "src/common/error.h"
#include "src/common/memreadstream.h"
#include "src/common/memwritestream.h"

#include "external/mspack/mspack.h"
#include "external/mspack/lzx.h"

namespace Common {

namespace {


struct StreamHolder : mspack_file {
	ReadStream *input;
	WriteStream *output;
	int rest;
};

int readWrapper(mspack_file *file, void *buffer, int bytes) {
	StreamHolder *stream = static_cast<StreamHolder *>(file);
	if (!stream->input)
		return 0;

	ReadStream &input = *stream->input;

	if (stream->rest == 0) {
		try {
			// Read the block header
			byte header = input.readByte();
			if (header == 0xFF) {
				// [0]   = FF
				// [1,2] = uncompressed block size
				// [3,4] = compressed block size
				/* uint16_t uncompressedBlockSize = */ input.readUint16BE();
				stream->rest = input.readUint16BE();
			} else {
				// [0,1] = compressed size
				stream->rest = (header << 8) | input.readByte();
			}
		} catch (...) {
			// Safety for going through C code
			return 0;
		}
	}

	int result = input.read(buffer, std::min(stream->rest, bytes));
	stream->rest -= result;
	return result;
}

int writeWrapper(mspack_file *file, void *buffer, int bytes) {
	StreamHolder *stream = static_cast<StreamHolder *>(file);
	if (!stream->output)
		return 0;

	return stream->output->write(buffer, bytes);
}

void *allocWrapper(mspack_system *UNUSED(self), size_t bytes) {
	return std::malloc(bytes);
}

void copyWrapper(void *src, void *dst, size_t bytes) {
	std::memcpy(dst, src, bytes);
}

mspack_system makeMSPackSystem() {
	mspack_system result;
	std::memset(&result, 0, sizeof(result));

	result.read  = &readWrapper;
	result.write = &writeWrapper;
	result.alloc = &allocWrapper;
	result.free  = &std::free;
	result.copy  = &copyWrapper;

	return result;
}


} // End of anonymous namespace

std::unique_ptr<SeekableReadStream> decompressXboxLZX(ReadStream &input, size_t outputSize) {
	std::unique_ptr<byte[]> outputBuf = std::make_unique<byte[]>(outputSize);
	Common::MemoryWriteStream writeStream(outputBuf.get(), outputSize);

	StreamHolder inputHolder;
	std::memset(&inputHolder, 0, sizeof(inputHolder));
	inputHolder.input = &input;

	StreamHolder outputHolder;
	std::memset(&outputHolder, 0, sizeof(outputHolder));
	outputHolder.output = &writeStream;

	mspack_system system = makeMSPackSystem();

	lzxd_stream *lzxd = lzxd_init(&system, &inputHolder, &outputHolder, 17, 0, 256 * 1024, outputSize, 0);
	if (!lzxd)
		throw Exception("Failed to initialize lzxd");

	BOOST_SCOPE_EXIT((lzxd)) {
		lzxd_free(lzxd);
	} BOOST_SCOPE_EXIT_END;

	int result = lzxd_decompress(lzxd, outputSize);
	if (result != MSPACK_ERR_OK)
		throw Exception("lzxd_decompress returned %d", result);

	return std::make_unique<MemoryReadStream>(std::move(outputBuf), outputSize);
}

} // End of namespace Common
