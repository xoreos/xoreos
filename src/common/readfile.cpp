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

#include <cassert>

#include "src/common/readfile.h"
#include "src/common/error.h"
#include "src/common/ustring.h"
#include "src/common/platform.h"

namespace Common {

ReadFile::ReadFile() : _handle(0), _size(kSizeInvalid) {
}

ReadFile::ReadFile(const UString &fileName) : _handle(0), _size(kSizeInvalid) {
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
	if (!(_handle  = Platform::openFile(fileName, Platform::kFileModeRead)) ||
	    ((fileSize = getInitialSize(_handle)) < 0)) {

		close();
		return false;
	}

	if ((uint64)((unsigned long)fileSize) > (uint64)0x7FFFFFFFULL) {
		warning("ReadFile \"%s\" is too big", fileName.c_str());

		close();
		return false;
	}

	_size = (size_t)fileSize;

	return true;
}

void ReadFile::close() {
	if (_handle)
		std::fclose(_handle);

	_handle = 0;
	_size   = kSizeInvalid;
}

bool ReadFile::isOpen() const {
	return _handle != 0;
}

bool ReadFile::eos() const {
	if (!_handle)
		return true;

	return std::feof(_handle) != 0;
}

size_t ReadFile::pos() const {
	if (!_handle)
		return kPositionInvalid;

	return (size_t)std::ftell(_handle);
}

size_t ReadFile::size() const {
	return _size;
}

size_t ReadFile::seek(ptrdiff_t offset, Origin whence) {
	static const int kSeekToWhence[kOriginMAX] = { SEEK_SET, SEEK_CUR, SEEK_END };
	if (((size_t) whence) >= kOriginMAX)
		throw Exception(kSeekError);

	if (!_handle)
		throw Exception(kSeekError);

	size_t oldPos = pos();

	if (std::fseek(_handle, offset, kSeekToWhence[whence]) != 0)
		throw Exception(kSeekError);

	long p = std::ftell(_handle);
	if ((p < 0) || ((size_t)p > _size))
		throw Exception(kSeekError);

	return oldPos;
}

size_t ReadFile::read(void *dataPtr, size_t dataSize) {
	if (!_handle)
		return 0;

	assert(dataPtr);
	return std::fread(dataPtr, 1, dataSize, _handle);
}

} // End of namespace Common
