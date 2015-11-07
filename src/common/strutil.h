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
 *  Utility templates and functions for working with strings.
 */

#ifndef COMMON_STRUTIL_H
#define COMMON_STRUTIL_H

#include "src/common/types.h"

namespace Common {

class UString;
class SeekableReadStream;

/** Print a quick hex dump of the given data. */
void printDataHex(SeekableReadStream &stream, size_t size = SIZE_MAX);
/** Print a quick hex dump of the given data. */
void printDataHex(const byte *data, size_t size);

/** Create an elaborate string from an integer tag, for debugging purposes.
 *
 *  If all 4 bytes of the integer are printable characters, return
 *  "0xXXXX ('cccc')"
 *  Otherwise, return
 *  "0xXXXX"
 */
UString debugTag(uint32 tag, bool trim = false);

/** Parse a string into any POD integer, float/double or bool type.
 *
 *  If allowEmpty is false, parseString() will throw when encountering an empty string.
 *  If allowEmpty is true and an empty string is encountered, parseString() will
 *  immediately return without modifying the value parameter.
 */
template<typename T> void parseString(const UString &str, T &value, bool allowEmpty = false);

/** Convert any POD integer, float/double or bool type into a string. */
template<typename T> UString composeString(T value);

} // End of namespace Common

#endif // COMMON_STRUTIL_H
