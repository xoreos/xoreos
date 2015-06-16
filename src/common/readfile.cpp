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
 *  Implementing the stream reading interfaces for files.
 */

#include "src/common/readfile.h"
#include "src/common/error.h"
#include "src/common/ustring.h"

namespace Common {

ReadFile::ReadFile() : _handle(0), _size(-1) {
}

ReadFile::ReadFile(const UString &fileName) : _handle(0), _size(-1) {
	if (!open(fileName))
		throw Exception("Can't open file \"%s\"", fileName.c_str());
}

ReadFile::~ReadFile() {
	close();
}

static long getInitialSize(std::FILE *handle) {
	if (!handle)
		return -1;

	if (std::fseek(handle, 0, SEEK_END) != 0)
		return -1;

	long fileSize = std::ftell(handle);

	if (std::fseek(handle, 0, SEEK_SET) != 0)
		return -1;

	return fileSize;
}

bool ReadFile::open(const UString &fileName) {
	close();

	long fileSize = -1;
	if (!(_handle  = std::fopen(fileName.c_str(), "rb")) ||
	    ((fileSize = getInitialSize(_handle)) < 0)) {

		close();
		return false;
	}

	if ((int64)fileSize > (int64)0x7FFFFFFF) {
		warning("ReadFile \"%s\" is too big", fileName.c_str());

		close();
		return false;
	}

	_size = (int32)fileSize;

	return true;
}

void ReadFile::close() {
	if (_handle)
		std::fclose(_handle);

	_handle =  0;
	_size   = -1;
}

bool ReadFile::isOpen() const {
	return _handle != 0;
}

bool ReadFile::eos() const {
	if (!_handle)
		return true;

	return std::feof(_handle) != 0;
}

int32 ReadFile::pos() const {
	if (!_handle)
		return -1;

	return std::ftell(_handle);
}

int32 ReadFile::size() const {
	return _size;
}

uint32 ReadFile::seek(int32 offs, Origin whence) {
	static const int kSeekToWhence[kOriginMAX] = { SEEK_SET, SEEK_CUR, SEEK_END };
	if (((size_t) whence) >= kOriginMAX)
		throw Exception(kSeekError);

	if (!_handle)
		throw Exception(kSeekError);

	uint32 oldPos = pos();

	if (std::fseek(_handle, offs, kSeekToWhence[whence]) != 0)
		throw Exception(kSeekError);

	long p = std::ftell(_handle);
	if ((p < 0) || (p > _size))
		throw Exception(kSeekError);

	return oldPos;
}

uint32 ReadFile::read(void *dataPtr, uint32 dataSize) {
	if (!_handle)
		return 0;

	return std::fread(dataPtr, 1, dataSize, _handle);
}

} // End of namespace Common
