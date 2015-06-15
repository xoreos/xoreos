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
 *  Implementing the writing stream interfaces for memory blocks.
 */

#include <cstring>

#include "src/common/types.h"
#include "src/common/memwritestream.h"

namespace Common {

MemoryWriteStream::MemoryWriteStream(byte *buf, uint32 len) : _ptr(buf), _bufSize(len), _pos(0) {
}

MemoryWriteStream::~MemoryWriteStream() {
}

uint32 MemoryWriteStream::write(const void *dataPtr, uint32 dataSize) {
	// Write at most as many bytes as are still available...
	if (dataSize > _bufSize - _pos)
		dataSize = _bufSize - _pos;

	std::memcpy(_ptr, dataPtr, dataSize);

	_ptr += dataSize;
	_pos += dataSize;

	return dataSize;
}

uint32 MemoryWriteStream::pos() const {
	return _pos;
}

uint32 MemoryWriteStream::size() const {
	return _bufSize;
}


MemoryWriteStreamDynamic::MemoryWriteStreamDynamic(bool disposeMemory, uint32 capacity) :
	_data(0), _disposeMemory(disposeMemory), _ptr(0), _pos(0), _capacity(0), _size(0) {

	reserve(capacity);
}

MemoryWriteStreamDynamic::~MemoryWriteStreamDynamic() {
	if (_disposeMemory)
		delete[] _data;
}

void MemoryWriteStreamDynamic::reserve(uint32 s) {
	if (s <= _capacity)
		return;

	byte *oldData = _data;

	_capacity = s + 32;
	_data = new byte[_capacity];
	_ptr = _data + _pos;

	if (oldData) {
		// Copy old data
		std::memcpy(_data, oldData, _size);
		delete[] oldData;
	}
}

void MemoryWriteStreamDynamic::ensureCapacity(uint32 newLen) {
	if (newLen <= _capacity)
		return;

	reserve(newLen);

	_size = newLen;
}

uint32 MemoryWriteStreamDynamic::write(const void *dataPtr, uint32 dataSize) {
	ensureCapacity(_pos + dataSize);

	std::memcpy(_ptr, dataPtr, dataSize);

	_ptr += dataSize;
	_pos += dataSize;

	if (_pos > _size)
		_size = _pos;

	return dataSize;
}

void MemoryWriteStreamDynamic::dispose() {
	delete[] _data;

	_data     = 0;
	_ptr      = 0;
	_pos      = 0;
	_size     = 0;
	_capacity = 0;
}

uint32 MemoryWriteStreamDynamic::pos() const {
	return _pos;
}

uint32 MemoryWriteStreamDynamic::size() const {
	return _size;
}

byte *MemoryWriteStreamDynamic::getData() {
	return _data;
}

} // End of namespace Common
