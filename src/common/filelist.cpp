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

namespace Common {

FileList::FileList() {
}

FileList::~FileList() {
}

bool FileList::isEmpty() const {
	return _files.empty();
}

uint32 FileList::getSize() const {
	return _files.size();
}

void FileList::clear() {
	_files.clear();
}

bool FileList::addDirectory(const std::string &directory, int recurseDepth) {
	return false;
}

bool FileList::getSubMap(const std::string &glob, FileList &subMap) const {
	return false;
}

bool FileList::getSubMap(const std::string &glob, std::list<std::string> &list) const {
	return false;
}

bool FileList::constains(const std::string &fileName) const {
	return false;
}

SeekableReadStream *FileList::openFile(const std::string &fileName) const {
	return 0;
}

} // End of namespace Common
