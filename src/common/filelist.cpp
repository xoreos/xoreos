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
 *  A list of files.
 */

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

#include "src/common/filelist.h"
#include "src/common/filepath.h"

// boost-filesystem stuff
using boost::filesystem::directory_iterator;

namespace Common {

FileList::FileList() {
}

FileList::FileList(const UString &directory, int recurseDepth) {
	addDirectory(directory, recurseDepth);
}

FileList::FileList(const FileList &list) {
	*this = list;
}

FileList::~FileList() {
}

FileList &FileList::operator=(const FileList &list) {
	_files = list._files;

	return *this;
}

FileList &FileList::operator+=(const FileList &list) {
	_files.insert(_files.end(), list._files.begin(), list._files.end());

	return *this;
}

void FileList::clear() {
	_files.clear();
}

bool FileList::empty() const {
	return _files.empty();
}

size_t FileList::size() const {
	return _files.size();
}

void FileList::sort(bool caseInsensitive) {
	if (caseInsensitive)
		_files.sort(Common::UString::iless());
	else
		_files.sort(Common::UString::sless());
}

void FileList::relativize(const Common::UString &basePath) {
	std::list<UString>::iterator file = _files.begin();

	while (file != _files.end()) {
		*file = FilePath::relativize(basePath, *file);

		if (file->empty())
			file = _files.erase(file);
		else
			++file;
	}
}

FileList::const_iterator FileList::begin() const {
	return _files.begin();
}

FileList::const_iterator FileList::end() const {
	return _files.end();
}

bool FileList::addDirectory(const UString &directory, int recurseDepth) {
	// Not a directory? Fail.
	if (!FilePath::isDirectory(directory))
		return false;

	try {
		// Iterator over the directory's contents
		for (directory_iterator itEnd, itDir(directory.c_str()); itDir != itEnd; ++itDir) {
			const UString path = itDir->path().generic_string();

			if (FilePath::isDirectory(path)) {
				// It's a directory. Recurse into it if the depth limit wasn't yet reached

				if (recurseDepth != 0)
					if (!addDirectory(path, (recurseDepth == -1) ? -1 : (recurseDepth - 1)))
						return false;

			} else
				// It's a path, add it to the list
				_files.push_back(FilePath::canonicalize(path, false));

		}
	} catch (...) {
		return false;
	}

	return true;
}

bool FileList::getSubList(const UString &str, bool caseInsensitive, FileList &subList) const {
	UString match = caseInsensitive ? str.toLower() : str;

	bool foundMatch = false;

	// Iterate through the whole list, adding the matches to the sub list
	for (Files::const_iterator it = _files.begin(); it != _files.end(); ++it) {
		bool matching = caseInsensitive ? it->toLower().endsWith(match) : it->endsWith(match);

		if (matching) {
			subList._files.push_back(*it);
			foundMatch = true;
		}
	}

	return foundMatch;
}

bool FileList::getSubListGlob(const UString &glob, bool caseInsensitive, FileList &subList) const {
	boost::regex::flag_type type = boost::regex::perl;
	if (caseInsensitive)
		type |= boost::regex::icase;
	boost::regex expression(glob.c_str(), type);

	bool foundMatch = false;

	// Iterate through the whole list, adding the matches to the sub list
	for (Files::const_iterator it = _files.begin(); it != _files.end(); ++it)
		if (boost::regex_match(it->c_str(), expression)) {
			subList._files.push_back(*it);
			foundMatch = true;
		}

	return foundMatch;
}

bool FileList::contains(const UString &str, bool caseInsensitive) const {
	return !findFirst(str, caseInsensitive).empty();
}

bool FileList::containsGlob(const UString &glob, bool caseInsensitive) const {
	return !findFirstGlob(glob, caseInsensitive).empty();
}

UString FileList::findFirst(const UString &str, bool caseInsensitive) const {
	UString match = caseInsensitive ? str.toLower() : str;

	// Iterate through the whole list, adding the matches to the sub list
	for (Files::const_iterator it = _files.begin(); it != _files.end(); ++it) {
		bool matching = caseInsensitive ? it->toLower().endsWith(match) : it->endsWith(match);

		if (matching)
			return *it;
	}

	return "";
}

UString FileList::findFirstGlob(const UString &glob, bool caseInsensitive) const {
	boost::regex::flag_type type = boost::regex::perl;
	if (caseInsensitive)
		type |= boost::regex::icase;
	boost::regex expression(glob.c_str(), type);

	// Iterate through the whole list, adding the matches to the sub list
	for (Files::const_iterator it = _files.begin(); it != _files.end(); ++it)
		if (boost::regex_match(it->c_str(), expression))
			return *it;

	return "";
}

} // End of namespace Common
