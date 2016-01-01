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

#ifndef COMMON_MEMWRITESTREAM_H
#define COMMON_MEMWRITESTREAM_H

#include "src/common/types.h"
#include "src/common/writestream.h"

namespace Common {

/** Simple memory based 'stream', which implements the WriteStream interface for
 *  a plain memory block.
 */
class MemoryWriteStream : public WriteStream {
public:
	MemoryWriteStream(byte *buf, size_t len);
	~MemoryWriteStream();

	size_t write(const void *dataPtr, size_t dataSize);

	size_t pos() const;
	size_t size() const;

private:
	byte *_ptr;

	const size_t _bufSize;
	size_t _pos;
};

/** A sort of hybrid between MemoryWriteStream and Array classes. A stream
 *  that grows as it's written to.
 */
class MemoryWriteStreamDynamic : public WriteStream {
public:
	MemoryWriteStreamDynamic(bool disposeMemory = false, size_t capacity = 0);
	~MemoryWriteStreamDynamic();

	void reserve(size_t s);

	size_t write(const void *dataPtr, size_t dataSize);

	void dispose();

	size_t pos() const;
	size_t size() const;

	byte *getData();

private:
	byte *_data;

	bool _disposeMemory;

	byte *_ptr;
	size_t _pos;

	size_t _capacity;
	size_t _size;

	void ensureCapacity(size_t newLen);
};

} // End of namespace Common

#endif // COMMON_MEMWRITESTREAM_H
