/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
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

#include <string>
#include <map>
#include <list>

namespace Common {

class SeekableReadStream;

/** A class encapsulating ZIP file access. */
class ZipFile {
public:
	typedef std::list<std::string> FileList;

	ZipFile(SeekableReadStream *stream);
	~ZipFile();

	/** Return the list of files contained in the ZIP archive. */
	const FileList &getFileList() const;

	/** Open a file contained in the archive. */
	SeekableReadStream *open(const std::string &filename);

private:
	struct FileRecord {
		uint16 compMethod;
		uint32 compSize;
		uint32 size;
		uint32 offset;
	};

	typedef std::map<std::string, FileRecord> FileMap;

	SeekableReadStream *_stream;

	FileList _fileList; ///< List of file names.
	FileMap  _fileMap;  ///< Map of files.

	SeekableReadStream *decompressFile(const FileRecord &fileRecord);
};

} // End of namespace Common

#endif
