/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

// Largely based on the stream implementation found in ScummVM.

/** @file common/stream.cpp
 *  Basic stream interfaces.
 */

#include "common/stream.h"
#include "common/error.h"
#include "common/util.h"

namespace Common {

void WriteStream::writeStream(ReadStream &stream) {
	byte buf[4096];

	while (!stream.eos()) {
		uint32 n = stream.read(buf, 4096);

		write(buf, n);
	}
}

MemoryReadStream *ReadStream::readStream(uint32 dataSize) {
	byte *buf = new byte[dataSize];

	dataSize = read(buf, dataSize);
	assert(dataSize > 0);

	return new MemoryReadStream(buf, dataSize, DisposeAfterUse::YES);
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

bool MemoryReadStream::seek(int32 offs, int whence) {
	// Pre-Condition
	assert(_pos <= _size);
	switch (whence) {
	case SEEK_END:
		// SEEK_END works just like SEEK_SET, only 'reversed',
		// i.e. from the end.
		offs = _size + offs;
		// Fall through
	case SEEK_SET:
		_ptr = _ptrOrig + offs;
		_pos = offs;
		break;

	case SEEK_CUR:
		_ptr += offs;
		_pos += offs;
		break;
	}
	// Post-Condition
	assert(_pos <= _size);

	// Reset end-of-stream flag on a successful seek
	_eos = false;
	return true; // FIXME: STREAM REWRITE
}


uint32 SeekableReadStream::seekTo(uint32 offset) {
	uint32 curPos = pos();

	if (!seek(offset))
		throw Exception(kSeekError);

	return curPos;
}


uint32 SubReadStream::read(void *dataPtr, uint32 dataSize) {
	if (dataSize > _end - _pos) {
		dataSize = _end - _pos;
		_eos = true;
	}

	dataSize = _parentStream->read(dataPtr, dataSize);
	_eos |= _parentStream->eos();
	_pos += dataSize;

	return dataSize;
}

SeekableSubReadStream::SeekableSubReadStream(SeekableReadStream *parentStream, uint32 begin, uint32 end, DisposeAfterUse::Flag disposeParentStream)
	: SubReadStream(parentStream, end, disposeParentStream),
	_parentStream(parentStream),
	_begin(begin) {
	assert(_begin <= _end);
	_pos = _begin;
	_parentStream->seek(_pos);
	_eos = false;
}

bool SeekableSubReadStream::seek(int32 offset, int whence) {
	assert(_pos >= _begin);
	assert(_pos <= _end);

	switch (whence) {
	case SEEK_END:
		offset = size() + offset;
		// fallthrough
	case SEEK_SET:
		_pos = _begin + offset;
		break;
	case SEEK_CUR:
		_pos += offset;
	}

	assert(_pos >= _begin);
	assert(_pos <= _end);

	bool ret = _parentStream->seek(_pos);
	if (ret) _eos = false; // reset eos on successful seek

	return ret;
}

BufferedReadStream::BufferedReadStream(ReadStream *parentStream, uint32 bufSize, DisposeAfterUse::Flag disposeParentStream)
	: _parentStream(parentStream),
	_disposeParentStream(disposeParentStream),
	_pos(0),
	_bufSize(0),
	_realBufSize(bufSize) {

	assert(parentStream);
	_buf = new byte[bufSize];
	assert(_buf);
}

BufferedReadStream::~BufferedReadStream() {
	if (_disposeParentStream)
		delete _parentStream;
	delete[] _buf;
}

uint32 BufferedReadStream::read(void *dataPtr, uint32 dataSize) {
	uint32 alreadyRead = 0;
	const uint32 bufBytesLeft = _bufSize - _pos;

	// Check whether the data left in the buffer suffices....
	if (dataSize > bufBytesLeft) {
		// Nope, we need to read more data

		// First, flush the buffer, if it is non-empty
		if (0 < bufBytesLeft) {
			std::memcpy(dataPtr, _buf + _pos, bufBytesLeft);
			_pos = _bufSize;
			alreadyRead += bufBytesLeft;
			dataPtr = (byte *)dataPtr + bufBytesLeft;
			dataSize -= bufBytesLeft;
		}

		// At this point the buffer is empty. Now if the read request
		// exceeds the buffer size, just satisfy it directly.
		if (dataSize > _bufSize)
			return alreadyRead + _parentStream->read(dataPtr, dataSize);

		// Refill the buffer.
		// If we didn't read as many bytes as requested, the reason
		// is EOF or an error. In that case we truncate the buffer
		// size, as well as the number of  bytes we are going to
		// return to the caller.
		_bufSize = _parentStream->read(_buf, _realBufSize);
		_pos = 0;
		if (dataSize > _bufSize)
			dataSize = _bufSize;
	}

	// Satisfy the request from the buffer
	std::memcpy(dataPtr, _buf + _pos, dataSize);
	_pos += dataSize;
	return alreadyRead + dataSize;
}

BufferedSeekableReadStream::BufferedSeekableReadStream(SeekableReadStream *parentStream, uint32 bufSize, DisposeAfterUse::Flag disposeParentStream)
	: BufferedReadStream(parentStream, bufSize, disposeParentStream),
	_parentStream(parentStream) {
}

bool BufferedSeekableReadStream::seek(int32 offset, int whence) {
	// If it is a "local" seek, we may get away with "seeking" around
	// in the buffer only.
	// Note: We could try to handle SEEK_END and SEEK_SET, too, but
	// since they are rarely used, it seems not worth the effort.
	if (whence == SEEK_CUR && (int)_pos + offset >= 0 && _pos + offset <= _bufSize) {
		_pos += offset;
	} else {
		// Seek was not local enough, so we reset the buffer and
		// just seeks normally in the parent stream.
		if (whence == SEEK_CUR)
			offset -= (_bufSize - _pos);
		_pos = _bufSize;
		_parentStream->seek(offset, whence);
	}

	return true; // FIXME: STREAM REWRITE
}

} // End of namespace Common
