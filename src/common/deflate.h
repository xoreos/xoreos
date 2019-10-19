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

#ifndef COMMON_DEFLATE_H
#define COMMON_DEFLATE_H

#include "src/common/types.h"

namespace Common {

/* TODO (should be need it):
 * - Compression
 * - Decompress dynamically, without needing to know the size
 *   of the decompressed data beforehand
 */

class ReadStream;
class SeekableReadStream;

static const int kWindowBitsMax    =  15;
static const int kWindowBitsMaxRaw = -kWindowBitsMax;

/** Decompress (inflate) using zlib's DEFLATE algorithm.
 *
 *  @param  data       The compressed input data.
 *  @param  inputSize  The size of the input data in bytes.
 *  @param  outputSize The size of the decompressed output data.
 *                     It is assumed that this information is known and that
 *                     the whole decompressed data will fit into a buffer of
 *                     this size.
 *  @param windowBits  The base two logarithm of the window size (the size of
 *                     the history buffer). See the zlib documentation on
 *                     inflateInit2() for details.
 *  @return The decompressed data.
 */
byte *decompressDeflate(const byte *data, size_t inputSize,
                        size_t outputSize, int windowBits);

/** Decompress (inflate) using zlib's DEFLATE algorithm without knowing the output size.
 *
 *  @param  data       The compressed input data.
 *  @param  inputSize  The size of the input data in bytes.
 *  @param  outputSize The size of the decompressed output data.
 *  @param windowBits  The base two logarithm of the window size (the size of
 *                     the history buffer). See the zlib documentation on
 *                     inflateInit2() for details.
 *  @param frameSize   The size of the extracted frames, defaults to 4096.
 *  @return The decompressed data
 */
byte *decompressDeflateWithoutOutputSize(const byte *data, size_t inputSize, size_t &outputSize,
                                         int windowBits, unsigned int frameSize = 4096);

/** Decompress (inflate) using zlib's DEFLATE algorithm.
 *
 *  @param  input      The compressed input data.
 *  @param  inputSize  The size of the input data to read in bytes.
 *  @param  outputSize The size of the decompressed output data.
 *                     It is assumed that this information is known and that
 *                     the whole decompressed data will fit into a buffer of
 *                     this size.
 *  @param windowBits  The base two logarithm of the window size (the size of
 *                     the history buffer). See the zlib documentation on
 *                     inflateInit2() for details.
 *  @return A stream of the decompressed data.
 */
SeekableReadStream *decompressDeflate(ReadStream &input, size_t inputSize,
                                      size_t outputSize, int windowBits);

/** Decompress (inflate) using zlib's DEFLATE algorithm without knowing the output size.
 *
 *  @param  input      The compressed input data.
 *  @param  inputSize  The size of the input data to read in bytes.
 *  @param windowBits  The base two logarithm of the window size (the size of
 *                     the history buffer). See the zlib documentation on
 *                     inflateInit2() for details.
 *  @param frameSize   The size of the extracted frames, defaults to 4096.
 *  @return A stream of the decompressed data.
 */
SeekableReadStream *decompressDeflateWithoutOutputSize(ReadStream &input, size_t inputSize,
                                                       int windowBits, unsigned int frameSize = 4096);

/** Decompress (inflate) using zlib's DEFLATE algorithm, until a stream end marker was reached.
 *
 *  Used for decompressing and reassembling a file that was split into multiple,
 *  individually compressed chunks. This function decompresses a single chunk.
 *
 *  @param  input      The compressed input data.
 *  @param windowBits  The base two logarithm of the window size (the size of
 *                     the history buffer). See the zlib documentation on
 *                     inflateInit2() for details.
 *  @param output      The output of the current chunk is stored here.
 *  @param outputSize  Maximum number of bytes to write into output.
 *  @param frameSize   The size of frame for reading from the input stream.
 *  @return The number of bytes written to output.
 */
size_t decompressDeflateChunk(SeekableReadStream &input, int windowBits, byte *output, size_t outputSize,
                              unsigned int frameSize = 4096);

/** Compress (deflate) using zlib's DEFLATE algorithm.
 *
 *  @param input      The input data to compress.
 *  @param inputSize  The size of the input data.
 *  @param windowBits The base two logarithm of the window size (the size of
 *                    the history buffer). See the zlib documentation on
 *                    deflateInit2() for details.
 *  @param frameSize  The size of a frame for reading from the input stream.
 *  @return A stream of compressed data.
 */
SeekableReadStream *compressDeflate(ReadStream &input, size_t inputSize, int windowBits,
                                    unsigned int frameSize = 4096);

/** Compress (deflate) using zlib's DEFLATE algorithm.
 *
 *  @param data       A pointer to the data to compress.
 *  @param inputSize  The size of the input data.
 *  @param outputSize A reference to the size of the output data.
 *  @param windowBits The base two logarithm of the window size (the size of
 *                    the history buffer). See the zlib documentation on
 *                    deflateInit2() for details.
 *  @param frameSize  The size of a frame for reading from the input stream.
 *  @return A stream of compressed data.
 */
byte *compressDeflate(const byte *data, size_t inputSize, size_t &outputSize, int windowBits,
                      unsigned int frameSize = 4096);

} // End of namespace Common

#endif // COMMON_DEFLATE_H
