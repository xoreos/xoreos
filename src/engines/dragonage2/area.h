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
 *  The context holding a Dragon Age II area.
 */

#ifndef ENGINES_DRAGONAGE2_AREA_H
#define ENGINES_DRAGONAGE2_AREA_H

#include <vector>
#include <list>
#include <map>

#include "src/common/ptrlist.h"
#include "src/common/ptrvector.h"
#include "src/common/ustring.h"
#include "src/common/mutex.h"

#include "src/aurora/types.h"
#include "src/aurora/locstring.h"

#include "src/events/types.h"
#include "src/events/notifyable.h"

#include "src/engines/aurora/resources.h"

#include "src/engines/dragonage2/object.h"

namespace Engines {

namespace DragonAge2 {

class Campaign;

class Room;
class Object;

/** An area in Dragon Age II, holding all objects and rooms within, as well as
 *  general area properties like the current background music and ambient
 *  sounds.
 *
 *  Note: the coordinate system of the in-world graphics is oriented as viewed
 *  from the top down. The negative Z axis goes down into the ground, while the
 *  positive Y axis points due north and the positive X axis points due east.
 */
class Area : public DragonAge2::Object, public Events::Notifyable {
public:
	Area(Campaign &campaign, const Common::UString &resRef,
	     const Common::UString &env, const Common::UString &rim);
	~Area();

	const Common::UString   &getResRef() const;
	const Aurora::LocString &getName()   const;

	/** Return the position and orientation the PC should enter this area. */
	void getEntryLocation(float &posX, float &posY, float &posZ,
	                      float &orientX, float &orientY, float &orientZ, float &orientAngle) const;

	void show();
	void hide();

	/** Add a single event for consideration into the area event queue. */
	void addEvent(const Events::Event &event);
	/** Process the current event queue. */
	void processEventQueue();

	/** Forcibly remove the focus from the currently highlighted object. */
	void removeFocus();

	static Common::UString getName(const Common::UString &resRef, const Common::UString &rimFile = "");


protected:
	void notifyCameraMoved();


private:
	typedef Common::PtrVector<Room> Rooms;

	typedef Common::PtrList<DragonAge2::Object> Objects;
	typedef std::map<uint32_t, DragonAge2::Object *> ObjectMap;


	Campaign *_campaign;

	Common::UString _resRef;

	uint32_t _environmentID;
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

	/** The currently active (highlighted) object. */
	DragonAge2::Object *_activeObject;

	bool _highlightAll; ///< Are we currently highlighting all objects?

	std::recursive_mutex _mutex;


	void load(const Common::UString &resRef, const Common::UString &env, const Common::UString &rim);
	void loadEnvironment(const Common::UString &resRef);
	void loadARE(const Common::UString &resRef);

	void loadObject(DragonAge2::Object &object);
	void loadWaypoints (const Aurora::GFF3List &list);
	void loadPlaceables(const Aurora::GFF3List &list);
	void loadCreatures (const Aurora::GFF3List &list);

	void checkActive(int x = -1, int y = -1);
	void setActive(DragonAge2::Object *object);
	DragonAge2::Object *getObjectAt(int x, int y);

	void highlightAll(bool enabled);

	void click(int x, int y);

	void clean();
};

} // End of namespace DragonAge2

} // End of namespace Engines

#endif // ENGINES_DRAGONAGE2_AREA_H
