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

#include "src/common/system.h"

#if defined(WIN32)
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <wchar.h>

	#include "src/common/encoding.h"
	#include "src/common/memreadstream.h"
#endif

#include "src/common/writefile.h"
#include "src/common/error.h"
#include "src/common/ustring.h"
#include "src/common/filepath.h"

namespace Common {

#if defined(WIN32)

/* On Windows, we have to convert the UTF-8 filename to UTF-16 and use _wfopen,
 * otherwise we can'to open files with non-ASCII characters in their names. */
static inline std::FILE *openFile(const UString &fileName) {
	MemoryReadStream *utf16Name = convertString(fileName, kEncodingUTF16LE);

	std::FILE *file = _wfopen((const wchar_t *) utf16Name->getData(), L"wb");

	delete utf16Name;

	return file;
}

#else

/* On non-Windows, we just open the files as-is. */
static inline std::FILE *openFile(const UString &fileName) {
	return std::fopen(fileName.c_str(), "wb");
}

#endif


WriteFile::WriteFile() : _handle(0) {
}

WriteFile::WriteFile(const UString &fileName) : _handle(0) {
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

	FilePath::createDirectories(FilePath::getDirectory(path));

	if (!(_handle = openFile(path)))
		return false;

	return true;
}

void WriteFile::close() {
	flush();

	if (_handle)
		std::fclose(_handle);

	_handle = 0;
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

	return std::fwrite(dataPtr, 1, dataSize, _handle);
}

} // End of namespace Common
