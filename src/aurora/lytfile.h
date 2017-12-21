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
 *  Handling BioWare's LYTs (Layout files).
 */

#ifndef AURORA_LYTFILE_H
#define AURORA_LYTFILE_H

#include <vector>

#include "src/common/types.h"
#include "src/common/ustring.h"

namespace Common {
	class SeekableReadStream;
}

namespace Aurora {

/** An LYT, BioWare's Layout Format.
 *
 *  A LYT file is a simple ASCII file specifying the layout of an area in
 *  the two Knights of the Old Republic games and Jade Empire. The LYT
 *  defines the general visual structure of an area.
 *
 *  This includes:
 *  - Rooms: a single model making up a part of the area geometry
 *  - Door hooks: coordinates where doors can be placed
 *  - Art placeables: simple placeable objects
 *
 *  As well as these fields we don't support yet:
 *  - File dependencies
 *  - Obstacles
 *  - Walk mesh rooms
 */
class LYTFile {
public:
	/** A room. */
	struct Room {
		Common::UString model;
		float x, y, z;
		bool canWalk;
	};

	/** A simple, non-interactive placeable. */
	struct ArtPlaceable {
		Common::UString model;
		float x, y, z;
	};

	/** A place a door hooks into. */
	struct DoorHook {
		Common::UString room;
		Common::UString name;
		float x, y, z;
		float unk1, unk2, unk3, unk4, unk5;
	};

	typedef std::vector<Room> RoomArray;
	typedef std::vector<ArtPlaceable> ArtPlaceableArray;
	typedef std::vector<DoorHook> DoorHookArray;

	LYTFile();
	~LYTFile();

	/** Clear all information. */
	void clear();

	/** Load a LYT file.
	 *
	 *  @param lyt A stream of an LYT file.
	 */
	void load(Common::SeekableReadStream &lyt);

	/** Get all rooms in this layout. */
	const RoomArray &getRooms() const;

	/** Get art placeables in this layout. */
	const ArtPlaceableArray &getArtPlaceables() const;

	/** Get all door hooks in this layout. */
	const DoorHookArray &getDoorHooks() const;

	/** Get the file dependency in this layout. */
	Common::UString getFileDependency() const;

private:
	RoomArray _rooms;
	ArtPlaceableArray _artPlaceables;
	DoorHookArray _doorHooks;
	Common::UString _fileDependency;

	void assertTokenCount(const std::vector<Common::UString> &tokens, size_t n,
	                      const Common::UString &name);
};

} // End of namespace Aurora

#endif // AURORA_LYTFILE_H
