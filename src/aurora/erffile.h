/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file aurora/erffile.h
 *  Handling BioWare's ERFs (encapsulated resource file).
 */

#ifndef AURORA_ERFFILE_H
#define AURORA_ERFFILE_H

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

/** Class to hold resource data of an ERF file. */
class ERFFile : public AuroraBase {
public:
	/** A resource. */
	struct Resource {
		Common::UString name; ///< The resource's name.
		FileType        type; ///< The resource's type.

		uint32 offset; ///< The resource's offset within the ERF.
		uint32 size;   ///< The resource's size.
	};

	typedef std::vector<Resource> ResourceList;

	ERFFile();
	~ERFFile();

	/** Clear all resource information. */
	void clear();

	/** Load an ERF file.
	 *
	 *  @param erf A stream of an ERF file.
	 */
	void load(Common::SeekableReadStream &erf);

	/** Return the description. */
	const LocString &getDescription() const;

	/** Return a list of all containing resources. */
	const ResourceList &getResources() const;

private:
	uint32 _langCount;     ///< Number of language strings in the description.
	uint32 _descriptionID; ///< ID of the description.

	uint32 _offDescription; ///< Offset to the description.
	uint32 _offKeyList;     ///< Offset to the key list.
	uint32 _offResList;     ///< Offset to the resource list.

	LocString _description; ///< The ERF's description.

	ResourceList _resources; ///< All containing resources.

	void readERFHeader  (Common::SeekableReadStream &erf);
	void readDescription(Common::SeekableReadStream &erf);
	void readResources  (Common::SeekableReadStream &erf);

	// V1.0
	void readV1ResList(Common::SeekableReadStream &erf);
	void readV1KeyList(Common::SeekableReadStream &erf);

	// V2.0
	void readV2ResList(Common::SeekableReadStream &erf);
};

} // End of namespace Aurora

#endif // AURORA_ERFFILE_H
