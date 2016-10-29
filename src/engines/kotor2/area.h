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

#include "src/common/ptrlist.h"
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

class Module;
class Room;

/** An area in Star Wars: Knights of the Old Republic II - The Sith Lords,
 *  holding all objects and rooms within, as well as general area properties
 *  like the current background music and ambient sounds.
 *
 *  Note: the coordinate system of the in-world graphics is oriented as viewed
 *  from the top down. The negative Z axis goes down into the ground, while the
 *  positive Y axis points due north and the positive X axis points due east.
 */
class Area : public KotOR2::Object, public Events::Notifyable {
public:
	Area(Module &module, const Common::UString &resRef);
	~Area();

	// General properties

	/** Return the area's resref (resource ID). */
	const Common::UString &getResRef();

	// Visibility

	void show();
	void hide();

	// Music/Sound

	uint32 getMusicDayTrack   () const; ///< Return the music track ID playing by day.
	uint32 getMusicNightTrack () const; ///< Return the music track ID playing by night.
	uint32 getMusicBattleTrack() const; ///< Return the music track ID playing in battle.

	void setMusicDayTrack   (uint32 track); ///< Set the music track ID playing by day.
	void setMusicNightTrack (uint32 track); ///< Set the music track ID playing by night.
	void setMusicBattleTrack(uint32 track); ///< Set the music track ID playing in battle.

	void stopSound();        ///< Stop all sounds.
	void stopAmbientMusic(); ///< Stop the ambient music.
	void stopAmbientSound(); ///< Stop the ambient sound.

	/** Play the specified music (or the area's default) as ambient music. */
	void playAmbientMusic(Common::UString music = "");
	/** Play the specified sound (or the area's default) as ambient sound. */
	void playAmbientSound(Common::UString sound = "");

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
	typedef Common::PtrList<Room> RoomList;

	typedef Common::PtrList<KotOR2::Object> ObjectList;
	typedef std::map<uint32, KotOR2::Object *> ObjectMap;


	Module *_module; ///< The module this area is in.

	Common::UString _resRef;      ///< The area's resref (resource ID).
	Common::UString _displayName; ///< The area's localized display name.

	Common::UString _ambientDay;   ///< Ambient sound that plays by day.
	Common::UString _ambientNight; ///< Ambient sound that plays by night.

	uint32 _musicDayTrack;    ///< Music track ID that plays by day.
	uint32 _musicNightTrack;  ///< Music track ID that plays by night.
	uint32 _musicBattleTrack; ///< Music track ID that plays in battle.

	Common::UString _musicDay;    ///< Music that plays by day.
	Common::UString _musicNight;  ///< Music that plays by night.
	Common::UString _musicBattle; ///< Music that plays in battle.

	/** Battle music stingers. */
	std::vector<Common::UString> _musicBattleStinger;

	float _ambientDayVol;   ///< Day ambient sound volume.
	float _ambientNightVol; ///< Night ambient sound volume.

	bool _visible; ///< Is the area currently visible?

	Sound::ChannelHandle _ambientSound; ///< Sound handle of the currently playing sound.
	Sound::ChannelHandle _ambientMusic; ///< Sound handle of the currently playing music.

	Aurora::LYTFile _lyt; ///< The area's layout description.
	Aurora::VISFile _vis; ///< The area's inter-room visibility description.

	RoomList _rooms; ///< All rooms in the area.

	ObjectList _objects;   ///< List of all objects in the area.
	ObjectMap  _objectMap; ///< Map of all non-static objects in the area.

	/** The currently active (highlighted) object. */
	KotOR2::Object *_activeObject;

	bool _highlightAll; ///< Are we currently highlighting all objects?

	std::list<Events::Event> _eventQueue; ///< The event queue.

	Common::Mutex _mutex; ///< Mutex securing access to the area.


	// Loading helpers

	void clear();
	void load();

	void loadLYT();
	void loadVIS();

	void loadARE(const Aurora::GFF3Struct &are);
	void loadGIT(const Aurora::GFF3Struct &git);

	void loadRooms();

	void loadProperties(const Aurora::GFF3Struct &props);

	void loadObject(KotOR2::Object &object);

	void loadWaypoints (const Aurora::GFF3List &list);
	void loadPlaceables(const Aurora::GFF3List &list);
	void loadDoors     (const Aurora::GFF3List &list);
	void loadCreatures (const Aurora::GFF3List &list);

	void unload();

	// Highlight / active helpers

	void checkActive(int x = -1, int y = -1);
	void setActive(KotOR2::Object *object);
	KotOR2::Object *getObjectAt(int x, int y);

	void highlightAll(bool enabled);

	void click(int x, int y);


	friend class Console;
};

} // End of namespace KotOR2

} // End of namespace Engines

#endif // ENGINES_KOTOR2_AREA_H
