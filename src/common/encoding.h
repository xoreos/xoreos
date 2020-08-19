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

#include <cstddef>

#include "src/common/types.h"

namespace Common {

class UString;
class SeekableReadStream;
class MemoryReadStream;
class WriteStream;

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

/** Return the human readable name of an encoding. */
UString getEncodingName(Encoding encoding);

/** Do we have support for this encoding?
 *
 *  To support an encoding, we need to be able to convert it to and from UTF-8.
 */
bool hasSupportEncoding(Encoding encoding);

/** Read a string with the given encoding of a stream.
 *
 *  Reading stops after an end-of-string terminating sequence has been read.
 *  For single- and variable-byte encodings, this is 0x00 ('\0'). For 2-byte
 *  encodings, this is 0x0000. For 4-byte encodings, this is 0x00000000.
 */
UString readString(SeekableReadStream &stream, Encoding encoding);

/** Read length bytes as a string with the given encoding out of a stream.
 *
 *  Exactly length bytes will be read out of the stream (unless reading
 *  out of the stream fails).
 */
UString readStringFixed(SeekableReadStream &stream, Encoding encoding, size_t length);

/** Read a line with the given encoding out of a stream.
 *
 *  Reading stops after an end-of-line sequence has been read. For single-
 *  and variable-byte encodings, this is 0x0A ('\n', LF, line feed). For 2-
 *  byte encodings, this is 0x000A. For 4-byte encodings, this is 0x0000000A.
 *
 *  Any occurrence of '\r' (CR, carriage return, 0x0D/0x000D/0x0000000D) will
 *  be read and ignored, and *not* stored in the resulting string. This way,
 *  both Unix-like (GNU/Linux, Mac OS X, *BSD) and DOS-like (DOS, Windows)
 *  newlines can be understood.
 */
UString readStringLine(SeekableReadStream &stream, Encoding encoding);

/** Read a string with the given encoding from the raw buffer.
 *
 *  The raw buffer may or may not end in a terminating end-of-string
 *  sequence.
 */
UString readString(const byte *data, size_t size, Encoding encoding);

/** Write a string into a stream with a given encoding.
 *
 *  @param  stream The stream to write into.
 *  @param  str The string to write.
 *  @param  encoding The encoding to convert the string into.
 *  @param  terminate Should we write a terminating end-of-string sequence into
 *                    the stream after the string has been written?
 *  @return The number of bytes written to the stream, including the end-of-string
 *          sequence if requested.
 */
size_t writeString(WriteStream &stream, const Common::UString &str, Encoding encoding, bool terminate = true);

/** Write a string into a stream with a given encoding and fixed length in bytes.
 *
 *  If the string is longer than length (in bytes), the string will be cut off.
 *  For multi-byte encodings, this may result in invalid/incomplete sequences
 *  at the end of the string.
 *
 *  If the string is shorter than length, 0x00 will be written into the stream
 *  until length has been reached.
 */
void writeStringFixed(WriteStream &stream, const Common::UString &str, Encoding encoding, size_t length);

/** Convert a string into the given encoding.
 *
 *  @param  str The string to convert.
 *  @param  encoding The encoding to convert the string into.
 *  @param  terminateString Should the result contain a terminating end-of-
 *                          string sequence?
 *  @return A newly created MemoryReadStream of the converted string.
 */
MemoryReadStream *convertString(const UString &str, Encoding encoding, bool terminateString = true);

/** Return the number of bytes per codepoint in this encoding.
 *
 *  Note: This will throw on encodings with a variable number of bytes per codepoint.
 */
size_t getBytesPerCodepoint(Encoding encoding);

/** Return whether the given codepoint is valid in this encoding.
 *
 *  TODO: Implement for Unicode and CJK encodings.
 */
bool isValidCodepoint(Encoding encoding, uint32_t cp);

} // End of namespace Common

#endif // COMMON_ENCODING_H
