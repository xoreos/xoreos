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
 *  Implementing the writing stream interfaces for memory blocks.
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

#include <cassert>
#include <cstring>
#include <cstddef>

#include "src/common/types.h"
#include "src/common/memwritestream.h"

namespace Common {

size_t MemoryWriteStream::write(const void *dataPtr, size_t dataSize) {
	assert(dataPtr);

	// Write at most as many bytes as are still available...
	if (dataSize > _bufSize - _pos)
		dataSize = _bufSize - _pos;

	std::memcpy(_ptr, dataPtr, dataSize);

	_ptr += dataSize;
	_pos += dataSize;

	return dataSize;
}

size_t MemoryWriteStream::pos() const {
	return _pos;
}

size_t MemoryWriteStream::size() const {
	return _bufSize;
}

size_t MemoryWriteStream::seek(ptrdiff_t offset, SeekableWriteStream::Origin whence) {
	assert((size_t)_pos <= _bufSize);

	const size_t oldPos = _pos;
	const size_t newPos = evalSeek(offset, whence, _pos, 0, size());
	if (newPos > _bufSize)
		throw Exception(kSeekError);

	_pos = newPos;
	_ptr = _ptr - oldPos + newPos;

	return oldPos;
}


MemoryWriteStreamDynamic::MemoryWriteStreamDynamic(bool disposeMemory, size_t capacity) :
		_data(0, disposeMemory), _ptr(0), _capacity(0), _size(0) {

	reserve(capacity);
}

MemoryWriteStreamDynamic::~MemoryWriteStreamDynamic() {
}

void MemoryWriteStreamDynamic::reserve(size_t s) {
	if (s <= _capacity)
		return;

	while (_capacity < s)
		_capacity = MAX<size_t>(2, _capacity * 2);

	byte *newData = new byte[_capacity];
	if (_data)
		memcpy(newData, _data.get(), _size);

	const size_t oldPos = pos();
	_data.dispose();
	_data.reset(newData);
	_ptr = _data.get() + oldPos;
}

void MemoryWriteStreamDynamic::ensureCapacity(size_t newLen) {
	if (newLen <= _capacity)
		return;

	reserve(newLen);
}

size_t MemoryWriteStreamDynamic::write(const void *dataPtr, size_t dataSize) {
	assert(dataPtr);

	ensureCapacity(pos() + dataSize);

	std::memcpy(_ptr, dataPtr, dataSize);

	if (pos() + dataSize > _size)
		_size = MAX(pos() + dataSize, _size);

	_ptr  += dataSize;

	return dataSize;
}

void MemoryWriteStreamDynamic::setDisposable(bool disposeMemory) {
	_data.setDisposable(disposeMemory);
}

void MemoryWriteStreamDynamic::dispose() {
	_data.dispose();

	_ptr      = 0;
	_size     = 0;
	_capacity = 0;
}

size_t MemoryWriteStreamDynamic::pos() const {
	return _ptr - _data.get();
}

size_t MemoryWriteStreamDynamic::size() const {
	return _size;
}

size_t MemoryWriteStreamDynamic::seek(ptrdiff_t offset, SeekableWriteStream::Origin whence) {
	assert((size_t)pos() <= _size);

	const size_t oldPos = pos();
	const size_t newPos = evalSeek(offset, whence, pos(), 0, size());
	if (newPos > _size)
		throw Exception(kSeekError);

	_ptr = _data.get() + newPos;

	return oldPos;
}

byte *MemoryWriteStreamDynamic::getData() {
	return _data.get();
}

} // End of namespace Common
