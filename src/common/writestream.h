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
 *  Basic writing stream interfaces.
 */

/* Based on ScummVM (<http://scummvm.org>) code, which is released
 * under the terms of version 2 or later of the GNU General Public
 * License.
 *
 * The original copyright note in ScummVM reads as follows:
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef COMMON_WRITESTREAM_H
#define COMMON_WRITESTREAM_H

#include <cstddef>
#include <string>

#include "src/common/types.h"
#include "src/common/endianness.h"
#include "src/common/util.h"
#include "src/common/error.h"

namespace Common {

class UString;
class ReadStream;

/** Generic interface for a writable data stream. */
class WriteStream {
public:
	WriteStream();
	virtual ~WriteStream();

	/** Write data into the stream. Subclasses must implement this
	 *  method; all other write methods are implemented using it.
	 *
	 *  @param  dataPtr pointer to the data to be written.
	 *  @param  dataSize number of bytes to be written.
	 *  @return the number of bytes which were actually written.
	 */
	virtual size_t write(const void *dataPtr, size_t dataSize) = 0;

	/** Write data into the stream, throwing on error.
	 *
	 *  @param  dataPtr pointer to the data to be written.
	 *  @param  dataSize number of bytes to be written.
	 */
	FORCEINLINE void writeChecked(const void *dataPtr, size_t dataSize) {
		if (write(dataPtr, dataSize) != dataSize)
			throw Exception(kWriteError);
	}

	/** Commit any buffered data to the underlying channel or
	 *  storage medium; unbuffered streams can use the default
	 *  implementation.
	 *
	 *  When flushing fails, a kWriteError exception is thrown.
	 */
	virtual void flush();

	// --- The following methods should generally not be overloaded ---

	void writeByte(byte value) {
		writeChecked(&value, 1);
	}

	void writeSByte(int8_t value) {
		writeChecked(&value, 1);
	}

	void writeUint16LE(uint16_t value) {
		value = TO_LE_16(value);
		writeChecked(&value, 2);
	}

	void writeUint32LE(uint32_t value) {
		value = TO_LE_32(value);
		writeChecked(&value, 4);
	}

	void writeUint64LE(uint64_t value) {
		value = TO_LE_64(value);
		writeChecked(&value, 8);
	}

	void writeUint16BE(uint16_t value) {
		value = TO_BE_16(value);
		writeChecked(&value, 2);
	}

	void writeUint32BE(uint32_t value) {
		value = TO_BE_32(value);
		writeChecked(&value, 4);
	}

	void writeUint64BE(uint64_t value) {
		value = TO_BE_64(value);
		writeChecked(&value, 8);
	}

	/** Write n bytes of value to the stream. */
	void writeBytes(byte value, size_t n) {
		for (size_t i = 0; i < n; ++i)
			writeChecked(&value, 1);
	}

	/** Write n zeros to the stream. */
	FORCEINLINE void writeZeros(size_t n) {
		writeBytes(0, n);
	}

	FORCEINLINE void writeSint16LE(int16_t value) {
		writeUint16LE((uint16_t)value);
	}

	FORCEINLINE void writeSint32LE(int32_t value) {
		writeUint32LE((uint32_t)value);
	}

	FORCEINLINE void writeSint64LE(int64_t value) {
		writeUint64LE((uint64_t)value);
	}

	FORCEINLINE void writeSint16BE(int16_t value) {
		writeUint16BE((uint16_t)value);
	}

	FORCEINLINE void writeSint32BE(int32_t value) {
		writeUint32BE((uint32_t)value);
	}

	FORCEINLINE void writeSint64BE(int64_t value) {
		writeUint64BE((uint64_t)value);
	}

	FORCEINLINE void writeIEEEFloatLE(float value) {
		writeUint32LE((uint32_t)convertIEEEFloat(value));
	}

	FORCEINLINE void writeIEEEFloatBE(float value) {
		writeUint32BE((uint32_t)convertIEEEFloat(value));
	}

	FORCEINLINE void writeIEEEDoubleLE(double value) {
		writeUint64LE((uint64_t)convertIEEEDouble(value));
	}

	FORCEINLINE void writeIEEEDoubleBE(double value) {
		writeUint64BE((uint64_t)convertIEEEDouble(value));
	}

	/** Copy n bytes of the given stream into the stream.
	 *
	 *  Even if less bytes than requested could be written to the
	 *  stream, all requested bytes will always be read from the
	 *  input stream.
	 *
	 *  @param  stream The stream to read from.
	 *  @param  n The number of bytes to read from the stream.
	 *  @return the number of bytes which were actually written.
	 */
	size_t writeStream(ReadStream &stream, size_t n);

	/** Copy the complete contents of the given stream.
	 *
	 *  Even if less bytes than are available in the input stream
	 *  can be written to this stream, the input stream will always
	 *  be exhausted to completion.
	 *
	 *  @param  stream The stream to read from.
	 *  @return the number of bytes which were actually written.
	 */
	size_t writeStream(ReadStream &stream);

	/** Write the given string to the stream, encoded as-is.
	 *  No terminating zero byte is written. */
	void writeString(const char *str);

	/** Write the given string to the stream, encoded as-is.
	 *  No terminating zero byte is written. */
	FORCEINLINE void writeString(const std::string &str) {
		writeChecked(str.c_str(), str.size());
	}

	/** Write the given string to the stream, encoded as UTF-8.
	 *  No terminating zero byte is written. */
	FORCEINLINE void writeString(const UString &str) {
		writeString(str.c_str());
	}
};

class SeekableWriteStream : public WriteStream {
public:
	/** The position a seeking offset takes as a base. */
	enum Origin {
		kOriginBegin   = 0, ///< Seek from the begin of the stream.
		kOriginCurrent = 1, ///< Seek from the current position of the stream.
		kOriginEnd     = 2, ///< Seek from the end of the stream.
		kOriginMAX          ///< For range checks.
	};

	SeekableWriteStream();
	~SeekableWriteStream();

	/** Obtains the current value of the stream position indicator of the stream.
	 *
	 *  @return the current position indicator, or kPositionInvalid if an error occurred.
	 */
	virtual size_t pos() const = 0;

	/** Obtains the total size of the stream, measured in bytes.
	 *  If this value is unknown or can not be computed, kSizeInvalid is returned.
	 *
	 *  @return the size of the stream, or kSizeInvalid if an error occurred.
	 */
	virtual size_t size() const = 0;

	/** Sets the stream position indicator for the stream. The new position,
	 *  measured in bytes, is obtained by adding offset bytes to the position
	 *  specified by whence. If whence is set to kOriginBegin, kOriginCurrent, or
	 *  kOriginEnd, the offset is relative to the start of the file, the current
	 *  position indicator, or end-of-file, respectively.
	 *
	 *  On error, or when trying to seek outside the stream, a kSeekError
	 *  exception is thrown.
	 *
	 *  @param  offset the relative offset in bytes.
	 *  @param  whence the seek reference: kOriginBegin, kOriginCurrent or kOriginEnd.
	 *  @return the previous position of the stream, before seeking.
	 */
	virtual size_t seek(ptrdiff_t offset, Origin whence = kOriginBegin) = 0;

	/** Skip the specified number of bytes, adding that offset to the current
	 *  position in the stream.
	 *
	 *  On error, or when trying to skip outside the stream, a kSeekError
	 *  exception is thrown.
	 *
	 *  @param  offset the number of bytes to skip.
	 *  @return the previous position of the stream, before skipping.
	 */
	virtual size_t skip(ptrdiff_t offset) {
		return seek(offset, kOriginCurrent);
	}

	/** Evaluate the seek offset relative to whence into a position from the beginning. */
	static size_t evalSeek(ptrdiff_t offset, Origin whence, size_t pos, size_t begin, size_t size);
};

} // End of namespace Common

#endif // COMMON_WRITESTREAM_H
