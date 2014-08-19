/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

/** @file common/zipfile.h
 *  ZIP file decompresssion.
 */

#ifndef COMMON_ZIPFILE_H
#define COMMON_ZIPFILE_H

#include "common/types.h"
#include "common/ustring.h"

#include <list>
#include <vector>

namespace Common {

class SeekableReadStream;
class File;

/** A class encapsulating ZIP file access. */
class ZipFile {
public:
	/** A file. */
	struct File {
		UString name; ///< The file's name.
		uint32 index; ///< The file's local index within the ZIP.
	};

	typedef std::list<File> FileList;

	ZipFile(const UString &fileName);
	~ZipFile();

	/** Clear the file list. */
	void clear();

	/** Return the list of files. */
	const FileList &getFiles() const;

	/** Return the size of a file. */
	uint32 getFileSize(uint32 index) const;

	/** Return a stream of the files's contents. */
	SeekableReadStream *getFile(uint32 index) const;

private:
	/** Internal file information. */
	struct IFile {
		uint32 offset; ///< The offset of the file within the ZIP.
		uint32 size;   ///< The file's size.
	};

	typedef std::vector<IFile> IFileList;

	/** External list of file names and types. */
	FileList _files;

	/** Internal list of file offsets and sizes. */
	IFileList _iFiles;

	/** The name of the ZIP file. */
	UString _fileName;

	void open(Common::File &file) const;

	void load();
	uint32 findCentralDirectoryEnd(SeekableReadStream &zip);

	static SeekableReadStream *decompressFile(SeekableReadStream &zip, uint32 method,
			uint32 compSize, uint32 realSize);

	const IFile &getIFile(uint32 index) const;
	void getFileProperties(Common::SeekableReadStream &zip, const IFile &file,
			uint16 &compMethod, uint32 &compSize, uint32 &realSize) const;
};

} // End of namespace Common

#endif
