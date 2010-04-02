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

#include "common/types.h"

#include "aurora/types.h"

namespace Common {
	class SeekableReadStream;
}

namespace Aurora {

/** Class to hold resource data of an erf file. */
class ERFFile {
public:
	ERFFile();
	~ERFFile();

	/** Clear all resource information. */
	void clear();

	/** Load an erf file.
	 *
	 *  @param  erf A stream of an erf file.
	 *  @return true if loading was successful, false otherwise.
	 */
	bool load(Common::SeekableReadStream &erf);

private:
	uint32 _id;
	uint32 _version;
};

} // End of namespace Aurora

#endif // AURORA_ERFFILE_H
