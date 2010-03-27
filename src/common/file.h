/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and Bioware corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

#ifndef COMMON_FILE_H
#define COMMON_FILE_H

#include <cstdio>

#include "types.h"
#include "stream.h"
#include "noncopyable.h"

namespace Common {

/** A simple streaming file class. */
class File : public SeekableReadStream, public NonCopyable {
public:
	File();
	virtual ~File();

	/**
	 * Checks if a given file exists.
	 *
	 * @param  fileName the file to check for
	 * @return true if the file exists, false otherwise
	 */
	static bool exists(const std::string &fileName);

	/**
	 * Try to open the file with the given fileName.
	 * @note Must not be called if this file already is open (i.e. if isOpen returns true).
	 *
	 * @param  fileName the name of the file to open
	 * @return true if file was opened successfully, false otherwise
	 */
	bool open(const std::string &fileName);

	/**
	 * Close the file, if open.
	 */
	void close();

	/**
	 * Checks if the object opened a file successfully.
	 *
	 * @return true if any file is opened, false otherwise.
	 */
	bool isOpen() const;

	bool err() const; // implement abstract Stream method
	void clearErr();  // implement abstract Stream method
	bool eos() const; // implement abstract SeekableReadStream method

	int32 pos() const;  // implement abstract SeekableReadStream method
	int32 size() const; // implement abstract SeekableReadStream method
	bool seek(int32 offs, int whence = SEEK_SET); // implement abstract SeekableReadStream method
	uint32 read(void *dataPtr, uint32 dataSize);  // implement abstract SeekableReadStream method

protected:
	std::FILE *_handle; ///< The actual file handle.
	int32 _size;   ///< The file's size.
};

} // End of namespace Common

#endif
