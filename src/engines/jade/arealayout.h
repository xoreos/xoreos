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
 *  A layout for an area.
 */

#ifndef ENGINES_JADE_AREALAYOUT_H
#define ENGINES_JADE_AREALAYOUT_H

#include <list>

#include "src/common/configfile.h"
#include "src/common/ustring.h"
#include "src/common/changeid.h"

#include "src/aurora/lytfile.h"
#include "src/aurora/visfile.h"

namespace Engines {

namespace Jade {

class Room;
class Object;

/** A Jade area layout. */
class AreaLayout {
public:
	AreaLayout(const Common::UString &resRef);
	virtual ~AreaLayout();

	// General properties

	/** Return the area's resref (resource ID). */
	const Common::UString &getResRef();

	// Visibility

	virtual void show();
	virtual void hide();

	// Update Camera with configuration from the current room.
	void updateCamera();

protected:
	// Only used to init class Area.
	AreaLayout();

	Common::UString _resRef;      ///< The area's resref (resource ID).
	Common::UString _layout;      ///< The area's layout resref (resource ID).

	std::list<Common::ChangeID> _resources; ///< The area's resource archives.

	bool _visible; ///< Is the area currently visible?

	// Loading helpers

	virtual void clear();
	virtual void load();

	virtual void loadResources();

private:
	typedef std::list<Room *>   RoomList;


	RoomList _rooms; ///< All rooms in the area.

	Aurora::LYTFile _lyt; ///< The area's layout description.
	Aurora::VISFile _vis; ///< The area's inter-room visibility description.
	Common::ConfigFile _art; ///< The area's environment properties.

	void loadLYT();
	void loadVIS();
	void loadART();

	void loadRooms();

	Room *currentRoom() const;
};

} // End of namespace Jade

} // End of namespace Engines

#endif // ENGINES_JADE_AREALAYOUT_H
