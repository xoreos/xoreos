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
 *  Utility class for manipulating file paths.
 */

#include <list>
#include <regex>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include "src/common/filepath.h"
#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/encoding.h"
#include "src/common/platform.h"

// boost-filesystem stuff
using boost::filesystem::path;
using boost::filesystem::exists;
using boost::filesystem::is_regular_file;
using boost::filesystem::is_directory;
using boost::filesystem::file_size;
using boost::filesystem::directory_iterator;
using boost::filesystem::create_directories;

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

size_t FilePath::getFileSize(const UString &p) {
	uintmax_t size = (uintmax_t) -1;

	try {
		size = file_size(p.c_str());
	} catch (...) {
	}

	if (size == ((uintmax_t) -1)) {
		warning("Failed to get size of file \"%s\"", p.c_str());
		return kFileInvalid;
	}

	if (size > 0x7FFFFFFF) {
		warning("Size of file \"%s\" too large", p.c_str());
		return kFileInvalid;
	}

	return size;
}

UString FilePath::getFile(const UString &p) {
	path file(p.c_str());

	return file.filename().string();
}

UString FilePath::getStem(const UString &p) {
	path file(p.c_str());

	return file.stem().string();
}

UString FilePath::getExtension(const UString &p) {
	path file(p.c_str());

	return file.extension().string();
}

UString FilePath::changeExtension(const UString &p, const UString &ext) {
	path file(p.c_str());

	file.replace_extension(ext.c_str());

	return file.string();
}

UString FilePath::getDirectory(const UString &p) {
	path file(p.c_str());

	return file.parent_path().string();
}

bool FilePath::isAbsolute(const UString &p) {
	return boost::filesystem::path(p.c_str()).is_absolute();
}

bool FilePath::isPOSIXAbsolute(const UString &p) {
	return !p.empty() && *p.begin() == '/';
}

static path convertToSlash(const path &p) {
	const std::regex bSlash("\\\\");
	const std::string fSlash("/");
	const std::regex_constants::match_flag_type flags(
			std::regex_constants::match_default | std::regex_constants::format_sed);

	return path(std::regex_replace(p.string(), bSlash, fSlash, flags));
}

UString FilePath::absolutize(const UString &p) {
	path source = convertToSlash(p.c_str()).native();

	// Resolve ~ as the user's home directory
	path::iterator itr = source.begin();
	if ((itr != source.end()) && (itr->string() == "~")) {
		path newSource = getHomeDirectory().c_str();

		for (++itr; itr != source.end(); ++itr)
			newSource /= *itr;

		source = newSource;
	}

	return convertToSlash(boost::filesystem::absolute(source)).string().c_str();
}

UString FilePath::relativize(const UString &basePath, const UString &path) {
	const Common::UString normPath = normalize(path, false);
	const Common::UString normBase = normalize(basePath, false);

	UString relative = "";

	if (normPath.beginsWith(normBase))
		relative = normPath.substr(normPath.getPosition(normBase.size() + 1), normPath.end());

	return relative;
}

UString FilePath::normalize(const UString &p, bool resolveSymLinks) {
	boost::filesystem::path source = convertToSlash(p.c_str()).native();
	boost::filesystem::path result;

	// Resolve ~ as the user's home directory
	boost::filesystem::path::iterator itr = source.begin();
	if ((itr != source.end()) && (itr->string() == "~")) {
		boost::filesystem::path newSource = getHomeDirectory().c_str();

		for (++itr; itr != source.end(); ++itr)
			newSource /= *itr;

		source = newSource;
	}

	bool scan = true;
	while (scan) {
		scan = false;
		result.clear();

		for (itr = source.begin(); itr != source.end(); ++itr) {
			// Resolve .
			if (itr->string() == ".")
				continue;

			// Resolve .., but only if symbolic links are also resolved
			if (resolveSymLinks && (itr->string() == "..")) {
				if (result != source.root_path())
					result.remove_filename();
				continue;
			}

			result /= *itr;

			/* If the resolving of symbolic links was requested, check if the current path
			 * is one. If it is, resolve it and restart normalization.
			 */

			if (!resolveSymLinks)
				continue;

			boost::system::error_code ec;
			boost::filesystem::path link = boost::filesystem::read_symlink(result, ec);

			if (!link.empty()) {
				result.remove_filename();

				if (link.is_absolute()) {
					for (++itr; itr != source.end(); ++itr)
						link /= *itr;

					source = link;

				} else {
					boost::filesystem::path newSource = result;

					newSource /= link;

					for (++itr; itr != source.end(); ++itr)
						newSource /= *itr;

					source = newSource;
				}

				scan = true;
				break;
			}
		}
	}

	if (!result.has_root_directory())
		result = boost::filesystem::absolute(result);

	return convertToSlash(result).string().c_str();
}

UString FilePath::canonicalize(const UString &p, bool resolveSymLinks) {
	return normalize(absolutize(p), resolveSymLinks);
}

bool FilePath::getSubDirectories(const UString &directory, std::list<UString> &subDirectories) {
	path dirPath(directory.c_str());

	try {
		// Iterate over the directory's contents
		directory_iterator itEnd;
		for (directory_iterator itDir(dirPath); itDir != itEnd; ++itDir) {
			if (is_directory(itDir->status())) {
				subDirectories.push_back(itDir->path().generic_string());
			}
		}
	} catch (...) {
		return false;
	}

	return true;
}

static void splitDirectories(const UString &directory, std::list<UString> &dirs) {
	UString curDir;

	for (UString::iterator it = directory.begin(); it != directory.end(); ++it) {
		uint32_t c = *it;

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
		// Special case: . is the same, current directory
		if (subDirectory == UString("."))
			return directory;

		const path dirPath(directory.c_str());

		// Special case: .. is the parent directory
		if (subDirectory == UString("..")) {
			path parent = dirPath.parent_path();
			if (parent == dirPath)
				return "";

			return parent.generic_string();
		}

		// Iterator over the directory's contents
		directory_iterator itEnd;
		for (directory_iterator itDir(dirPath); itDir != itEnd; ++itDir) {
			if (is_directory(itDir->status())) {
				// It's a directory. Check if it's the one we're looking for

				if (caseInsensitive) {
					if (iequals(itDir->path().filename().string(), subDirectory.c_str()))
						return itDir->path().generic_string();
				} else {
					if (equals(itDir->path().filename().string(), subDirectory.c_str()))
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

bool FilePath::createDirectories(const UString &path) {
	try {
		return create_directories(path.c_str());
	} catch (std::exception &se) {
		throw Exception(se);
	}
}

UString FilePath::escapeStringLiteral(const UString &str) {
	const std::regex esc("[\\^\\.\\$\\|\\(\\)\\[\\]\\*\\+\\?\\/\\\\]");
	const std::string rep("\\$&");

	return std::regex_replace(std::string(str.c_str()), esc, rep);
}

UString FilePath::getHumanReadableSize(size_t size) {
	static const char * const sizes[] = {"B", "K", "M", "G"};

	double s = size;
	size_t n = 0;

	while ((s >= 1024) && ((n + 1) < ARRAYSIZE(sizes))) {
		n++;
		s /= 1024;
	}

	return UString::format("%.2lf%s", s, sizes[n]);
}

UString FilePath::getHomeDirectory() {
	return Platform::getHomeDirectory();
}

UString FilePath::getConfigDirectory() {
	return Platform::getConfigDirectory();
}

UString FilePath::getUserDataDirectory() {
	return Platform::getUserDataDirectory();
}

UString FilePath::getUserDataFile(UString file) {
	if (!isAbsolute(file))
		file = getUserDataDirectory() + "/" + file;

	return canonicalize(file);
}

} // End of namespace Common
