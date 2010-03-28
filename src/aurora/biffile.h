/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

#ifndef AURORA_BIFFILE_H
#define AURORA_BIFFILE_H

#include <vector>

#include "common/types.h"

#include "aurora/types.h"

namespace Common {
	class SeekableReadStream;
}

namespace Aurora {

/** Class to hold resource data information of a bif file. */
class BifFile {
public:
	/** A resource. */
	struct Resource {
		FileType type; ///< The resource's type.

		uint32 offset; ///< The resource's offset into this bif file.
		uint32 size;   ///< The resource's size.
	};

	typedef std::vector<Resource> ResourceList;

	BifFile();
	~BifFile();

	/** Clear all resource informations. */
	void clear();

	/** Load a resource data file.
	 *
	 *  @param  bif A stream of a data bif file.
	 *  @return true if loading was successful, false otherwise.
	 *
	 */
	bool load(Common::SeekableReadStream &bif);

	/** Return a list of all containing resources. */
	const ResourceList &getResources() const;

private:
	ResourceList _resources; ///< All resources within the bif.

	bool readVarResTable(Common::SeekableReadStream &bif, uint32 varResCount);
};

} // End of namespace Aurora

#endif // AURORA_BIFFILE_H
