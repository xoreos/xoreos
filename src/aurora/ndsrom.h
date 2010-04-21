/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file aurora/ndsrom.h
 *  Nintendo DS rom parsing.
 */

#ifndef AURORA_NDSROM_H
#define AURORA_NDSROM_H

#include "common/types.h"

#include <string>
#include <map>
#include <list>

namespace Aurora {

class SeekableReadStream;

/** A class encapsulating ZIP file access. */
class NDSFile {
public:
	typedef std::list<std::string> FileList;

	NDSFile(Common::SeekableReadStream *stream);
	~NDSFile();

	/** Return the list of files contained in the ZIP archive. */
	const FileList &getFileList() const;

	/** Open a file contained in the archive. */
	Common::SeekableReadStream *open(const std::string &filename);

private:
	struct FileRecord {
		uint32 offset;
		uint32 size;
	};

	typedef std::map<std::string, FileRecord> FileMap;

	Common::SeekableReadStream *_stream;

	FileList _fileList; ///< List of file names.
	FileMap _fileMap;   ///< Map of files.
};

} // End of namespace Aurora

#endif
