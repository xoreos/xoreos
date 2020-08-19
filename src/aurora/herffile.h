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
 *  BioWare's HERF (hashed ERF) file parsing.
 */

#ifndef AURORA_HERFFILE_H
#define AURORA_HERFFILE_H

#include <vector>
#include <map>
#include <memory>

#include "src/common/types.h"
#include "src/common/ustring.h"

#include "src/aurora/types.h"
#include "src/aurora/archive.h"

namespace Common {
	class SeekableReadStream;
}

namespace Aurora {

/** Class to hold resource data of an HERF archive file.
 *
 *  A HERF file is a very simplified version of a ERF file, similar to a
 *  RIM file. But unlike a RIM file, a HERF file only stores djb2 hashes
 *  of the included resource names. A dictionary, which matches hashes
 *  back to names might be present, but doesn't have to.
 *
 *  HERF files are only used in the Nintendo DS game Sonic Chronicles.
 */
class HERFFile : public Archive {
public:
	/** Take over this stream and read an HERF file out of it. */
	HERFFile(Common::SeekableReadStream *herf);
	~HERFFile();

	/** Return the list of resources. */
	const ResourceList &getResources() const;

	/** Return the size of a resource. */
	uint32_t getResourceSize(uint32_t index) const;

	/** Return a stream of the resource's contents. */
	Common::SeekableReadStream *getResource(uint32_t index, bool tryNoCopy = false) const;

	/** Return with which algorithm the name is hashed. */
	Common::HashAlgo getNameHashAlgo() const;

private:
	/** Internal resource information. */
	struct IResource {
		uint32_t offset;   ///< The offset of the resource within the HERF.
		uint32_t size;     ///< The resource's size.
	};

	typedef std::vector<IResource> IResourceList;

	std::unique_ptr<Common::SeekableReadStream> _herf;

	/** External list of resource names and types. */
	ResourceList _resources;

	/** Internal list of resource offsets and sizes. */
	IResourceList _iResources;

	uint32_t _dictOffset; ///< The offset of the dict file (if available).
	uint32_t _dictSize;   ///< The size of the dict file (if available).

	void load(Common::SeekableReadStream &herf);
	void searchDictionary(Common::SeekableReadStream &herf, uint32_t resCount);
	void readDictionary(Common::SeekableReadStream &herf, std::map<uint32_t, Common::UString> &dict);
	void readResList(Common::SeekableReadStream &herf);

	void readNames();

	const IResource &getIResource(uint32_t index) const;
};

} // End of namespace Aurora

#endif // AURORA_HERFFILE_H
