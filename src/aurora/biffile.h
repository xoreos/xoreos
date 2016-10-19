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
 *  Handling BioWare's BIFs (resource data files).
 */

#ifndef AURORA_BIFFILE_H
#define AURORA_BIFFILE_H

#include <vector>

#include "src/common/types.h"
#include "src/common/scopedptr.h"

#include "src/aurora/types.h"
#include "src/aurora/archive.h"
#include "src/aurora/aurorafile.h"

namespace Common {
	class SeekableReadStream;
}

namespace Aurora {

class KEYFile;

/** Class to hold resource data information of a BIF file.
 *
 *  A BIF file is one part of the KEY/BIF resource archive system.
 *  The KEY file contains the resource names and types, and the BIF
 *  file contains the actual resource data. So BIF files only contain
 *  the resource data itself.
 *
 *  A KEY file can index resources of several BIF files and several
 *  BIF files can in turn index different resources of the same BIF
 *  file.
 *
 *  Additionally, there are BZF files. A BZF is a compressed variation
 *  of a BIF file, found exclusively in the iOS version of Knights of
 *  the Old Republic.
 *
 *  See also classes KEYFile in keyfile.h and BZFFile in bzffile.h.
 *
 *  There are two versions of BIF files known and supported
 *  - V1, used by Neverwinter Nights, Neverwinter Nights 2, Knight of
 *    the Old Republic, Knight of the Old Republic II and Jade Empire
 *  - V1.1, used by The Witcher
 *
 *  Please note that BIF (and KEY) files found in Infinity Engine
 *  games (Baldur's Gate et al) are not supported at all, even though
 *  they claim to be V1.
 */
class BIFFile : public Archive, public AuroraFile {
public:
	/** Take over this stream and read a BIF file out of it. */
	BIFFile(Common::SeekableReadStream *bif);
	~BIFFile();

	/** Return the list of resources. */
	const ResourceList &getResources() const;

	/** Return the size of a resource. */
	uint32 getResourceSize(uint32 index) const;

	/** Return a stream of the resource's contents. */
	Common::SeekableReadStream *getResource(uint32 index, bool tryNoCopy = false) const;

	/** Merge information from the KEY into the BIF.
	 *
	 *  Without this step, this BIFFile archive does not contain any
	 *  resource names at all.
	 *
	 *  @param key      A KEYFile with information about this BIF.
	 *  @param bifIndex The index this BIF has within the KEY file.
	 */
	void mergeKEY(const KEYFile &key, uint32 bifIndex);

private:
	/** Internal resource information. */
	struct IResource {
		FileType type; ///< The resource's type.

		uint32 offset; ///< The offset of the resource within the BIF.
		uint32 size;   ///< The resource's size.
	};

	typedef std::vector<IResource> IResourceList;

	Common::ScopedPtr<Common::SeekableReadStream> _bif;

	/** External list of resource names and types. */
	ResourceList _resources;

	/** Internal list of resource offsets and sizes. */
	IResourceList _iResources;

	void load(Common::SeekableReadStream &bif);
	void readVarResTable(Common::SeekableReadStream &bif, uint32 offset);

	const IResource &getIResource(uint32 index) const;
};

} // End of namespace Aurora

#endif // AURORA_BIFFILE_H
