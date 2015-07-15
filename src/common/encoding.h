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
 *  Utility functions for working with differing string encodings.
 */

#ifndef COMMON_ENCODING_H
#define COMMON_ENCODING_H

#include "src/common/types.h"

namespace Common {

class UString;
class SeekableReadStream;
class MemoryReadStream;

enum Encoding {
	kEncodingInvalid = -1,

	kEncodingASCII,   ///< Plain, unextended ASCII (7bit clean).

	kEncodingUTF8,    ///< UTF-8.

	kEncodingUTF16LE, ///< UTF-16 LE (little endian).
	kEncodingUTF16BE, ///< UTF-16 BE (big endian).

	kEncodingLatin9,  ///< ISO-8859-15 (Latin-9).

	kEncodingCP1250,  ///< Windows codepage 1250 (Eastern European, Latin alphabet).
	kEncodingCP1251,  ///< Windows codepage 1251 (Eastern European, Cyrillic alphabet).
	kEncodingCP1252,  ///< Windows codepage 1252 (Western European, Latin alphabet).

	kEncodingCP932,   ///< Windows codepage 932 (Japanese, extended Shift-JIS).
	kEncodingCP936,   ///< Windows codepage 936 (Simplified Chinese, extended GB2312 with GBK codepoints).
	kEncodingCP949,   ///< Windows codepage 949 (Korean, similar to EUC-KR).
	kEncodingCP950,   ///< Windows codepage 950 (Traditional Chinese, similar to Big5).

	kEncodingMAX      ///< For range checks.
};

/** Read a string with the given encoding of a stream. */
UString readString(SeekableReadStream &stream, Encoding encoding);

/** Read length bytes as a string with the given encoding out of a stream. */
UString readStringFixed(SeekableReadStream &stream, Encoding encoding, size_t length);

/** Read a line with the given encoding out of a stream. */
UString readStringLine(SeekableReadStream &stream, Encoding encoding);

/** Read a string with the given encoding from the raw buffer. */
UString readString(const byte *data, size_t size, Encoding encoding);

/** Convert a string into the given encoding. */
MemoryReadStream *convertString(const UString &str, Encoding encoding);

/** Return the number of bytes per codepoint in this encoding.
 *
 *  Note: This will throw on encodings with a variable number of bytes per codepoint.
 */
size_t getBytesPerCodepoint(Encoding encoding);

} // End of namespace Common

#endif // COMMON_ENCODING_H
