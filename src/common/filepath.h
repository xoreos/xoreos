/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file common/filepath.h
 *  Utility class for manipulating file paths.
 */

#ifndef COMMON_FILEPATH_H
#define COMMON_FILEPATH_H

#include "boost/filesystem.hpp"

#include "common/types.h"
#include "common/ustring.h"

namespace Common {

/** Utility class for manipulating file paths. */
class FilePath {
public:
	/** Does specified path exist and is it a regular file?
	 *
	 *  @param  p The path to look up.
	 *  @return true if the path exists and is a regular file, false otherwise.
	 */
	static bool isRegularFile(const UString &p);

	/** Does specified path exist and is it a directory?
	 *
	 *  @param  p The path to look up.
	 *  @return true if the path exists and is a directory, false otherwise.
	 */
	static bool isDirectory(const UString &p);

	/** Return a file name's stem.
	 *
	 *  Example: "/path/to/file.ext" -> "file"
	 *
	 *  @param  p The path to manipulate.
	 *  @return The path's stem.
	 */
	static UString getStem(const UString &p);

	/** Return a file name's extension.
	 *
	 *  Example: "/path/to/file.ext" -> ".ext"
	 *
	 *  @param  p The path to manipulate.
	 *  @return The path's extension.
	 */
	static UString getExtension(const UString &p);

	/** Change a file name's extension.
	 *
	 *  Example: "/path/to/file.ext", ".bar" -> "/path/to/file.bar"
	 *
	 *  @param  p The path to manipulate.
	 *  @param  ext The path's new extension.
	 *  @return The new path.
	 */
	static UString changeExtension(const UString &p, const UString &ext = "");

	/** Normalize a path.
	 *
	 *  A normalized path contains no consecutive '/', uses '/' as a directory path separator,
	 *  starts with either "/", "[A-Za-z]:/" or "./" and does not end with a '/'.
	 *
	 *  @param  p The path to normalize.
	 *  @return The normalized path.
	 */
	static boost::filesystem::path normalize(const boost::filesystem::path &p);

	/** Normalize a path.
	 *
	 *  A normalized path contains no consecutive '/', uses '/' as a directory path separator
	 *  and and starts either "/", "[A-Za-z]:/" or "./".
	 *
	 *  @param  p The path to normalize.
	 *  @return The normalized path.
	 */
	static UString normalize(const UString &p);

	/** Is the given string an absolute path?
	 *
	 *  @param  p The path to check.
	 *  @return true if the string describes an absolute path, false otherwise.
	 */
	static bool isAbsolute(const UString &p);

	/** Return the absolute path.
	 *
	 *  If the path is already absolute, just return that path. If not, interpret it
	 *  as relative to the program starting path and then return an absolute path of that.
	 *
	 *  @param  p The path to absolutize.
	 *  @return The absolutized path.
	 */
	static boost::filesystem::path makeAbsolute(const boost::filesystem::path &p);

	/** Return the absolute path.
	 *
	 *  If the path is already absolute, just return that path. If not, interpret it
	 *  as relative to the program starting path and then return an absolute path of that.
	 *
	 *  @param  p The path to absolutize.
	 *  @return The absolutized path.
	 */
	static UString makeAbsolute(const UString &p);

	/** Find a directory's subdirectory.
	 *
	 *  @param  directory The directory in which to look.
	 *  @param  subDirectory The subdirectory to find.
	 *  @param  caseInsensitive Should the case of the subdirectory be ignored?
	 *  @return The path of the subdirectory or "" if not found.
	 */
	static UString findSubDirectory(const UString &directory, const UString &subDirectory,
	                                bool caseInsensitive = false);
};

} // End of namespace Common

#endif // COMMON_FILEPATH_H
