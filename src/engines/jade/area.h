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

#include <memory>
#include "src/common/ptrlist.h"
#include "src/common/mutex.h"

#include "src/sound/types.h"

#include "src/aurora/types.h"

#include "src/events/types.h"
#include "src/events/notifyable.h"

#include "src/engines/jade/arealayout.h"
#include "src/engines/jade/module.h"
#include "src/engines/jade/object.h"

namespace Sound {
	class XACTSoundBank;
}

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
class Area : public AreaLayout, public Object, public Events::Notifyable {
public:
	Area(Module &module, const Common::UString &resRef);
	~Area();

	// Visibility

	void show();
	void hide();

	// Music/Sound

	void playMusic();
	void stopMusic();

	// Events

	/** Add a single event for consideration into the area event queue. */
	void addEvent(const Events::Event &event);
	/** Process the current event queue. */
	void processEventQueue();

	/** Forcibly remove the focus from the currently highlighted object. */
	void removeFocus();


protected:
	void notifyCameraMoved();


private:
	typedef Common::PtrList<Object> ObjectList;
	typedef std::map<uint32_t, Object *> ObjectMap;


	Module *_module; ///< The module this area is in.

	std::unique_ptr<Sound::XACTSoundBank> _musicBank;
	Sound::ChannelHandle _music;

	int32_t _ambientMusicState;
	int32_t _combatMusicState;

	ObjectList _objects;   ///< List of all objects in the area.
	ObjectMap  _objectMap; ///< Map of all non-static objects in the area.

	/** The currently active (highlighted) object. */
	Jade::Object *_activeObject;

	bool _highlightAll; ///< Are we currently highlighting all objects?

	std::list<Events::Event> _eventQueue; ///< The event queue.

	std::recursive_mutex _mutex; ///< Mutex securing access to the area.

	// Loading helpers

	void clear();
	void load();

	void loadARE(const Aurora::GFF3Struct &are);
	void loadSAV(const Aurora::GFF3Struct &sav);

	void loadResources();

	void loadObject(Jade::Object &object);

	void loadWaypoints (const Aurora::GFF3List &list);
	void loadCreatures (const Aurora::GFF3List &list);
	void loadPlaceables(const Aurora::GFF3List &list);
	void loadTriggers  (const Aurora::GFF3List &list);

	// Highlight / active helpers

	void checkActive(int x = -1, int y = -1);
	void setActive(Jade::Object *object);
	Jade::Object *getObjectAt(int x, int y);

	void highlightAll(bool enabled);

	void click(int x, int y);
};

} // End of namespace Jade

} // End of namespace Engines

#endif // ENGINES_JADE_AREA_H
