/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

#ifndef COMMON_ZIPFILE_H
#define COMMON_ZIPFILE_H

#include "common/types.h"

#include <string>
#include <map>
#include <list>

namespace Common {

class SeekableReadStream;

class ZipFile {
public:
	ZipFile(SeekableReadStream *stream);
	~ZipFile();

	typedef std::list<std::string> FileList;

	SeekableReadStream *open(const std::string &filename);
	const FileList &getFileList() const;

private:
	SeekableReadStream *_stream;

	struct FileRecord {
		uint16 compMethod;
		uint32 compSize;
		uint32 size;
		uint32 offset;
	};

	SeekableReadStream *decompressFile(const FileRecord &fileRecord);

	typedef std::map<std::string, FileRecord> FileMap;

	FileMap _fileMap;
	FileList _fileList;
};

} // End of namespace Common

#endif
