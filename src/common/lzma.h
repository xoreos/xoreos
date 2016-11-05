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

#ifndef COMMON_LZMA_H
#define COMMON_LZMA_H

#include "src/common/types.h"

namespace Common {

class ReadStream;
class SeekableReadStream;

/** Decompress using the LZMA1 algorithm.
 *
 *  @param  data       The compressed input data.
 *  @param  inputSize  The size of the input data in bytes.
 *  @param  outputSize The size of the decompressed output data.
 *                     It is assumed that this information is known and that
 *                     the whole decompressed data will fit into a buffer of
 *                     this size.
 *  @return The decompressed data.
 */
byte *decompressLZMA1(const byte *data, size_t inputSize, size_t outputSize);

/** Decompress using the LZMA1 algorithm.
 *
 *  @param  input      The compressed input data.
 *  @param  inputSize  The size of the input data to read in bytes.
 *  @param  outputSize The size of the decompressed output data.
 *                     It is assumed that this information is known and that
 *                     the whole decompressed data will fit into a buffer of
 *                     this size.
 *  @return A stream of the decompressed data.
 */
SeekableReadStream *decompressLZMA1(ReadStream &input, size_t inputSize, size_t outputSize);

} // End of namespace Common

#endif // COMMON_LZMA_H
