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
 *  The context holding a Dragon Age: Origins area.
 */

#ifndef ENGINES_DRAGONAGE_AREA_H
#define ENGINES_DRAGONAGE_AREA_H

#include <vector>
#include <list>
#include <map>

#include "src/common/ustring.h"

#include "src/aurora/types.h"
#include "src/aurora/locstring.h"

#include "src/events/types.h"
#include "src/events/notifyable.h"

#include "src/engines/aurora/resources.h"

#include "src/engines/dragonage/scriptobject.h"

namespace Engines {

namespace DragonAge {

class Room;
class Object;

class Area : public ScriptObject, public Events::Notifyable {
public:
	Area(const Common::UString &resRef, const Common::UString &env, const Common::UString &rim);
	~Area();

	const Common::UString   &getResRef() const;
	const Aurora::LocString &getName()   const;

	void show();
	void hide();

	/** Add a single event for consideration into the area event queue. */
	void addEvent(const Events::Event &event);
	/** Process the current event queue. */
	void processEventQueue();

	static Aurora::LocString getName(const Common::UString &resRef);


protected:
	void notifyCameraMoved();


private:
	typedef std::vector<Room *> Rooms;

	typedef std::list<DragonAge::Object *> Objects;
	typedef std::map<uint32, DragonAge::Object *> ObjectMap;


	Common::UString _resRef;

	Common::UString _environmentName;
	Common::UString _skyDome;

	Common::UString _startPoint;
	Common::UString _script;

	Aurora::LocString _name;

	Rooms _rooms;

	ChangeList _resources;
	std::list<Events::Event> _eventQueue;

	Objects    _objects;   ///< List of all objects in the area.
	ObjectMap  _objectMap; ///< Map of objects by their model IDs.


	void load(const Common::UString &resRef, const Common::UString &env, const Common::UString &rim);
	void loadEnvironment(const Common::UString &resRef);
	void loadARE(const Common::UString &resRef);

	void loadObject(DragonAge::Object &object);
	void loadWaypoints (const Aurora::GFF3List &list);
	void loadPlaceables(const Aurora::GFF3List &list);

	void clean();
};

} // End of namespace DragonAge

} // End of namespace Engines

#endif // ENGINES_DRAGONAGE_AREA_H
