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
 *  Handling various archive files.
 */

#ifndef AURORA_ARCHIVE_H
#define AURORA_ARCHIVE_H

#include <list>

#include <boost/noncopyable.hpp>

#include "src/common/types.h"
#include "src/common/ustring.h"
#include "src/common/hash.h"

#include "src/aurora/types.h"

namespace Common {
	class SeekableReadStream;
}

namespace Aurora {

/** An abstract file archive. */
class Archive : boost::noncopyable {
public:
	/** A resource within the archive. */
	struct Resource {
		Common::UString name;  ///< The resource's name.
		uint64          hash;  ///< The resource's hashed name.
		FileType        type;  ///< The resource's type.
		uint32          index; ///< The resource's local index within the archive.

		Resource();
	};

	typedef std::list<Resource> ResourceList;

	Archive();
	virtual ~Archive();

	/** Return the list of resources. */
	virtual const ResourceList &getResources() const = 0;

	/** Return the size of a resource. */
	virtual uint32 getResourceSize(uint32 index) const;

	/** Return a stream of the resource's contents.
	 *
	 *  @param  index The index of the resource we want.
	 *  @param  tryNoCopy Try to return a SeekableSubReadStream of the archive instead of copying.
	 *  @return A (sub)stream of the resource's contents.
	 */
	virtual Common::SeekableReadStream *getResource(uint32 index, bool tryNoCopy = false) const = 0;

	/** Return with which algorithm the name is hashed. */
	virtual Common::HashAlgo getNameHashAlgo() const;

	/** Return the index of the resource matching the hash, or 0xFFFFFFFF if not found. */
	uint32 findResource(uint64 hash) const;
	/** Return the index of the resource matching the name and type, or 0xFFFFFFFF if not found. */
	uint32 findResource(const Common::UString &name, FileType type) const;
};

} // End of namespace Aurora

#endif // AURORA_ARCHIVE_H
