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
 *  Basic reading stream interfaces.
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

#ifndef COMMON_READSTREAM_H
#define COMMON_READSTREAM_H

#include "src/common/types.h"
#include "src/common/endianness.h"
#include "src/common/disposableptr.h"
#include "src/common/util.h"
#include "src/common/error.h"

namespace Common {

class MemoryReadStream;

/** Generic interface for a readable data stream. */
class ReadStream {
public:
	/** Return value for end-of-file. See readChar(). */
	static const uint32 kEOF = 0xFFFFFFFF;

	static const size_t kSizeInvalid     = SIZE_MAX;
	static const size_t kPositionInvalid = SIZE_MAX;

	ReadStream();
	virtual ~ReadStream();

	/** Returns true if a read failed because the stream has been reached.
	 *  For a SeekableReadStream, it is cleared by a successful seek.
	 */
	virtual bool eos() const = 0;

	/** Read data from the stream. Subclasses must implement this
	 *  method; all other read methods are implemented using it.
	 *
	 *  @param  dataPtr pointer to a buffer into which the data is read.
	 *  @param  dataSize number of bytes to be read.
	 *  @return the number of bytes which were actually read.
	 */
	virtual size_t read(void *dataPtr, size_t dataSize) = 0;

	// --- The following methods should generally not be overloaded ---

	/** Read an unsigned byte from the stream and return it. */
	byte readByte() {
		byte b;
		if (read(&b, 1) != 1)
			throw Exception(kReadError);

		return b;
	}

	/** Read a signed byte from the stream and return it.  */
	FORCEINLINE int8 readSByte() {
		return (int8)readByte();
	}

	/** Reads the next character from stream and returns it as an unsigned char
	 *  cast to an uint32, or kEOF on end of file or error. Similar to fgetc().
	 */
	uint32 readChar() {
		byte b;
		try {
			b = readByte();
		} catch (...) {
			return kEOF;
		}

		return b;
	}

	/** Read an unsigned 16-bit word stored in little endian (LSB first) order
	 *  from the stream and return it.
	 */
	uint16 readUint16LE() {
		uint16 val;
		if (read(&val, 2) != 2)
			throw Exception(kReadError);

		return FROM_LE_16(val);
	}

	/** Read an unsigned 32-bit word stored in little endian (LSB first) order
	 *  from the stream and return it.
	 */
	uint32 readUint32LE() {
		uint32 val;
		if (read(&val, 4) != 4)
			throw Exception(kReadError);

		return FROM_LE_32(val);
	}

	/** Read an unsigned 64-bit word stored in little endian (LSB first) order
	 *  from the stream and return it.
	 */
	uint64 readUint64LE() {
		uint64 val;
		if (read(&val, 8) != 8)
			throw Exception(kReadError);

		return FROM_LE_64(val);
	}

	/** Read an unsigned 16-bit word stored in big endian (MSB first) order
	 *  from the stream and return it.
	 */
	uint16 readUint16BE() {
		uint16 val;
		if (read(&val, 2) != 2)
			throw Exception(kReadError);

		return FROM_BE_16(val);
	}

	/** Read an unsigned 32-bit word stored in big endian (MSB first) order
	 *  from the stream and return it.
	 */
	uint32 readUint32BE() {
		uint32 val;
		if (read(&val, 4) != 4)
			throw Exception(kReadError);

		return FROM_BE_32(val);
	}

	/** Read an unsigned 64-bit word stored in big endian (MSB first) order
	 *  from the stream and return it.
	 */
	uint64 readUint64BE() {
		uint64 val;
		if (read(&val, 8) != 8)
			throw Exception(kReadError);

		return FROM_BE_64(val);
	}

	/** Read a signed 16-bit word stored in little endian (LSB first) order
	 *  from the stream and return it.
	 */
	FORCEINLINE int16 readSint16LE() {
		return (int16)readUint16LE();
	}

	/** Read a signed 32-bit word stored in little endian (LSB first) order
	 *  from the stream and return it.
	 */
	FORCEINLINE int32 readSint32LE() {
		return (int32)readUint32LE();
	}

	/** Read a signed 64-bit word stored in little endian (LSB first) order
	 *  from the stream and return it.
	 */
	FORCEINLINE int64 readSint64LE() {
		return (int64)readUint64LE();
	}

	/** Read a signed 16-bit word stored in big endian (MSB first) order
	 *  from the stream and return it.
	 */
	FORCEINLINE int16 readSint16BE() {
		return (int16)readUint16BE();
	}

	/** Read a signed 32-bit word stored in big endian (MSB first) order
	 *  from the stream and return it.
	 */
	FORCEINLINE int32 readSint32BE() {
		return (int32)readUint32BE();
	}

	/** Read a signed 64-bit word stored in big endian (MSB first) order
	 *  from the stream and return it.
	 */
	FORCEINLINE int64 readSint64BE() {
		return (int64)readUint64BE();
	}

	/** Read a 32-bit IEEE float stored in little endian (LSB first) order
	 *  from the stream and return it.
	 */
	FORCEINLINE float readIEEEFloatLE() {
		return convertIEEEFloat(readUint32LE());
	}

	/** Read a 32-bit IEEE float stored in big endian (MSB first) order
	 *  from the stream and return it.
	 */
	FORCEINLINE float readIEEEFloatBE() {
		return convertIEEEFloat(readUint32BE());
	}

	/** Read a 64-bit IEEE double stored in little endian (LSB first) order
	 *  from the stream and return it.
	 */
	FORCEINLINE double readIEEEDoubleLE() {
		return convertIEEEDouble(readUint64LE());
	}

	/** Read a 64-bit IEEE double stored in big endian (MSB first) order
	 *  from the stream and return it.
	 */
	FORCEINLINE double readIEEEDoubleBE() {
		return convertIEEEDouble(readUint64BE());
	}

	/** Read the specified amount of data into a new[]'ed buffer
	 *  which then is wrapped into a MemoryReadStream.
	 *
	 *  When reading fails, a kReadError exception is thrown.
	 */
	MemoryReadStream *readStream(size_t dataSize);
};


/** Interface for a seekable & readable data stream. */
class SeekableReadStream : virtual public ReadStream {
public:
	/** The position a seeking offset takes as a base. */
	enum Origin {
		kOriginBegin   = 0, ///< Seek from the begin of the stream.
		kOriginCurrent = 1, ///< Seek from the current position of the stream.
		kOriginEnd     = 2, ///< Seek from the end of the stream.
		kOriginMAX          ///< For range checks.
	};

	SeekableReadStream();
	~SeekableReadStream();

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
	 *  position indicator, or end-of-file, respectively. A successful call
	 *  to the seek() method clears the end-of-file indicator for the stream.
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
	 *  position in the stream. A successful call to the skip() method clears
	 *  the end-of-file indicator for the stream.
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


/** SubReadStream provides access to a ReadStream restricted to the range
 *  [currentPosition, currentPosition+end).
 *
 *  Manipulating the parent stream directly /will/ mess up a substream.
 *  Likewise, manipulating two substreams of a parent stream will cause them to
 *  step on each others toes.
 */
class SubReadStream : virtual public ReadStream {
public:
	SubReadStream(ReadStream *parentStream, size_t end, bool disposeParentStream = false);
	~SubReadStream();

	bool eos() const;

	size_t read(void *dataPtr, size_t dataSize);

protected:
	DisposablePtr<ReadStream> _parentStream;

	size_t _pos;
	size_t _end;

	bool _eos;
};


/** SeekableSubReadStream provides access to a SeekableReadStream restricted to
 *  the range [begin, end).
 *  The same caveats apply to SeekableSubReadStream as do to SeekableReadStream.
 *
 *  Manipulating the parent stream directly /will/ mess up a substream.
 *  @see SubReadStream
 */
class SeekableSubReadStream : public SubReadStream, public SeekableReadStream {
public:
	SeekableSubReadStream(SeekableReadStream *parentStream, size_t begin, size_t end,
	                      bool disposeParentStream = false);
	~SeekableSubReadStream();

	size_t pos() const;
	size_t size() const;

	size_t seek(ptrdiff_t offset, Origin whence = kOriginBegin);

protected:
	SeekableReadStream *_parentStream;

	size_t _begin;
};


/** This is a wrapper around SeekableSubReadStream, but it adds non-endian
 *  read methods whose endianness is set on the stream creation.
 *
 *  Manipulating the parent stream directly /will/ mess up a substream.
 *  @see SubReadStream
 */
class SeekableSubReadStreamEndian : public SeekableSubReadStream {
private:
	const bool _bigEndian;

public:
	SeekableSubReadStreamEndian(SeekableReadStream *parentStream, size_t begin, size_t end,
	                            bool bigEndian = false, bool disposeParentStream = false);
	~SeekableSubReadStreamEndian();

	uint16 readUint16() {
		return _bigEndian ? readUint16BE() : readUint16LE();
	}

	uint32 readUint32() {
		return _bigEndian ? readUint32BE() : readUint32LE();
	}

	uint64 readUint64() {
		return _bigEndian ? readUint64BE() : readUint64LE();
	}

	uint16 readSint16() {
		return _bigEndian ? readSint16BE() : readSint16LE();
	}

	uint32 readSint32() {
		return _bigEndian ? readSint32BE() : readSint32LE();
	}

	uint64 readSint64() {
		return _bigEndian ? readSint64BE() : readSint64LE();
	}

	float readIEEEFloat() {
		return _bigEndian ? readIEEEFloatBE() : readIEEEFloatLE();
	}

	double readIEEEDouble() {
		return _bigEndian ? readIEEEDoubleBE() : readIEEEDoubleLE();
	}
};

} // End of namespace Common

#endif // COMMON_READSTREAM_H
