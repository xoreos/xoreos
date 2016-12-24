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
 *  Implementing the reading stream interfaces for plain memory blocks.
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

#ifndef COMMON_MEMREADSTREAM_H
#define COMMON_MEMREADSTREAM_H

#include <cstring>

#include <boost/noncopyable.hpp>

#include "src/common/types.h"
#include "src/common/disposableptr.h"
#include "src/common/readstream.h"

namespace Common {

/** Simple memory based 'stream', which implements the ReadStream interface for
 *  a plain memory block.
 */
class MemoryReadStream : boost::noncopyable, public SeekableReadStream {
public:
	/** This constructor takes a pointer to a memory buffer and a length, and
	 *  wraps it. If disposeMemory is true, the MemoryReadStream takes ownership
	 *  of the buffer and hence delete[]'s it when destructed. */
	MemoryReadStream(const byte *dataPtr, size_t dataSize, bool disposeMemory = false) :
		_ptrOrig(dataPtr, disposeMemory), _ptr(dataPtr), _size(dataSize), _pos(0), _eos(false) {

	}

	/** Create a MemoryReadStream around a static string buffer, optionally including the
	 *  terminating \0. Never disposes its memory. */
	MemoryReadStream(const char *str, bool useTerminator = false) :
		_ptrOrig(reinterpret_cast<const byte *>(str), false), _ptr(reinterpret_cast<const byte *>(str)),
		_size(strlen(str) + (useTerminator ? 1 : 0)), _pos(0), _eos(false) {

	}

	/** Template constructor to create a MemoryReadStream around a static array buffer.
	 *  Never disposes its memory. */
	template<size_t N>
	MemoryReadStream(const byte (&array)[N]) :
		_ptrOrig(array, false), _ptr(array), _size(N), _pos(0), _eos(false) {

	}

	~MemoryReadStream() { }

	size_t read(void *dataPtr, size_t dataSize);

	bool eos() const;

	size_t pos() const;
	size_t size() const;

	size_t seek(ptrdiff_t offset, Origin whence = kOriginBegin);

	const byte *getData() const;

private:
	DisposableArray<const byte> _ptrOrig;
	const byte *_ptr;

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
	MemoryReadStreamEndian(const byte *dataPtr, size_t dataSize, bool bigEndian = false,
	                       bool disposeMemory = false);
	~MemoryReadStreamEndian();

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

#endif // COMMON_MEMREADSTREAM_H
