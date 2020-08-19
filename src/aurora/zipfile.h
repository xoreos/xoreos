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

#ifndef AURORA_ZIPFILE_H
#define AURORA_ZIPFILE_H

#include <memory>

#include "src/common/types.h"

#include "src/aurora/types.h"
#include "src/aurora/archive.h"

namespace Common {
	class SeekableReadStream;
	class ZipFile;
}

namespace Aurora {

/** A class encapsulating ZIP files for resource archive access. */
class ZIPFile : public Archive {
public:
	/** Take over this stream and read a ZIP file out of it. */
	ZIPFile(Common::SeekableReadStream *zip);
	~ZIPFile();

	/** Return the list of resources. */
	const ResourceList &getResources() const;

	/** Return the size of a resource. */
	uint32_t getResourceSize(uint32_t index) const;

	/** Return a stream of the resource's contents. */
	Common::SeekableReadStream *getResource(uint32_t index, bool tryNoCopy = false) const;

private:
	/** The actual zip file. */
	std::unique_ptr<Common::ZipFile> _zipFile;

	/** External list of resource names and types. */
	ResourceList _resources;

	void load();
};

} // End of namespace Aurora

#endif // AURORA_ZIPFILE_H
