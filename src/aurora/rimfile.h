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
#include <memory>

#include "src/common/types.h"

#include "src/aurora/types.h"
#include "src/aurora/archive.h"
#include "src/aurora/aurorafile.h"

namespace Common {
	class SeekableReadStream;
}

namespace Aurora {

/** Class to hold resource data of a RIM archive file.
 *
 *  A RIM file is a resource archive, used in several Aurora games.
 *  It is a simplified version of the more complex ERF format, lacking
 *  the support for a localized description text as found in early ERF
 *  formats, as well as compression and encryption as found in later
 *  ERF formats.
 *
 *  There is only one single version, V1.0, of the RIM format known and
 *  supported. For each resource file, it stores a path- and extension-less
 *  name (with a maximum of 16 ASCII characters) and a Type ID.
 *
 *  RIM files are found in Knights of the Old Republic, Knights of the Old
 *  Republic II and Jade Empire. They always have the .rim extension.
 *
 *  Note: .rim files in the Dragon Age games are not real RIM files.
 *        Instead, they are ERF files. See class ERFFile in erffile.h.
 */
class RIMFile : public Archive, public AuroraFile {
public:
	/** Take over this stream and read a RIM file out of it. */
	RIMFile(Common::SeekableReadStream *rim);
	~RIMFile();

	/** Return the list of resources. */
	const ResourceList &getResources() const;

	/** Return the size of a resource. */
	uint32_t getResourceSize(uint32_t index) const;

	/** Return a stream of the resource's contents. */
	Common::SeekableReadStream *getResource(uint32_t index, bool tryNoCopy = false) const;

private:
	/** Internal resource information. */
	struct IResource {
		uint32_t offset; ///< The offset of the resource within the RIM.
		uint32_t size;   ///< The resource's size.
	};

	typedef std::vector<IResource> IResourceList;

	std::unique_ptr<Common::SeekableReadStream> _rim;

	/** External list of resource names and types. */
	ResourceList _resources;

	/** Internal list of resource offsets and sizes. */
	IResourceList _iResources;

	void load(Common::SeekableReadStream &rim);
	void readResList(Common::SeekableReadStream &rim, uint32_t offset);

	const IResource &getIResource(uint32_t index) const;
};

} // End of namespace Aurora

#endif // AURORA_RIMFILE_H
