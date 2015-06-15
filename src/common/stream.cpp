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
 *  Basic stream interfaces.
 */

#include "src/common/stream.h"
#include "src/common/error.h"
#include "src/common/util.h"
#include "src/common/ustring.h"

namespace Common {

uint32 WriteStream::writeStream(ReadStream &stream, uint32 n) {
	uint32 haveRead = 0;

	byte buf[4096];
	while (!stream.eos() && (n > 0)) {
		uint32 toRead  = MIN<uint32>(4096, n);
		uint32 bufRead = stream.read(buf, toRead);

		write(buf, bufRead);

		n        -= bufRead;
		haveRead += bufRead;
	}

	return haveRead;
}

uint32 WriteStream::writeStream(ReadStream &stream) {
	return writeStream(stream, 0xFFFFFFFF);
}

void WriteStream::writeString(const UString &str) {
	write(str.c_str(), strlen(str.c_str()));
}


MemoryReadStream *ReadStream::readStream(uint32 dataSize) {
	byte *buf = new byte[dataSize];

	try {

		if (read(buf, dataSize) != dataSize)
			throw Exception(kReadError);

	} catch (...) {
		delete[] buf;
		throw;
	}

	return new MemoryReadStream(buf, dataSize, true);
}


uint32 MemoryReadStream::read(void *dataPtr, uint32 dataSize) {
	// Read at most as many bytes as are still available...
	if (dataSize > _size - _pos) {
		dataSize = _size - _pos;
		_eos = true;
	}
	std::memcpy(dataPtr, _ptr, dataSize);

	if (_encbyte) {
		byte *p = (byte *)dataPtr;
		byte *end = p + dataSize;
		while (p < end)
			*p++ ^= _encbyte;
	}

	_ptr += dataSize;
	_pos += dataSize;

	return dataSize;
}

void MemoryReadStream::seek(int32 offs, int whence) {
	assert(_pos <= _size);

	uint32 newPos = _pos;

	switch (whence) {
	case SEEK_END:
		// SEEK_END works just like SEEK_SET, only 'reversed',
		// i.e. from the end.
		offs = _size + offs;
		// Fall through
	case SEEK_SET:
		newPos = offs;
		break;

	case SEEK_CUR:
		newPos += offs;
		break;
	}

	if (newPos > _size)
		throw Exception(kSeekError);

	_pos = newPos;
	_ptr = _ptrOrig + newPos;

	// Reset end-of-stream flag on a successful seek
	_eos = false;
}


uint32 SeekableReadStream::seekTo(uint32 offset) {
	uint32 curPos = pos();

	seek(offset);

	return curPos;
}


uint32 SubReadStream::read(void *dataPtr, uint32 dataSize) {
	if (dataSize > _end - _pos) {
		dataSize = _end - _pos;
		_eos = true;
	}

	dataSize = _parentStream->read(dataPtr, dataSize);
	_pos += dataSize;

	return dataSize;
}

SeekableSubReadStream::SeekableSubReadStream(SeekableReadStream *parentStream, uint32 begin, uint32 end, bool disposeParentStream)
	: SubReadStream(parentStream, end, disposeParentStream),
	_parentStream(parentStream),
	_begin(begin) {
	assert(_begin <= _end);
	_pos = _begin;
	_parentStream->seek(_pos);
	_eos = false;
}

void SeekableSubReadStream::seek(int32 offset, int whence) {
	assert(_pos >= _begin);
	assert(_pos <= _end);

	uint32 newPos = _pos;

	switch (whence) {
	case SEEK_END:
		offset = size() + offset;
		// fallthrough
	case SEEK_SET:
		newPos = _begin + offset;
		break;
	case SEEK_CUR:
		newPos += offset;
	}

	if ((newPos < _begin) || (newPos > _end))
		throw Exception(kSeekError);

	_pos = newPos;

	_parentStream->seek(_pos);
	_eos = false; // reset eos on successful seek
}

} // End of namespace Common
