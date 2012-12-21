/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
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

/** @file aurora/herffile.h
 *  BioWare's HERF file parsing.
 */

#ifndef AURORA_HERFFILE_H
#define AURORA_HERFFILE_H

#include <map>

#include "common/types.h"
#include "common/ustring.h"

#include "aurora/types.h"
#include "aurora/archive.h"

namespace Common {
	class SeekableReadStream;
	class File;
}

namespace Aurora {

/** A class encapsulating Nintendo DS ROM access. */
class HERFFile : public Archive {
public:
	HERFFile(const Common::UString &fileName);
	~HERFFile();

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
		uint32 offset;   ///< The offset of the resource within the HERF.
		uint32 size;     ///< The resource's size.
	};

	typedef std::map<uint32, IResource> IResourceMap;

	/** External list of resource names and types. */
	ResourceList _resources;

	/** Internal list of resource offsets and sizes. */
	IResourceMap _iResources;

	/** The name of the HERF file. */
	Common::UString _fileName;

	void open(Common::File &file) const;

	void load();
	void readNames();

	const IResource &getIResource(uint32 index) const;

	/** Hash a string for indexing into the file */
	uint32 hashString(const Common::UString &string);
};

} // End of namespace Aurora

#endif // AURORA_NDSROM_H
