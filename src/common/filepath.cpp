/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file common/filepath.cpp
 *  Utility class for manipulating file paths.
 */

#include <list>

#include <boost/algorithm/string.hpp>
#include <boost/system/config.hpp>
#include <boost/regex.hpp>
#include <boost/version.hpp>

#include "common/filepath.h"
#include "common/util.h"

// boost-filesystem stuff
using boost::filesystem::path;
using boost::filesystem::exists;
using boost::filesystem::is_regular_file;
using boost::filesystem::is_directory;
using boost::filesystem::file_size;
using boost::filesystem::directory_iterator;

// boost-string_algo
using boost::equals;
using boost::iequals;

#if ((((BOOST_VERSION / 100000) == 1) && (((BOOST_VERSION / 100) % 1000) < 44)) || BOOST_FILESYSTEM_VERSION == 2)
#define generic_string() string()
#elif BOOST_FILESYSTEM_VERSION == 3
#define stem() stem().string()
#define extension() extension().string()
#define filename() filename().string()
#endif

namespace Common {

bool FilePath::isRegularFile(const UString &p) {
	return (exists(p.c_str()) && is_regular_file(p.c_str()));
}

bool FilePath::isDirectory(const UString &p) {
	return (exists(p.c_str()) && is_directory(p.c_str()));
}

uint32 FilePath::getFileSize(const UString &p) {
	uintmax_t size = file_size(p.c_str());

	if (size == ((uintmax_t) -1))
		return kFileInvalid;

	return size;
}

UString FilePath::getFile(const UString &p) {
	path file(p.c_str());

	return file.filename();
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
	if (p.size() >= 2) {
		UString::iterator it = p.begin();
		if (isalpha(*it))
			if (*++it == ':') {
				if (p.size() == 2)
					return true;

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
		absolute = boost::filesystem::initial_path().generic_string() + "/" + p.c_str();

	return normalize(absolute);
}

static void splitDirectories(const UString &directory, std::list<UString> &dirs) {
	UString curDir;

	for (UString::iterator it = directory.begin(); it != directory.end(); ++it) {
		uint32 c = *it;

		if (c == '/') {
			// Found a directory separator, split here

			// Got a real directory, add it to our list
			if (!curDir.empty())
				dirs.push_back(curDir);

			curDir.clear();

		} else
			// Otherwise, just append the current character
			curDir += c;

	}

	// Got trailing data, add it to our list
	if (!curDir.empty())
		dirs.push_back(curDir);
}

static UString findSubDirectory_internal(const UString &directory, const UString &subDirectory,
		bool caseInsensitive) {

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
						return itDir->path().generic_string();
				} else {
					if (equals(itDir->path().filename(), subDirectory.c_str()))
						return itDir->path().generic_string();
				}
			}
		}
	} catch (...) {
	}

	return "";
}

UString FilePath::findSubDirectory(const UString &directory, const UString &subDirectory,
		bool caseInsensitive) {

	if (!exists(directory.c_str()) || !is_directory(directory.c_str()))
		// Path is either no directory or doesn't exist
		return "";

	if (subDirectory.empty())
		// Subdirectory to look for is empty, return the directory instead
		return directory;

	// Split the subDirectory string into actual directories
	std::list<UString> dirs;
	splitDirectories(subDirectory, dirs);

	// Iterate over the directory list to find each successive subdirectory
	UString curDir = directory;
	for (std::list<UString>::iterator it = dirs.begin(); it != dirs.end(); ++it)
		if ((curDir = findSubDirectory_internal(curDir, *it, caseInsensitive)).empty())
			return "";

	return curDir;
}

UString FilePath::escapeStringLiteral(const UString &str) {
	const boost::regex esc("[\\^\\.\\$\\|\\(\\)\\[\\]\\*\\+\\?\\/\\\\]");
	const std::string  rep("\\\\\\1&");

	return boost::regex_replace(std::string(str.c_str()), esc, rep, boost::match_default | boost::format_sed);
}

} // End of namespace Common
