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
 *  Decompress LZX, using libmspack.
 */

#ifndef COMMON_LZX_H
#define COMMON_LZX_H

#include "src/common/types.h"

namespace Common {

class ReadStream;
class SeekableReadStream;

/** Decompress using the Xbox 360 LZX algorithm.
 *
 *  @param  input      The compressed input data.
 *  @param  outputSize The size of the decompressed output data.
 *                     It is assumed that this information is known and that
 *                     the whole decompressed data will fit into a buffer of
 *                     this size.
 *  @return A stream of the decompressed data.
 */
std::unique_ptr<SeekableReadStream> decompressXboxLZX(ReadStream &input, size_t outputSize);

} // End of namespace Common

#endif // COMMON_LZX_H
