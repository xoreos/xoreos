/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file aurora/visfile.h
 *  Handling BioWare's VISs (Visibility files).
 */

#ifndef AURORA_VISFILE_H
#define AURORA_VISFILE_H

#include <map>
#include <vector>

#include "common/types.h"
#include "common/ustring.h"

#include "aurora/types.h"

namespace Common {
	class SeekableReadStream;
}

namespace Aurora {

/** A VIS, BioWare's Visibility Format. */
class VISFile {
public:
	VISFile();
	~VISFile();

	/** Clear all information. */
	void clear();

	/** Load a VIS file.
	 *
	 *  @param vis A stream of an VIS file.
	 */
	void load(Common::SeekableReadStream &vis);

	const std::vector<Common::UString> &getVisibilityArray(const Common::UString &room);

private:
	std::map<Common::UString, std::vector<Common::UString> > _map;
};

} // End of namespace Aurora

#endif // AURORA_VISFILE_H
