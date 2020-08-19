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
 *  A decompression implementation of the oodle1 compression. Based
 *  on https://github.com/Arbos/nwn2mdk/blob/master/nwn2mdk-lib/gr2_decompress.cpp
 */

#ifndef AURORA_OODLE_H
#define AURORA_OODLE_H

#include "src/common/readstream.h"

namespace Aurora {

/** Decompress a chunk of data compressed using the oodle1 compression
 *  algorithm. It needs also the stop0 and stop1 values.
 *
 *  @param data The incoming compressed oodle1 data
 *  @param compressedSize The size of the compressed data
 *  @param decompressedSize The decompressed size of the data
 *  @param stop0 The stop0 value
 *  @param stop1 The stop1 value
 *  @return A stream of the decompressed data
 */
Common::ReadStream *decompressOodle1(byte *data, size_t compressedSize, size_t decompressedSize, uint32_t stop0, uint32_t stop1);

} // End of namespace Aurora

#endif // AURORA_OODLE_H
