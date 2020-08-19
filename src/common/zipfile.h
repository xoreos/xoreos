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
 *  ZIP file decompression.
 */

#ifndef COMMON_ZIPFILE_H
#define COMMON_ZIPFILE_H

#include <list>
#include <vector>

#include <memory>

#include <boost/noncopyable.hpp>

#include "src/common/types.h"
#include "src/common/ustring.h"

namespace Common {

class SeekableReadStream;

/** A class encapsulating ZIP file access. */
class ZipFile : boost::noncopyable {
public:
	/** A file. */
	struct File {
		UString name; ///< The file's name.
		uint32_t index; ///< The file's local index within the ZIP.
	};

	typedef std::list<File> FileList;

	ZipFile(SeekableReadStream *zip);
	~ZipFile();

	/** Return the list of files. */
	const FileList &getFiles() const;

	/** Return the size of a file. */
	size_t getFileSize(uint32_t index) const;

	/** Return a stream of the file's contents. */
	SeekableReadStream *getFile(uint32_t index, bool tryNoCopy = false) const;

private:
	/** Internal file information. */
	struct IFile {
		uint32_t offset; ///< The offset of the file within the ZIP.
		uint32_t size;   ///< The file's size.
	};

	typedef std::vector<IFile> IFileList;

	std::unique_ptr<SeekableReadStream> _zip;

	/** External list of file names and types. */
	FileList _files;

	/** Internal list of file offsets and sizes. */
	IFileList _iFiles;

	void load(SeekableReadStream &zip);

	static SeekableReadStream *decompressFile(SeekableReadStream &zip, uint32_t method,
			uint32_t compSize, uint32_t realSize);

	const IFile &getIFile(uint32_t index) const;
	void getFileProperties(SeekableReadStream &zip, const IFile &file,
			uint16_t &compMethod, uint32_t &compSize, uint32_t &realSize) const;
};

} // End of namespace Common

#endif // COMMON_ZIPFILE_H
