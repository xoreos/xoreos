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
 *  Basic reading stream interfaces.
 */

#include <cassert>

#include "src/common/readstream.h"
#include "src/common/memreadstream.h"
#include "src/common/error.h"

namespace Common {

ReadStream::ReadStream() {
}

ReadStream::~ReadStream() {
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


SeekableReadStream::SeekableReadStream() {
}

SeekableReadStream::~SeekableReadStream() {
}

uint32 SeekableReadStream::evalSeek(int32 offset, Origin whence, uint32 pos, uint32 begin, int32 size) {
	switch (whence) {
		case kOriginEnd:
			offset = size + offset;
			// fallthrough
		case kOriginBegin:
			return begin + offset;
		case kOriginCurrent:
			return pos + offset;

		default:
			break;
	}

	throw Exception("Invalid whence (%d)", (int) whence);
}


SubReadStream::SubReadStream(ReadStream *parentStream, uint32 end, bool disposeParentStream) :
	_parentStream(parentStream), _disposeParentStream(disposeParentStream),
	_pos(0), _end(end), _eos(false) {

	assert(parentStream);
}

SubReadStream::~SubReadStream() {
	if (_disposeParentStream)
		delete _parentStream;
}

bool SubReadStream::eos() const {
	return _eos | _parentStream->eos();
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


SeekableSubReadStream::SeekableSubReadStream(SeekableReadStream *parentStream, uint32 begin, uint32 end,
                                             bool disposeParentStream) :
	SubReadStream(parentStream, end, disposeParentStream), _parentStream(parentStream), _begin(begin) {

	assert(_begin <= _end);

	_pos = begin;
	_parentStream->seek(_pos);
}

SeekableSubReadStream::~SeekableSubReadStream() {
}

int32 SeekableSubReadStream::pos() const {
	return _pos - _begin;
}

int32 SeekableSubReadStream::size() const {
	return _end - _begin;
}

uint32 SeekableSubReadStream::seek(int32 offset, Origin whence) {
	assert(_pos >= _begin);
	assert(_pos <= _end);

	const uint32 oldPos = _pos;
	const uint32 newPos = evalSeek(offset, whence, _pos, _begin, size());
	if ((newPos < _begin) || (newPos > _end))
		throw Exception(kSeekError);

	_pos = newPos;

	_parentStream->seek(_pos);
	_eos = false; // reset eos on successful seek

	return oldPos;
}


SeekableSubReadStreamEndian::SeekableSubReadStreamEndian(SeekableReadStream *parentStream,
		uint32 begin, uint32 end, bool bigEndian, bool disposeParentStream) :
		SeekableSubReadStream(parentStream, begin, end, disposeParentStream), _bigEndian(bigEndian) {

}

SeekableSubReadStreamEndian::~SeekableSubReadStreamEndian() {
}

} // End of namespace Common
