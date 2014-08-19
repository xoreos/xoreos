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

uint32 ZIPFile::getResourceSize(uint32 index) const {
	return _zipFile->getFileSize(index);
}

Common::SeekableReadStream *ZIPFile::getResource(uint32 index) const {
	return _zipFile->getFile(index);
}

void ZIPFile::load() {
	const Common::ZipFile::FileList &files = _zipFile->getFiles();
	for (Common::ZipFile::FileList::const_iterator file = files.begin(); file != files.end(); ++file) {
		Resource res;

		res.name  = Common::FilePath::getStem(file->name);
		res.type  = TypeMan.getFileType(file->name);
		res.index = file->index;

		_resources.push_back(res);
	}

	_zipFile->clear();
}

} // End of namespace Aurora
