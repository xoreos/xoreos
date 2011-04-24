/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file aurora/archive.h
 *  Handling various archive files.
 */

#ifndef AURORA_ARCHIVE_H
#define AURORA_ARCHIVE_H

#include <list>

#include "common/types.h"
#include "common/ustring.h"

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
		FileType        type;  ///< The resource's type.
		uint32          index; ///< The resource's local index within the archive.
	};

	typedef std::list<Resource> ResourceList;

	Archive();
	virtual ~Archive();

	/** Clear the resource list. */
	virtual void clear() = 0;

	/** Return the list of resources. */
	virtual const ResourceList &getResources() const = 0;

	/** Return a stream of the resource's contents. */
	virtual Common::SeekableReadStream *getResource(uint32 index) const = 0;
};

} // End of namespace Aurora

#endif // AURORA_ARCHIVE_H
