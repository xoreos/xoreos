/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

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

std::string FilePath::getStem(const std::string &p) {
	path file(p);

	return file.stem();
}

std::string FilePath::getExtension(const std::string &p) {
	path file(p);

	return file.extension();
}

std::string FilePath::changeExtension(const std::string &p, const std::string &ext) {
	path file(p);

	file.replace_extension(ext);

	return file.string();
}

path FilePath::normalize(const path &p) {
	return path(normalize(p.string()));
}

std::string FilePath::normalize(const std::string &p) {
	std::string norm;

	// To at least the path + "./"
	norm.reserve(p.size() + 3);

	// Make sure there's a path qualifier
	const char *s = p.c_str();
	if (!((s[0] == '/') || ((s[0] == '.') && (s[1] == '/'))))
		norm += "./";

	// Remove consecutive '/'
	bool hasSlash = !norm.empty();
	for (; *s; s++) {
		if (*s == '/') {
			// Only append the '/' if the last character wasn't one as well

			if (!hasSlash)
				norm += *s;

			hasSlash = true;
			continue;
		}

		// Append the character
		norm += *s;
		hasSlash = false;
	}

	return norm;
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
