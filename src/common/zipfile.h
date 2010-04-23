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
#include "common/ustring.h"

#include <list>

namespace Common {

class SeekableReadStream;

/** A class encapsulating ZIP file access. */
class ZipFile {
public:
	/** A file. */
	struct File {
		UString name;  ///< The file's name.

		uint32 offset; ///< The file's offset within the ZIP.
		uint32 size;   ///< The file's size.
	};

	typedef std::list<File> FileList;

	ZipFile();
	~ZipFile();

	/** Clear all file information. */
	void clear();

	/** Load a ZIP file.
	 *
	 *  @param zip A stream of a ZIP file.
	 */
	void load(SeekableReadStream &zip);

	/** Return a list of all containing files. */
	const FileList &getFiles() const;

	/** Return a stream of the file found at this offset. */
	static SeekableReadStream *getFile(SeekableReadStream &stream, uint32 offset);

private:
	FileList _files; ///< All containing files.

	static SeekableReadStream *decompressFile(SeekableReadStream &zip, uint32 method,
			uint32 compSize, uint32 realSize);

	uint32 findCentralDirectoryEnd(SeekableReadStream &zip);
};

} // End of namespace Common

#endif
