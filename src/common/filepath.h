/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

#ifndef COMMON_FILEPATH_H
#define COMMON_FILEPATH_H

#include <string>

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
