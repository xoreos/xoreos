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
 *  Implementing the stream writing interfaces for files.
 */

#include <cassert>

#include "src/common/writefile.h"
#include "src/common/error.h"
#include "src/common/ustring.h"
#include "src/common/platform.h"
#include "src/common/filepath.h"

namespace Common {

WriteFile::WriteFile() : _handle(0), _size(0) {
}

WriteFile::WriteFile(const UString &fileName) : _handle(0), _size(0) {
	if (!open(fileName))
		throw Exception("Can't open file \"%s\" for writing", fileName.c_str());
}

WriteFile::~WriteFile() {
	try {
		close();
	} catch (...) {
	}
}

bool WriteFile::open(const UString &fileName) {
	close();

	UString path = FilePath::normalize(fileName);
	if (path.empty())
		return false;

	try {
		FilePath::createDirectories(FilePath::getDirectory(path));
	} catch (...) {
		return false;
	}

	if (!(_handle = Platform::openFile(path, Platform::kFileModeWrite)))
		return false;

	return true;
}

void WriteFile::close() {
	flush();

	if (_handle)
		std::fclose(_handle);

	_handle = 0;
	_size   = 0;
}

bool WriteFile::isOpen() const {
	return _handle != 0;
}

void WriteFile::flush() {
	if (!_handle)
		return;

	if (std::fflush(_handle) != 0)
		throw Exception(kWriteError);
}

size_t WriteFile::write(const void *dataPtr, size_t dataSize) {
	if (!_handle)
		return 0;

	assert(dataPtr);

	const size_t written = std::fwrite(dataPtr, 1, dataSize, _handle);
	_size += written;

	return written;
}

size_t WriteFile::size() const {
	return _size;
}

} // End of namespace Common
