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

#ifndef COMMON_WRITEFILE_H
#define COMMON_WRITEFILE_H

#include <cstdio>

#include <boost/noncopyable.hpp>

#include "src/common/types.h"
#include "src/common/writestream.h"

namespace Common {

class UString;

/** A simple streaming file writing class. */
class WriteFile : boost::noncopyable, public WriteStream {
public:
	WriteFile();
	WriteFile(const UString &fileName);
	~WriteFile();

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

	void flush();

	size_t write(const void *dataPtr, size_t dataSize);

	/** Return the number of bytes written to the current file in total. */
	size_t size() const;

protected:
	std::FILE *_handle; ///< The actual file handle.

	size_t _size;
};

} // End of namespace Common

#endif // COMMON_WRITEFILE_H
