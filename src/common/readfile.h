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
 *  Implementing the stream reading interfaces for files.
 */

#ifndef COMMON_READFILE_H
#define COMMON_READFILE_H

#include <cstdio>

#include <boost/noncopyable.hpp>

#include "src/common/types.h"
#include "src/common/readstream.h"

namespace Common {

class UString;

/** A simple streaming file reading class. */
class ReadFile : boost::noncopyable, public SeekableReadStream {
public:
	ReadFile();
	ReadFile(const UString &fileName);
	~ReadFile();

	/** Try to open the file with the given fileName.
	 *
	 *  @param  fileName the name of the file to open
	 *  @return true if file was opened successfully, false otherwise
	 */
	bool open(const UString &fileName);

	/** Close the file, if open. */
	void close();

	/** Checks if the object opened a file successfully.
	 *
	 *  @return true if any file is opened, false otherwise.
	 */
	bool isOpen() const;

	bool eos() const;

	size_t pos() const;
	size_t size() const;

	size_t seek(ptrdiff_t offset, Origin whence = kOriginBegin);
	size_t read(void *dataPtr, size_t dataSize);

protected:
	std::FILE *_handle; ///< The actual file handle.
	size_t _size;       ///< The file's size.
};

} // End of namespace Common

#endif // COMMON_READFILE_H
