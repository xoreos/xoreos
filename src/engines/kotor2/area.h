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
 *  The context holding a Star Wars: Knights of the Old Republic II - The Sith Lords area.
 */

#ifndef ENGINES_KOTOR2_AREA_H
#define ENGINES_KOTOR2_AREA_H

#include <vector>
#include <list>
#include <map>

#include "src/common/ustring.h"
#include "src/common/mutex.h"

#include "src/aurora/types.h"
#include "src/aurora/lytfile.h"
#include "src/aurora/visfile.h"

#include "src/sound/types.h"

#include "src/events/types.h"
#include "src/events/notifyable.h"

#include "src/engines/kotor2/object.h"

namespace Engines {

namespace KotOR2 {

class Room;

/** A KotOR2 area. */
class Area : public KotOR2::Object, public Events::Notifyable {
public:
	Area();
	~Area();

	void load(const Common::UString &resRef);

	/** Return the area's localized name. */
	const Common::UString &getName();

	void show();
	void hide();

	void addEvent(const Events::Event &event);
	void processEventQueue();

	void removeFocus();

	void playAmbientMusic(Common::UString music = "");
	void playAmbientSound(Common::UString sound = "");

	void stopSound();
	void stopAmbientMusic();
	void stopAmbientSound();


protected:
	void notifyCameraMoved();


private:
	typedef std::list<Room *>   RoomList;
	typedef std::list<Object *> ObjectList;

	typedef std::map<uint32, Object *> ObjectMap;


	bool _loaded;

	Common::UString _resRef;
	Common::UString _name;

	Common::UString _ambientDay;
	Common::UString _ambientNight;

	Common::UString _musicDay;
	Common::UString _musicNight;
	Common::UString _musicBattle;

	std::vector<Common::UString> _musicBattleStinger;

	float _ambientDayVol;
	float _ambientNightVol;

	bool _visible;

	Sound::ChannelHandle _ambientSound;
	Sound::ChannelHandle _ambientMusic;

	Aurora::LYTFile _lyt;
	Aurora::VISFile _vis;

	RoomList _rooms;

	ObjectList _objects;
	ObjectMap  _objectMap;

	Object *_activeObject;

	bool _highlightAll;

	std::list<Events::Event> _eventQueue;

	Common::Mutex _mutex;


	void loadLYT();
	void loadVIS();

	void loadARE(const Aurora::GFF3Struct &are);
	void loadGIT(const Aurora::GFF3Struct &git);

	void loadRooms();

	void loadProperties(const Aurora::GFF3Struct &props);

	void loadObject(Object &object);

	void loadPlaceables(const Aurora::GFF3List &list);
	void loadDoors     (const Aurora::GFF3List &list);
	void loadCreatures (const Aurora::GFF3List &list);

	void unload();

	void checkActive();
	void setActive(Object *object);
	Object *getObjectAt(int x, int y);

	void highlightAll(bool enabled);

	friend class Console;
};

} // End of namespace KotOR2

} // End of namespace Engines

#endif // ENGINES_KOTOR2_AREA_H
