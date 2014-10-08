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

/** @file common/filelist.cpp
 *  A list of files.
 */

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

#include "common/filelist.h"
#include "common/filepath.h"
#include "common/file.h"
#include "common/stream.h"

// boost-filesystem stuff
using boost::filesystem::directory_iterator;

namespace Common {

FileList::FileList() {
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

bool FileList::isEmpty() const {
	return _files.empty();
}

uint32 FileList::size() const {
	return _files.size();
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

bool FileList::getSubList(const UString &glob, FileList &subList, bool caseInsensitive) const {
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

bool FileList::getSubList(const UString &glob, std::list<UString> &list, bool caseInsensitive) const {
	boost::regex::flag_type type = boost::regex::perl;
	if (caseInsensitive)
		type |= boost::regex::icase;
	boost::regex expression(glob.c_str(), type);

	bool foundMatch = false;

	// Iterate through the whole list, adding the matches to the sub list
	for (Files::const_iterator it = _files.begin(); it != _files.end(); ++it)
		if (boost::regex_match(it->c_str(), expression)) {
			list.push_back(*it);
			foundMatch = true;
		}

	return foundMatch;
}

bool FileList::contains(const UString &fileName) const {
	return !getPath(fileName).empty();
}

bool FileList::contains(const UString &glob, bool caseInsensitive) const {
	return !getPath(glob, caseInsensitive).empty();
}

UString FileList::findFirst(const UString &glob, bool caseInsensitive) const {
	return getPath(glob, caseInsensitive);
}

SeekableReadStream *FileList::openFile(const UString &fileName) const {
	UString p = getPath(fileName);
	if (p.empty())
		return 0;

	File *file = new File;
	if (!file->open(p)) {
		delete file;
		return 0;
	}

	return file;
}

SeekableReadStream *FileList::openFile(const UString &glob, bool caseInsensitive) const {
	UString p = getPath(glob, caseInsensitive);
	if (p.empty())
		return 0;

	File *file = new File;
	if (!file->open(p)) {
		delete file;
		return 0;
	}

	return file;
}

UString FileList::getPath(const UString &fileName) const {
	UString canonicalFile = FilePath::canonicalize(fileName, false);

	// Iterate through the whole list, looking for a match
	for (Files::const_iterator it = _files.begin(); it != _files.end(); ++it)
		if (*it == canonicalFile)
			return *it;

	return "";
}

UString FileList::getPath(const UString &glob, bool caseInsensitive) const {
	boost::regex::flag_type type = boost::regex::perl;
	if (caseInsensitive)
		type |= boost::regex::icase;
	boost::regex expression(glob.c_str(), type);

	// Iterate through the whole list, looking for a match
	for (Files::const_iterator it = _files.begin(); it != _files.end(); ++it)
		if (boost::regex_match(it->c_str(), expression))
			return *it;

	return "";
}

} // End of namespace Common
