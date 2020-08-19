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
 *  Handling Aspyr's OBB virtual filesystems.
 */

#ifndef AURORA_OBBFILE_H
#define AURORA_OBBFILE_H

#include <vector>
#include <memory>

#include "src/common/types.h"

#include "src/aurora/types.h"
#include "src/aurora/archive.h"
#include "src/aurora/aurorafile.h"

namespace Common {
	class SeekableReadStream;
}

namespace Aurora {

/** Class to hold resource data of an OBB virtual filesystem.
 *
 *  Aspyr's Jade Empire port for Android comes with all data files
 *  wrapped into a virtual filesystem within two OBB files. All
 *  data and even the index are zlib compressed.
 *
 *  This class treats OBB files like an archive, allowing easy
 *  access to the files within.
 *
 *  TODO: Check if the OBB virtual filesystem is also used in Aspyr's
 *  ports of the Knights of the Old Republic games. Also, is it maybe
 *  also used for the iOS ports?
 */
class OBBFile : public Archive {
public:
	/** Take over this stream and read an OBB file out of it. */
	OBBFile(Common::SeekableReadStream *obb);
	~OBBFile();

	/** Return the list of resources. */
	const ResourceList &getResources() const;

	/** Return the size of a resource. */
	uint32_t getResourceSize(uint32_t index) const;

	/** Return a stream of the resource's contents. */
	Common::SeekableReadStream *getResource(uint32_t index, bool tryNoCopy = false) const;

private:
	/** Internal resource information. */
	struct IResource {
		uint32_t offset;           ///< The offset of the resource within the OBB.
		uint32_t uncompressedSize; ///< The resource's uncompressed size.
		uint32_t compressedSize;   ///< The resource's compressed size.
	};

	typedef std::vector<IResource> IResourceList;

	std::unique_ptr<Common::SeekableReadStream> _obb;

	/** External list of resource names and types. */
	ResourceList _resources;

	/** Internal list of resource offsets and sizes. */
	IResourceList _iResources;

	void load(Common::SeekableReadStream &obb);
	void readResList(Common::SeekableReadStream &index);

	Common::SeekableReadStream *getIndex(Common::SeekableReadStream &obb);

	const IResource &getIResource(uint32_t index) const;
};

} // End of namespace Aurora

#endif // AURORA_OBBFILE_H
