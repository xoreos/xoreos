/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file aurora/zipfile.cpp
 *  A ZIP archive.
 */

#include "common/ustring.h"
#include "common/file.h"
#include "common/zipfile.h"
#include "common/filepath.h"

#include "aurora/zipfile.h"
#include "aurora/util.h"

namespace Aurora {

ZIPFile::ZIPFile(const Common::UString &fileName) : _zipFile(0) {
	_zipFile = new Common::ZipFile(fileName);

	load();
}

ZIPFile::~ZIPFile() {
	delete _zipFile;
}

void ZIPFile::clear() {
	_resources.clear();
}

const Archive::ResourceList &ZIPFile::getResources() const {
	return _resources;
}

Common::SeekableReadStream *ZIPFile::getResource(uint32 index) const {
	return _zipFile->getFile(index);
}

void ZIPFile::load() {
	const Common::ZipFile::FileList &files = _zipFile->getFiles();
	for (Common::ZipFile::FileList::const_iterator file = files.begin(); file != files.end(); ++file) {
		Resource res;

		res.name  = Common::FilePath::getStem(file->name);
		res.type  = getFileType(file->name);
		res.index = file->index;

		_resources.push_back(res);
	}

	_zipFile->clear();
}

} // End of namespace Aurora
