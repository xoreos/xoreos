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
 *  Encryption / decryption using Bruce Schneier's Blowfish algorithm.
 */

#ifndef COMMON_BLOWFISH_H
#define COMMON_BLOWFISH_H

#include <vector>

#include "src/common/types.h"

namespace Common {

class SeekableReadStream;
class MemoryReadStream;

/** Encrypt the stream with the Blowfish algorithm in EBC mode. */
MemoryReadStream *encryptBlowfishEBC(SeekableReadStream &input, const std::vector<byte> &key);
/** Decrypt the stream with the Blowfish algorithm in EBC mode. */
MemoryReadStream *decryptBlowfishEBC(SeekableReadStream &input, const std::vector<byte> &key);

} // End of namespace Common

#endif // COMMON_BLOWFISH_H
