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
 *  An area.
 */

#ifndef ENGINES_JADE_AREA_H
#define ENGINES_JADE_AREA_H

#include <list>
#include <map>

#include "src/common/ustring.h"
#include "src/common/changeid.h"
#include "src/common/mutex.h"

#include "src/aurora/types.h"
#include "src/aurora/lytfile.h"
#include "src/aurora/visfile.h"

#include "src/events/types.h"
#include "src/events/notifyable.h"

#include "src/engines/jade/object.h"

namespace Engines {

namespace Jade {

class Room;

/** An area in Jade Empire, holding all objects and rooms within, as well as
 *  general area properties like the current background music and ambient
 *  sounds.
 *
 *  Note: the coordinate system of the in-world graphics is oriented as viewed
 *  from the top down. The negative Z axis goes down into the ground, while the
 *  positive Y axis points due north and the positive X axis points due east.
 */
class Area : public Object, public Events::Notifyable {
public:
	Area();
	~Area();

	void load(const Common::UString &resRef);

	const Common::UString &getName();

	void show();
	void hide();

	void addEvent(const Events::Event &event);
	void processEventQueue();

	void removeFocus();


protected:
	void notifyCameraMoved();


private:
	typedef std::list<Room *>   RoomList;
	typedef std::list<Object *> ObjectList;

	typedef std::map<uint32, Object *> ObjectMap;


	bool _loaded;

	Common::UString _resRef;
	Common::UString _layout;

	bool _visible;

	std::list<Common::ChangeID> _resources;

	Aurora::LYTFile _lyt;
	Aurora::VISFile _vis;

	RoomList _rooms;

	ObjectList _objects;
	ObjectMap  _objectMap;

	Object *_activeObject;

	bool _highlightAll;

	std::list<Events::Event> _eventQueue;

	Common::Mutex _mutex;


	void loadARE(const Aurora::GFF3Struct &are);

	void loadResources();

	void loadLYT();
	void loadVIS();

	void loadRooms();
	void loadArtPlaceables();

	void loadObject(Object &object);

	void unload();

	void checkActive();
	void setActive(Object *object);
	Object *getObjectAt(int x, int y);

	void highlightAll(bool enabled);
};

} // End of namespace Jade

} // End of namespace Engines

#endif // ENGINES_JADE_AREA_H
