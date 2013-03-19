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

/** @file common/filelist.cpp
 *  A list of files.
 */

#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <boost/version.hpp>

#include "common/filelist.h"
#include "common/file.h"
#include "common/stream.h"

// boost-filesystem stuff
using boost::filesystem::path;
using boost::filesystem::exists;
using boost::filesystem::is_directory;
using boost::filesystem::directory_iterator;

// boost-string_algo
using boost::to_lower_copy;
using boost::equals;
using boost::iequals;

#if ((((BOOST_VERSION / 100000) == 1) && (((BOOST_VERSION / 100) % 1000) < 44)) || BOOST_FILESYSTEM_VERSION == 2)
#define generic_string() string()
#elif BOOST_FILESYSTEM_VERSION == 3
#define stem() stem().string()
#endif

namespace Common {

FileList::FilePath::FilePath(UString b, boost::filesystem::path p) :
	baseDir(b), pathString(p.string()), filePath(p) {
}

FileList::FilePath::FilePath(const FilePath &p) :
	baseDir(p.baseDir), pathString(p.pathString), filePath(p.filePath) {
}


FileList::const_iterator::const_iterator(const const_iterator &i) : it(i.it) {
}

FileList::const_iterator::const_iterator(const std::list<FilePath>::const_iterator &i) : it(i) {
}

FileList::const_iterator &FileList::const_iterator::operator++() {
	++it;

	return *this;
}

FileList::const_iterator FileList::const_iterator::operator++(int) {
	const_iterator tmp(*this);
	++(*this);
	return tmp;
}

FileList::const_iterator &FileList::const_iterator::operator--() {
	--it;

	return *this;
}

FileList::const_iterator FileList::const_iterator::operator--(int) {
	const_iterator tmp(*this);
	--(*this);
	return tmp;
}

const UString &FileList::const_iterator::operator*() const {
	return it->pathString;
}

const UString *FileList::const_iterator::operator->() const {
	return &it->pathString;
}

bool FileList::const_iterator::operator==(const const_iterator &x) const {
	return it == x.it;
}

bool FileList::const_iterator::operator!=(const const_iterator &x) const {
	return it != x.it;
}


FileList::FileList() {
}

FileList::FileList(const FileList &list) {
	*this = list;
}

FileList::~FileList() {
}

FileList &FileList::operator=(const FileList &list) {
	_files = list._files;
	_fileMap = list._fileMap;

	return *this;
}

FileList &FileList::operator+=(const FileList &list) {
	_files.insert(_files.end(), list._files.begin(), list._files.end());
	_fileMap.insert(list._fileMap.begin(), list._fileMap.end());

	return *this;
}

void FileList::clear() {
	_files.clear();
	_fileMap.clear();
}

bool FileList::isEmpty() const {
	return _files.empty();
}

uint32 FileList::size() const {
	return _files.size();
}

uint FileList::getFileNames(std::list<UString> &list) const {
	uint n = 0;
	for (std::list<FilePath>::const_iterator it = _files.begin(); it != _files.end(); ++it) {
		list.push_back(it->pathString);
		n++;
	}

	return n;
}

bool FileList::addDirectory(const UString &directory, int recurseDepth) {
	return addDirectory(directory, path(directory.c_str()), recurseDepth);
}

bool FileList::addDirectory(const UString &base,
		const boost::filesystem::path &directory, int recurseDepth) {

	if (!exists(directory) || !is_directory(directory))
		// Path is either no directory or doesn't exist
		return false;

	try {
		// Iterator over the directory's contents
		directory_iterator itEnd;
		for (directory_iterator itDir(directory); itDir != itEnd; ++itDir) {
			if (is_directory(itDir->status())) {
				// It's a directory. Recurse into it if the depth limit wasn't yet reached

				if (recurseDepth != 0)
					if (!addDirectory(base, itDir->path(), (recurseDepth == -1) ? -1 : (recurseDepth - 1)))
						return false;

			} else
				// It's a path, add it to the list
				addPath(base, itDir->path());
		}
	} catch (...) {
		return false;
	}

	return true;
}

void FileList::addPath(const FilePath &p) {
	_files.push_back(p);
	_fileMap.insert(std::make_pair(to_lower_copy(p.filePath.stem()), --_files.end()));
}

void FileList::addPath(const UString &base, const boost::filesystem::path &p) {
	addPath(FilePath(base, p));
}

bool FileList::getSubList(const UString &glob, FileList &subList, bool caseInsensitive) const {
	boost::regex::flag_type type = boost::regex::perl;
	if (caseInsensitive)
		type |= boost::regex::icase;
	boost::regex expression(glob.c_str(), type);

	bool foundMatch = false;

	// Iterate through the whole list, adding the matches to the sub list
	for (std::list<FilePath>::const_iterator it = _files.begin(); it != _files.end(); ++it)
		if (boost::regex_match(it->filePath.generic_string(), expression)) {
			subList.addPath(*it);
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
	for (std::list<FilePath>::const_iterator it = _files.begin(); it != _files.end(); ++it)
		if (boost::regex_match(it->filePath.generic_string(), expression)) {
			list.push_back(it->filePath.generic_string());
			foundMatch = true;
		}

	return foundMatch;
}

bool FileList::contains(const UString &fileName) const {
	if (getPath(fileName))
		return true;

	return false;
}

bool FileList::contains(const UString &glob, bool caseInsensitive) const {
	if (getPath(glob, caseInsensitive))
		return true;

	return false;
}

UString FileList::findFirst(const UString &glob, bool caseInsensitive) const {
	const FilePath *p = getPath(glob, caseInsensitive);
	if (!p)
		return "";

	return p->filePath.generic_string();
}

SeekableReadStream *FileList::openFile(const UString &fileName) const {
	const FilePath *p = getPath(fileName);
	if (!p)
		return 0;

	File *file = new File;
	if (!file->open(p->filePath.generic_string())) {
		delete file;
		return 0;
	}

	return file;
}

SeekableReadStream *FileList::openFile(const UString &glob, bool caseInsensitive) const {
	const FilePath *p = getPath(glob, caseInsensitive);
	if (!p)
		return 0;

	File *file = new File;
	if (!file->open(p->filePath.generic_string())) {
		delete file;
		return 0;
	}

	return file;
}

const FileList::FilePath *FileList::getPath(const UString &fileName) const {
	// Iterate through the whole list, looking for a match
	for (std::list<FilePath>::const_iterator it = _files.begin(); it != _files.end(); ++it)
		if (it->pathString == fileName)
			return &*it;

	return 0;
}

const FileList::FilePath *FileList::getPath(const UString &glob, bool caseInsensitive) const {
	boost::regex::flag_type type = boost::regex::perl;
	if (caseInsensitive)
		type |= boost::regex::icase;
	boost::regex expression(glob.c_str(), type);

	// Iterate through the whole list, looking for a match
	for (std::list<FilePath>::const_iterator it = _files.begin(); it != _files.end(); ++it)
		if (boost::regex_match(it->filePath.generic_string(), expression))
			return &*it;

	return 0;
}

FileList::const_iterator FileList::begin() const {
	return const_iterator(_files.begin());
}

FileList::const_iterator FileList::end() const {
	return const_iterator(_files.end());
}

} // End of namespace Common
