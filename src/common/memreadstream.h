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

// Largely based on the stream implementation found in ScummVM.

/** @file
 *  Implementing the reading stream interfaces for plain memory blocks.
 */

#ifndef COMMON_MEMREADSTREAM_H
#define COMMON_MEMREADSTREAM_H

#include <cstring>

#include "src/common/types.h"
#include "src/common/readstream.h"

namespace Common {

/** Simple memory based 'stream', which implements the ReadStream interface for
 *  a plain memory block.
 */
class MemoryReadStream : public SeekableReadStream {
public:
	/** This constructor takes a pointer to a memory buffer and a length, and
	 *  wraps it. If disposeMemory is true, the MemoryReadStream takes ownership
	 *  of the buffer and hence delete[]'s it when destructed.
	 */
	MemoryReadStream(const byte *dataPtr, size_t dataSize, bool disposeMemory = false);
	~MemoryReadStream();

	size_t read(void *dataPtr, size_t dataSize);

	bool eos() const;

	size_t pos() const;
	size_t size() const;

	size_t seek(ptrdiff_t offset, Origin whence = kOriginBegin);

	const byte *getData() const;

private:
	const byte * const _ptrOrig;
	const byte *_ptr;

	bool _disposeMemory;

	const size_t _size;

	size_t _pos;

	bool _eos;
};


/** This is a wrapper around MemoryReadStream, but it adds non-endian
 *  read methods whose endianness is set on the stream creation.
 */
class MemoryReadStreamEndian : public MemoryReadStream {
private:
	const bool _bigEndian;

public:
	MemoryReadStreamEndian(const byte *buf, size_t len, bool bigEndian = false);
	~MemoryReadStreamEndian();

	uint16 readUint16() {
		uint16 val;
		read(&val, 2);
		return (_bigEndian) ? TO_BE_16(val) : TO_LE_16(val);
	}

	uint32 readUint32() {
		uint32 val;
		read(&val, 4);
		return (_bigEndian) ? TO_BE_32(val) : TO_LE_32(val);
	}

	uint64 readUint64() {
		uint64 val;
		read(&val, 8);
		return (_bigEndian) ? TO_BE_64(val) : TO_LE_64(val);
	}

	FORCEINLINE int16 readSint16() {
		return (int16)readUint16();
	}

	FORCEINLINE int32 readSint32() {
		return (int32)readUint32();
	}

	FORCEINLINE int64 readSint64() {
		return (int64)readUint64();
	}
};

} // End of namespace Common

#endif // COMMON_MEMREADSTREAM_H
