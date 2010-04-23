/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file aurora/rimfile.h
 *  Handling BioWare's RIMs (resource archives).
 */

#ifndef AURORA_RIMFILE_H
#define AURORA_RIMFILE_H

#include <vector>

#include "common/types.h"
#include "common/ustring.h"

#include "aurora/types.h"
#include "aurora/aurorafile.h"
#include "aurora/locstring.h"

namespace Common {
	class SeekableReadStream;
}

namespace Aurora {

/** Class to hold resource data of a RIM file. */
class RIMFile : public AuroraBase {
public:
	/** A resource. */
	struct Resource {
		Common::UString name; ///< The resource's name.
		FileType        type; ///< The resource's type.

		uint32 offset; ///< The resource's offset within the RIM.
		uint32 size;   ///< The resource's size.
	};

	typedef std::vector<Resource> ResourceList;

	RIMFile();
	~RIMFile();

	/** Clear all resource information. */
	void clear();

	/** Load a RIM file.
	 *
	 *  @param rim A stream of a RIM file.
	 */
	void load(Common::SeekableReadStream &rim);

	/** Return a list of all containing resources. */
	const ResourceList &getResources() const;

	/** Return a stream of the resource found at this offset. */
	static Common::SeekableReadStream *getResource(Common::SeekableReadStream &stream,
			uint32 offset, uint32 size);

private:
	ResourceList _resources; ///< All containing resources.

	void readResList(Common::SeekableReadStream &rim, uint32 offset);
};

} // End of namespace Aurora

#endif // AURORA_RIMFILE_H
