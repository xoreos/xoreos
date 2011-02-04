/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file aurora/ndsrom.h
 *  Nintendo DS ROM parsing.
 */

#ifndef AURORA_NDSROM_H
#define AURORA_NDSROM_H

#include <vector>

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
class NDSFile : public Archive {
public:
	NDSFile(const Common::UString &fileName);
	~NDSFile();

	/** Clear the resource list. */
	void clear();

	/** Return the list of resources. */
	const ResourceList &getResources() const;

	/** Return a stream of the resource's contents. */
	Common::SeekableReadStream *getResource(uint32 index) const;

	/** Check if a stream is a valid Nintendo DS ROM. */
	static bool isNDS(Common::SeekableReadStream &stream);

private:
	/** Internal resource information. */
	struct IResource {
		uint32 offset; ///< The offset of the resource within the NDS.
		uint32 size;   ///< The resource's size.
	};

	typedef std::vector<IResource> IResourceList;

	/** External list of resource names and types. */
	ResourceList _resources;

	/** Internal list of resource offsets and sizes. */
	IResourceList _iResources;

	/** The name of the NDS file. */
	Common::UString _fileName;

	void open(Common::File &file) const;

	void load();
	void readNames(Common::SeekableReadStream &nds, uint32 offset, uint32 length);
	void readFAT(Common::SeekableReadStream &nds, uint32 offset);
};

} // End of namespace Aurora

#endif // AURORA_NDSROM_H
