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
 *  Handling BioWare's BZFs (resource data files), used in the iOS version of
 *  Knights of the Old Republic.
 *
 *  Essentially, they are BIF files with LZMA-compressed data.
 */

#ifndef AURORA_BZFFILE_H
#define AURORA_BZFFILE_H

#include <vector>

#include "src/common/types.h"

#include "src/aurora/types.h"
#include "src/aurora/archive.h"
#include "src/aurora/aurorafile.h"

namespace Common {
	class SeekableReadStream;
	class File;
}

namespace Aurora {

class KEYFile;

/** Class to hold resource data information of a bzf file. */
class BZFFile : public Archive, public AuroraBase {
public:
	BZFFile(const Common::UString &fileName);
	~BZFFile();

	/** Clear the resource list. */
	void clear();

	/** Return the list of resources. */
	const ResourceList &getResources() const;

	/** Return the size of a resource. */
	uint32 getResourceSize(uint32 index) const;

	/** Return a stream of the resource's contents. */
	Common::SeekableReadStream *getResource(uint32 index) const;

	/** Merge information from the KEY into the BZF. */
	void mergeKEY(const KEYFile &key, uint32 bifIndex);

private:
	/** Internal resource information. */
	struct IResource {
		FileType type; ///< The resource's type.

		uint32 offset; ///< The offset of the resource within the BZF.
		uint32 size;   ///< The resource's size.

		uint32 packedSize; ///< Raw, uncompressed data size.
	};

	typedef std::vector<IResource> IResourceList;

	/** External list of resource names and types. */
	ResourceList _resources;

	/** Internal list of resource offsets and sizes. */
	IResourceList _iResources;

	/** The name of the BZF file. */
	Common::UString _fileName;

	void open(Common::File &file) const;

	void load();
	void readVarResTable(Common::SeekableReadStream &bzf, uint32 offset);

	const IResource &getIResource(uint32 index) const;

	Common::SeekableReadStream *decompress(byte *compressedData, uint32 packedSize, uint32 unpackedSize) const;
};

} // End of namespace Aurora

#endif // AURORA_BZFFILE_H
