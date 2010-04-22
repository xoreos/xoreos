/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file common/filepath.cpp
 *  Utility class for manipulating file paths.
 */

#include "boost/algorithm/string.hpp"
#include "boost/system/config.hpp"

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

bool FilePath::isRegularFile(const UString &p) {
	return (exists(p.c_str()) && is_regular_file(p.c_str()));
}

bool FilePath::isDirectory(const UString &p) {
	return (exists(p.c_str()) && is_directory(p.c_str()));
}

UString FilePath::getStem(const UString &p) {
	path file(p.c_str());

	return file.stem();
}

UString FilePath::getExtension(const UString &p) {
	path file(p.c_str());

	return file.extension();
}

UString FilePath::changeExtension(const UString &p, const UString &ext) {
	path file(p.c_str());

	file.replace_extension(ext.c_str());

	return file.string();
}

path FilePath::normalize(const boost::filesystem::path &p) {
	UString ustring = p.string();

	return path(normalize(ustring).c_str());
}

UString FilePath::normalize(const UString &p) {
	UString norm;

	// Make sure there's a path qualifier
	if (!isAbsolute(p)) {
		UString::iterator it = p.begin();

		if ((it == p.end()) || (*it != '.')) {
			norm += "./";
		} else {
			++it;
			if ((it == p.end()) || (*it != '/'))
				norm += "./";
		}
	}

	// Remove consecutive '/'
	bool hasSlash = !norm.empty();
	for (UString::iterator it = p.begin(); it != p.end(); ++it) {
		if ((*it == '/') || (*it == '\\')) {
			// Only append the '/' if the last character wasn't one as well and this
			// is not the final character.

			if (!hasSlash && (it != --p.end()))
				norm += '/';

			hasSlash = true;
			continue;
		}

		// Append the character
		norm += *it;
		hasSlash = false;
	}

	return norm;
}

bool FilePath::isAbsolute(const UString &p) {
	if (p.empty())
		return false;

#if defined(BOOST_WINDOWS_API)
	if (p.size() >= 3) {
		UString::iterator it = p.begin();
		if (isalpha(*it))
			if (*++it == ':') {
				++it;
				if ((*it == '/') || (*it == '\\'))
					return true;
			}
	}
#elif defined(BOOST_POSIX_API)
	if (*p.begin() == '/')
		return true;
#else
	#error Neither BOOST_WINDOWS_API nor BOOST_POSIX_API defined
#endif

	return false;
}

boost::filesystem::path FilePath::makeAbsolute(const boost::filesystem::path &p) {
	UString ustring = p.string();

	return path(makeAbsolute(ustring).c_str());
}

UString FilePath::makeAbsolute(const UString &p) {
	UString absolute;

	if (isAbsolute(p))
		absolute = p;
	else
		absolute = boost::filesystem::initial_path().string() + "/" + p.c_str();

	return normalize(absolute);
}

UString FilePath::findSubDirectory(const UString &directory, const UString &subDirectory,
		bool caseInsensitive) {

	if (!exists(directory.c_str()) || !is_directory(directory.c_str()))
		// Path is either no directory or doesn't exist
		return "";

	if (subDirectory.empty())
		// Subdirectory to look for is empty, return the directory instead
		return directory;

	try {
		path dirPath(directory.c_str());
		path subDirPath(subDirectory.c_str());

		// Iterator over the directory's contents
		directory_iterator itEnd;
		for (directory_iterator itDir(dirPath); itDir != itEnd; ++itDir) {
			if (is_directory(itDir->status())) {
				// It's a directory. Check if it's the one we're looking for

				if (caseInsensitive) {
					if (iequals(itDir->path().filename(), subDirectory.c_str()))
						return itDir->path().string();
				} else {
					if (equals(itDir->path().filename(), subDirectory.c_str()))
						return itDir->path().string();
				}
			}
		}
	} catch (...) {
	}

	return "";
}

} // End of namespace Common
