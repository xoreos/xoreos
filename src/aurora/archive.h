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

#include "common/types.h"
#include "common/ustring.h"
#include "common/hash.h"

#include "aurora/types.h"

namespace Common {
	class SeekableReadStream;
}

namespace Aurora {

/** An abstract file archive. */
class Archive {
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

	/** Clear the resource list. */
	virtual void clear() = 0;

	/** Return the list of resources. */
	virtual const ResourceList &getResources() const = 0;

	/** Return the size of a resource. */
	virtual uint32 getResourceSize(uint32 index) const;

	/** Return a stream of the resource's contents. */
	virtual Common::SeekableReadStream *getResource(uint32 index) const = 0;

	/** Return with which algorithm the name is hashed. */
	virtual Common::HashAlgo getNameHashAlgo() const;
};

} // End of namespace Aurora

#endif // AURORA_ARCHIVE_H
