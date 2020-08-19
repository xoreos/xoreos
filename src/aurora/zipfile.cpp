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
 *  A ZIP archive.
 */

#include <cassert>

#include "src/common/zipfile.h"
#include "src/common/filepath.h"

#include "src/aurora/zipfile.h"
#include "src/aurora/util.h"

namespace Aurora {

ZIPFile::ZIPFile(Common::SeekableReadStream *zip) {
	assert(zip);

	_zipFile = std::make_unique<Common::ZipFile>(zip);

	load();
}

ZIPFile::~ZIPFile() {
}

const Archive::ResourceList &ZIPFile::getResources() const {
	return _resources;
}

uint32_t ZIPFile::getResourceSize(uint32_t index) const {
	return _zipFile->getFileSize(index);
}

Common::SeekableReadStream *ZIPFile::getResource(uint32_t index, bool tryNoCopy) const {
	return _zipFile->getFile(index, tryNoCopy);
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
}

} // End of namespace Aurora
