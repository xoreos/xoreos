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
	MemoryWriteStream(byte *buf, uint32 len);
	~MemoryWriteStream();

	uint32 write(const void *dataPtr, uint32 dataSize);

	uint32 pos() const;
	uint32 size() const;

private:
	byte *_ptr;

	const uint32 _bufSize;
	uint32 _pos;
};

/** A sort of hybrid between MemoryWriteStream and Array classes. A stream
 *  that grows as it's written to.
 */
class MemoryWriteStreamDynamic : public WriteStream {
public:
	MemoryWriteStreamDynamic(bool disposeMemory = false, uint32 capacity = 0);
	~MemoryWriteStreamDynamic();

	void reserve(uint32 s);

	uint32 write(const void *dataPtr, uint32 dataSize);

	void dispose();

	uint32 pos() const;
	uint32 size() const;

	byte *getData();

private:
	byte *_data;

	bool _disposeMemory;

	byte *_ptr;
	uint32 _pos;

	uint32 _capacity;
	uint32 _size;

	void ensureCapacity(uint32 newLen);
};

} // End of namespace Common

#endif // COMMON_MEMWRITESTREAM_H
