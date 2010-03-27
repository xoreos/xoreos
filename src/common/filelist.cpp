/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and Bioware corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

#include "filelist.h"
#include "stream.h"

// boost-filesystem stuff
using boost::filesystem::path;
using boost::filesystem::exists;
using boost::filesystem::is_directory;
using boost::filesystem::directory_iterator;

namespace Common {

FileList::FilePath::FilePath(std::string b, boost::filesystem::path p) : baseDir(b), filePath(p) {
}

FileList::FilePath::FilePath(const FilePath &p) : baseDir(p.baseDir), filePath(p.filePath) {
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

uint32 FileList::getSize() const {
	return _files.size();
}

void FileList::getFileNames(std::list<std::string> &list) const {
	for (std::list<FilePath>::const_iterator it = _files.begin(); it != _files.end(); ++it)
		list.push_back(it->filePath.string());
}

bool FileList::addDirectory(const std::string &directory, int recurseDepth) {
	return addDirectory(directory, path(directory), recurseDepth);
}

bool FileList::addDirectory(const std::string &base, const path &directory, int recurseDepth) {
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
				_files.push_back(FilePath(base, itDir->path()));
		}
	} catch (...) {
		return false;
	}

	return true;
}

bool FileList::getSubMap(const std::string &glob, FileList &subMap) const {
	return false;
}

bool FileList::getSubMap(const std::string &glob, std::list<std::string> &list) const {
	return false;
}

bool FileList::constains(const std::string &fileName, bool caseInsensitive) const {
	return false;
}

SeekableReadStream *FileList::openFile(const std::string &fileName, bool caseInsensitive) const {
	return 0;
}

} // End of namespace Common
