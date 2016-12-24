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

#include <cassert>
#include <cstring>

#include "src/common/memreadstream.h"
#include "src/common/error.h"
#include "src/common/util.h"

namespace Common {

size_t MemoryReadStream::read(void *dataPtr, size_t dataSize) {
	assert(dataPtr);

	// Read at most as many bytes as are still available...
	if (dataSize > _size - _pos) {
		dataSize = _size - _pos;
		_eos = true;
	}
	std::memcpy(dataPtr, _ptr, dataSize);

	_ptr += dataSize;
	_pos += dataSize;

	return dataSize;
}

size_t MemoryReadStream::seek(ptrdiff_t offset, Origin whence) {
	assert((size_t)_pos <= _size);

	const size_t oldPos = _pos;
	const size_t newPos = evalSeek(offset, whence, _pos, 0, size());
	if (newPos > _size)
		throw Exception(kSeekError);

	_pos = newPos;
	_ptr = _ptrOrig.get() + newPos;

	// Reset end-of-stream flag on a successful seek
	_eos = false;

	return oldPos;
}

bool MemoryReadStream::eos() const {
	return _eos;
}

size_t MemoryReadStream::pos() const {
	return _pos;
}

size_t MemoryReadStream::size() const {
	return _size;
}

const byte *MemoryReadStream::getData() const {
	return _ptrOrig.get();
}


MemoryReadStreamEndian::MemoryReadStreamEndian(const byte *dataPtr, size_t dataSize,
                                               bool bigEndian, bool disposeMemory) :
	MemoryReadStream(dataPtr, dataSize, disposeMemory), _bigEndian(bigEndian) {

}

MemoryReadStreamEndian::~MemoryReadStreamEndian() {
}

} // End of namespace Common
