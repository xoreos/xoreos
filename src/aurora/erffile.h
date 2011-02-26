/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
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
#include "aurora/archive.h"
#include "aurora/aurorafile.h"
#include "aurora/locstring.h"

namespace Common {
	class SeekableReadStream;
	class File;
}

namespace Aurora {

/** Class to hold resource data of an ERF file. */
class ERFFile : public Archive, public AuroraBase {
public:
	ERFFile(const Common::UString &fileName, bool noResources = false);
	~ERFFile();

	/** Clear the resource list. */
	void clear();

	/** Return the list of resources. */
	const ResourceList &getResources() const;

	/** Return a stream of the resource's contents. */
	Common::SeekableReadStream *getResource(uint32 index) const;

	/** Return the description. */
	const LocString &getDescription() const;

private:
	/** The header of an ERF file. */
	struct ERFHeader {
		uint32 langCount;      ///< Number of language strings in the description.
		uint32 descriptionID;  ///< ID of the description.

		uint32 offDescription; ///< Offset to the description.
		uint32 offKeyList;     ///< Offset to the key list.
		uint32 offResList;     ///< Offset to the resource list.
	};

	/** Internal resource information. */
	struct IResource {
		uint32 offset; ///< The offset of the resource within the BIF.
		uint32 size;   ///< The resource's size.
	};

	typedef std::vector<IResource> IResourceList;

	bool _noResources;

	/** The ERF's description. */
	LocString _description;

	/** External list of resource names and types. */
	ResourceList _resources;

	/** Internal list of resource offsets and sizes. */
	IResourceList _iResources;

	/** The name of the ERF file. */
	Common::UString _fileName;

	void open(Common::File &file) const;

	void load();

	void readERFHeader  (Common::SeekableReadStream &erf,       ERFHeader &header);
	void readDescription(Common::SeekableReadStream &erf, const ERFHeader &header);
	void readResources  (Common::SeekableReadStream &erf, const ERFHeader &header);

	// V1.0
	void readV1ResList(Common::SeekableReadStream &erf, const ERFHeader &header);
	void readV1KeyList(Common::SeekableReadStream &erf, const ERFHeader &header);

	// V2.0
	void readV2ResList(Common::SeekableReadStream &erf, const ERFHeader &header);
};

} // End of namespace Aurora

#endif // AURORA_ERFFILE_H
