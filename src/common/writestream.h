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

	/** Commit any buffered data to the underlying channel or
	 *  storage medium; unbuffered streams can use the default
	 *  implementation.
	 *
	 *  When flushing fails, a kWriteError exception is thrown.
	 */
	virtual void flush();

	// --- The following methods should generally not be overloaded ---

	void writeByte(byte value) {
		if (write(&value, 1) != 1)
			throw Exception(kWriteError);
	}

	void writeSByte(int8 value) {
		if (write(&value, 1) != 1)
			throw Exception(kWriteError);
	}

	void writeUint16LE(uint16 value) {
		value = TO_LE_16(value);
		if (write(&value, 2) != 2)
			throw Exception(kWriteError);
	}

	void writeUint32LE(uint32 value) {
		value = TO_LE_32(value);
		if (write(&value, 4) != 4)
			throw Exception(kWriteError);
	}

	void writeUint64LE(uint64 value) {
		value = TO_LE_64(value);
		if (write(&value, 8) != 8)
			throw Exception(kWriteError);
	}

	void writeUint16BE(uint16 value) {
		value = TO_BE_16(value);
		if (write(&value, 2) != 2)
			throw Exception(kWriteError);
	}

	void writeUint32BE(uint32 value) {
		value = TO_BE_32(value);
		if (write(&value, 4) != 4)
			throw Exception(kWriteError);
	}

	void writeUint64BE(uint64 value) {
		value = TO_BE_64(value);
		if (write(&value, 8) != 8)
			throw Exception(kWriteError);
	}

	FORCEINLINE void writeSint16LE(int16 value) {
		writeUint16LE((uint16)value);
	}

	FORCEINLINE void writeSint32LE(int32 value) {
		writeUint32LE((uint32)value);
	}

	FORCEINLINE void writeSint64LE(int64 value) {
		writeUint64LE((uint64)value);
	}

	FORCEINLINE void writeSint16BE(int16 value) {
		writeUint16BE((uint16)value);
	}

	FORCEINLINE void writeSint32BE(int32 value) {
		writeUint32BE((uint32)value);
	}

	FORCEINLINE void writeSint64BE(int64 value) {
		writeUint64BE((uint64)value);
	}

	FORCEINLINE void writeIEEEFloatLE(float value) {
		writeUint32LE((uint32)convertIEEEFloat(value));
	}

	FORCEINLINE void writeIEEEFloatBE(float value) {
		writeUint32BE((uint32)convertIEEEFloat(value));
	}

	FORCEINLINE void writeIEEEDoubleLE(double value) {
		writeUint64LE((uint64)convertIEEEDouble(value));
	}

	FORCEINLINE void writeIEEEDoubleBE(double value) {
		writeUint64BE((uint64)convertIEEEDouble(value));
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

	/** Write the given string to the stream, encoded as UTF-8.
	 *  No terminating zero byte is written. */
	void writeString(const UString &str);
};

} // End of namespace Common

#endif // COMMON_WRITESTREAM_H
