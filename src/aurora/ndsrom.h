/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
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

#include <list>

#include "common/types.h"
#include "common/ustring.h"

#include "aurora/types.h"

namespace Aurora {

class SeekableReadStream;

/** A class encapsulating Nintendo DS ROM access. */
class NDSFile {
public:
	/** A resource. */
	struct Resource {
		Common::UString name; ///< The resource's name.
		FileType        type; ///< The resource's type.

		uint32 offset; ///< The resource's offset within the NDS.
		uint32 size;   ///< The resource's size.
	};

	typedef std::list<Resource> ResourceList;

	NDSFile();
	~NDSFile();

	/** Clear all resource information. */
	void clear();

	/** Load an NDS file.
	 *
	 *  @param nds A stream of an NDS file.
	 */
	void load(Common::SeekableReadStream &nds);

	/** Return a list of all containing resources. */
	const ResourceList &getResources() const;

	/** Return a stream of the resource found at this offset. */
	static Common::SeekableReadStream *getResource(Common::SeekableReadStream &stream,
			uint32 offset, uint32 size);

	/** Check if a stream is a valid Nintendo DS ROM. */
	static bool isNDS(Common::SeekableReadStream &stream);

private:
	ResourceList _resources; ///< All containing resources.

	void readNames(Common::SeekableReadStream &nds, uint32 offset, uint32 length);
	void readFAT(Common::SeekableReadStream &nds, uint32 offset);
};

} // End of namespace Aurora

#endif
