/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

#ifndef AURORA_KEYFILE_H
#define AURORA_KEYFILE_H

#include <vector>

#include "common/types.h"

#include "aurora/types.h"

namespace Common {
	class SeekableReadStream;
}

namespace Aurora {

/** Class to hold resource index information of a key file. */
class KeyFile {
public:
	/** A resource. */
	struct Resource {
		std::string name; ///< The resource's name.
		FileType    type; ///< The resource's type.

		uint32 bifIndex; ///< Index into the bif list.
		uint32 resIndex; ///< Index into the bif's resource table.
	};

	typedef std::vector<Resource> ResourceList;
	typedef std::vector<std::string> BifList;

	KeyFile();
	~KeyFile();

	/** Clear all resource informations. */
	void clear();

	/** Load a resource index.
	 *
	 *  @param  key A stream of an indexing key file.
	 *  @return true if loading was successful, false otherwise.
	 */
	bool load(Common::SeekableReadStream &key);

	/** Return a list of all managed bifs. */
	const BifList &getBifs() const;

	/** Return a list of all containing resources. */
	const ResourceList &getResources() const;

private:
	BifList      _bifs;      ///< All managed bifs.
	ResourceList _resources; ///< All containing resources.

	bool readBifList(Common::SeekableReadStream &key, uint32 bifCount);
	bool readKeyList(Common::SeekableReadStream &key, uint32 keyCount);
};

} // End of namespace Aurora

#endif // AURORA_KEYFILE_H
