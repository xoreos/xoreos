/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

#include "boost/filesystem.hpp"
#include "boost/algorithm/string.hpp"

#include "common/filepath.h"

// boost-filesystem stuff
using boost::filesystem::path;
using boost::filesystem::exists;
using boost::filesystem::is_regular_file;
using boost::filesystem::is_directory;
using boost::filesystem::directory_iterator;

// boost-string_algo
using boost::equals;
using boost::iequals;

namespace Common {

bool FilePath::isRegularFile(const std::string &p) {
	return (exists(p) && is_regular_file(p));
}

bool FilePath::isDirectory(const std::string &p) {
	return (exists(p) && is_directory(p));
}

std::string FilePath::findSubDirectory(const std::string &directory, const std::string &subDirectory,
		bool caseInsensitive) {

	if (!exists(directory) || !is_directory(directory))
		// Path is either no directory or doesn't exist
		return "";

	try {
		path dirPath(directory);
		path subDirPath(subDirectory);

		// Iterator over the directory's contents
		directory_iterator itEnd;
		for (directory_iterator itDir(dirPath); itDir != itEnd; ++itDir) {
			if (is_directory(itDir->status())) {
				// It's a directory. Recurse into it if the depth limit wasn't yet reached

				if (caseInsensitive) {
					if (iequals(itDir->path().filename(), subDirectory))
						return itDir->path().string();
				} else {
					if (equals(itDir->path().filename(), subDirectory))
						return itDir->path().string();
				}
			}
		}
	} catch (...) {
	}

	return "";
}

} // End of namespace Common
