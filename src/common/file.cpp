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
 *  File classes implementing the stream interfaces.
 */

#include "src/common/file.h"
#include "src/common/error.h"
#include "src/common/ustring.h"
#include "src/common/filepath.h"

namespace Common {

File::File() : _handle(0), _size(-1) {
}

File::File(const UString &fileName) : _handle(0), _size(-1) {
	if (!open(fileName))
		throw Exception("Can't open file \"%s\"", fileName.c_str());
}

File::~File() {
	close();
}

bool File::exists(const UString &fileName) {
	File file;

	if (!file.open(fileName))
		return false;

	file.close();
	return true;
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

bool File::open(const UString &fileName) {
	long fileSize = -1;
	if (!(_handle  = std::fopen(fileName.c_str(), "rb")) ||
	    ((fileSize = getInitialSize(_handle)) < 0)) {

		close();
		return false;
	}

	if ((int64)fileSize > (int64)0x7FFFFFFF) {
		warning("File \"%s\" is too big", fileName.c_str());

		close();
		return false;
	}

	_size = (int32)fileSize;

	return true;
}

void File::close() {
	if (_handle)
		std::fclose(_handle);

	_handle =  0;
	_size   = -1;
}

bool File::isOpen() const {
	return _handle != 0;
}

bool File::eos() const {
	if (!_handle)
		return true;

	return std::feof(_handle) != 0;
}

int32 File::pos() const {
	if (!_handle)
		return -1;

	return std::ftell(_handle);
}

int32 File::size() const {
	return _size;
}

uint32 File::seek(int32 offs, int whence) {
	if (!_handle)
		throw Exception(kSeekError);

	uint32 oldPos = pos();

	if (std::fseek(_handle, offs, whence) != 0)
		throw Exception(kSeekError);

	long p = std::ftell(_handle);
	if ((p < 0) || (p > _size))
		throw Exception(kSeekError);

	return oldPos;
}

uint32 File::read(void *dataPtr, uint32 dataSize) {
	if (!_handle)
		return 0;

	return std::fread(dataPtr, 1, dataSize, _handle);
}


DumpFile::DumpFile() : _handle(0), _size(-1) {
}

DumpFile::DumpFile(const UString &fileName) : _handle(0), _size(-1) {
	if (!open(fileName))
		throw Exception("Can't open file \"%s\" for writing", fileName.c_str());
}

DumpFile::~DumpFile() {
	close();
}

bool DumpFile::open(const UString &fileName) {
	UString path = FilePath::normalize(fileName);
	if (path.empty())
		return false;

	FilePath::createDirectories(FilePath::getDirectory(path));

	if (!(_handle = std::fopen(path.c_str(), "wb")))
		return false;

	_size = 0;

	return true;
}

void DumpFile::close() {
	flush();

	if (_handle)
		std::fclose(_handle);

	_handle =  0;
	_size   = -1;
}

bool DumpFile::isOpen() const {
	return _handle != 0;
}

void DumpFile::flush() {
	if (!_handle)
		return;

	if (std::fflush(_handle) != 0)
		throw Exception(kWriteError);
}

uint32 DumpFile::write(const void *dataPtr, uint32 dataSize) {
	if (!_handle)
		return 0;

	return std::fwrite(dataPtr, 1, dataSize, _handle);
}

} // End of namespace Common
