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
 *  Handling BioWare's RIMs (resource archives).
 */

#ifndef AURORA_RIMFILE_H
#define AURORA_RIMFILE_H

#include <vector>

#include "common/types.h"
#include "common/ustring.h"
#include "common/file.h"

#include "aurora/types.h"
#include "aurora/archive.h"
#include "aurora/aurorafile.h"

namespace Common {
	class SeekableReadStream;
	class File;
}

namespace Aurora {

/** Class to hold resource data of a RIM file. */
class RIMFile : public Archive, public AuroraBase {
public:
	RIMFile(const Common::UString &fileName);
	~RIMFile();

	/** Clear the resource list. */
	void clear();

	/** Return the list of resources. */
	const ResourceList &getResources() const;

	/** Return the size of a resource. */
	uint32 getResourceSize(uint32 index) const;

	/** Return a stream of the resource's contents. */
	Common::SeekableReadStream *getResource(uint32 index) const;

private:
	/** Internal resource information. */
	struct IResource {
		uint32 offset; ///< The offset of the resource within the RIM.
		uint32 size;   ///< The resource's size.
	};

	typedef std::vector<IResource> IResourceList;

	/** External list of resource names and types. */
	ResourceList _resources;

	/** Internal list of resource offsets and sizes. */
	IResourceList _iResources;

	/** The name of the RIM file. */
	Common::UString _fileName;

	void open(Common::File &file) const;

	void load();
	void readResList(Common::SeekableReadStream &rim, uint32 offset);

	const IResource &getIResource(uint32 index) const;
};

} // End of namespace Aurora

#endif // AURORA_RIMFILE_H
