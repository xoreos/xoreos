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

	const std::vector<Common::UString> &getVisibilityArray(Common::UString room) const;

private:
	std::map<Common::UString, std::vector<Common::UString> > _map;
};

} // End of namespace Aurora

#endif // AURORA_VISFILE_H
