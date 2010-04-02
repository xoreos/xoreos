/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
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

#include <string>

#include "boost/filesystem.hpp"

#include "common/types.h"

namespace Common {

/** Utility class for manipulating file paths. */
class FilePath {
public:
	/** Does specified path exist and is it a regular file?
	 *
	 *  @param  p The path to look up.
	 *  @return true if the path exists and is a regular file, false otherwise.
	 */
	static bool isRegularFile(const std::string &p);

	/** Does specified path exist and is it a directory?
	 *
	 *  @param  p The path to look up.
	 *  @return true if the path exists and is a directory, false otherwise.
	 */
	static bool isDirectory(const std::string &p);

	/** Return a file name's stem.
	 *
	 *  Example: "/path/to/file.ext" -> "file"
	 *
	 *  @param  p The path to manipulate.
	 *  @return The path's stem.
	 */
	static std::string getStem(const std::string &p);

	/** Return a file name's extension.
	 *
	 *  Example: "/path/to/file.ext" -> ".ext"
	 *
	 *  @param  p The path to manipulate.
	 *  @return The path's extension.
	 */
	static std::string getExtension(const std::string &p);

	/** Change a file name's extension.
	 *
	 *  Example: "/path/to/file.ext", ".bar" -> "/path/to/file.bar"
	 *
	 *  @param  p The path to manipulate.
	 *  @param  ext The path's new extension.
	 *  @return The new path.
	 */
	static std::string changeExtension(const std::string &p, const std::string &ext = "");

	/** Normalize a path.
	 *
	 *  A normalized path contains no consecutive '/' and starts with either "/" or "./".
	 *
	 *  @param  p The path to normalize.
	 *  @return The normalized path.
	 */
	static boost::filesystem::path normalize(const boost::filesystem::path &p);

	/** Normalize a path.
	 *
	 *  A normalized path contains no consecutive '/' and starts with either "/" or "./".
	 *
	 *  @param  p The path to normalize.
	 *  @return The normalized path.
	 */
	static std::string normalize(const std::string &p);

	/** Find a directory's subdirectory.
	 *
	 *  @param  directory The directory in which to look.
	 *  @param  subDirectory The subdirectory to find.
	 *  @param  caseInsensitive Should the case of the subdirectory be ignored?
	 *  @return The path of the subdirectory or "" if not found.
	 */
	static std::string findSubDirectory(const std::string &directory, const std::string &subDirectory,
	                                    bool caseInsensitive = false);
};

} // End of namespace Common

#endif // COMMON_FILEPATH_H
