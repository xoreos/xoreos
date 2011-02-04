/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
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
};

} // End of namespace Common

#endif
