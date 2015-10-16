/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * xoreos is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * xoreos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xoreos. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file
 *  Handling BioWare's VISs (Visibility files).
 */

#ifndef AURORA_VISFILE_H
#define AURORA_VISFILE_H

#include <map>
#include <vector>

#include "src/common/types.h"
#include "src/common/ustring.h"

#include "src/aurora/types.h"

namespace Common {
	class SeekableReadStream;
}

namespace Aurora {

/** A VIS, BioWare's Visibility Format.
 *
 *  A VIS file is a simple ASCII file defining the inter-room visibility in
 *  an area, i.e. which rooms are visible from any given other room. The
 *  definitions of the rooms themselves can be found in LYT files (see
 *  class LYTFile).
 *
 *  VIS files are used by the two Knights of the Old Republic games and
 *  Jade Empire.
 */
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

	/** Return the visibility array for a given room.
	 *
	 *  This is a list of rooms that are visible from that given room
	 */
	const std::vector<Common::UString> &getVisibilityArray(const Common::UString &room) const;

private:
	std::map<Common::UString, std::vector<Common::UString> > _map;
};

} // End of namespace Aurora

#endif // AURORA_VISFILE_H
