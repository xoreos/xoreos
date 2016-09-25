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
 *  Hashing/digesting using the MD5 algorithm.
 */

#ifndef COMMON_MD5_H
#define COMMON_MD5_H

#include <vector>

#include "src/common/types.h"

namespace Common {

class UString;
class ReadStream;

/** The length of an MD5 digest in bytes. */
static const size_t kMD5Length = 16;

/** Hash the stream into an MD5 digest of 16 bytes. */
void hashMD5(ReadStream &stream, std::vector<byte> &digest);
/** Hash the data into an MD5 digest of 16 bytes. */
void hashMD5(const byte *data, size_t dataLength, std::vector<byte> &digest);
/** Hash the string into an MD5 digest of 16 bytes. */
void hashMD5(const UString &string, std::vector<byte> &digest);
/** Hash the array of data into an MD5 digest of 16 bytes. */
void hashMD5(const std::vector<byte> &data, std::vector<byte> &digest);

/** Hash the stream and compare the digests, returning true if they match. */
bool compareMD5Digest(ReadStream &stream, const std::vector<byte> &digest);
/** Hash the array of data and compare the digests, returning true if they match. */
bool compareMD5Digest(const byte *data, size_t dataLength, const std::vector<byte> &digest);
/** Hash the string and compare the digests, returning true if they match. */
bool compareMD5Digest(const UString &string, const std::vector<byte> &digest);
/** Hash the array of data and compare the digests, returning true if they match. */
bool compareMD5Digest(const std::vector<byte> &data, const std::vector<byte> &digest);

} // End of namespace Common

#endif // COMMON_MD5_H
