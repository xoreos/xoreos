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
 *  The context holding an area in KotOR games.
 */

#ifndef ENGINES_KOTORBASE_AREA_H
#define ENGINES_KOTORBASE_AREA_H

#include <vector>
#include <list>
#include <map>
#include <set>

#include "src/common/ptrlist.h"
#include "src/common/ustring.h"
#include "src/common/mutex.h"
#include <memory>

#include "src/aurora/types.h"
#include "src/aurora/lytfile.h"
#include "src/aurora/visfile.h"

#include "src/sound/types.h"

#include "src/events/types.h"
#include "src/events/notifyable.h"

#include "src/engines/kotorbase/object.h"
#include "src/engines/kotorbase/trigger.h"

namespace Engines {

class LocalPathfinding;

namespace KotORBase {

class Module;
class Room;
class Situated;
class Creature;
class Pathfinding;
struct CreatureSearchCriteria;

/** An area in Star Wars: Knights of the Old Republic, holding all objects and
 *  rooms within, as well as general area properties like the current background
 *  music and ambient sounds.
 *
 *  Note: the coordinate system of the in-world graphics is oriented as viewed
 *  from the top down. The negative Z axis goes down into the ground, while the
 *  positive Y axis points due north and the positive X axis points due east.
 */
class Area : public Object, public Events::Notifyable {
public:
	Area(Module &module, const Common::UString &resRef);
	~Area();

	// General properties

	/** Return the area's resref (resource ID). */
	const Common::UString &getResRef();

	// Minimap

	/** Get the north axis id. */
	int getNorthAxis();
	/** Get the first map point. */
	void getMapPoint1(float &x, float &y);
	/** Get the second map point. */
	void getMapPoint2(float &x, float &y);
	/** Get the first world point. */
	void getWorldPoint1(float &x, float &y);
	/** Get the second world point. */
	void getWorldPoint2(float &x, float &y);

	// Visibility

	void show();
	void hide();

	// Music/Sound

	uint32_t getMusicDayTrack   () const; ///< Return the music track ID playing by day.
	uint32_t getMusicNightTrack () const; ///< Return the music track ID playing by night.
	uint32_t getMusicBattleTrack() const; ///< Return the music track ID playing in battle.

	void setMusicDayTrack   (uint32_t track); ///< Set the music track ID playing by day.
	void setMusicNightTrack (uint32_t track); ///< Set the music track ID playing by night.
	void setMusicBattleTrack(uint32_t track); ///< Set the music track ID playing in battle.

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

	// Walkmesh

	float evaluateElevation(float x, float y);
	bool walkable(const glm::vec3 &orig, const glm::vec3 &dest) const;
	void toggleWalkmesh();
	bool rayTest(const glm::vec3 &orig, const glm::vec3 &dest, glm::vec3 &intersect) const;

	// Triggers

	void toggleTriggers();
	void evaluateTriggers(float x, float y);

	// Object management

	Object *getObjectByTag(const Common::UString &tag);
	Creature *getNearestCreature(const Object *target, int nth, const CreatureSearchCriteria &criteria) const;
	const std::vector<Creature *> &getCreatures() const;

	void addCreature(Creature *creature);
	void addToObjectMap(Object *object);
	void removeObject(Object *object);

	// Camera style

	struct CameraStyle {
		float distance { 0.0f };
		float pitch { 0.0f };
		float height { 0.0f };
		float viewAngle { 0.0f };
	};

	const CameraStyle &getCameraStyle() const;

	// Room visiblity

	const std::vector<Common::UString> &getRoomsVisibleFrom(const Common::UString &room) const;
	std::set<Common::UString> getRoomsVisibleByPartyLeader() const;

	void showAllRooms();
	void notifyObjectMoved(Object &o);
	void notifyPartyLeaderMoved();

	// Minigame

	bool isMinigame();
	const Aurora::GFF3Struct& getMinigame();


	Object *getActiveObject();

	void processCreaturesActions(float dt);
	void handleCreaturesDeath();

protected:
	void notifyCameraMoved();

private:
	typedef Common::PtrList<Room> RoomList;

	typedef Common::PtrList<Object> ObjectList;
	typedef std::map<uint32_t, Object *> ObjectMap;

	std::unique_ptr<Aurora::GFF3File> _are;

	Module *_module; ///< The module this area is in.

	Common::UString _resRef;      ///< The area's resref (resource ID).
	Common::UString _displayName; ///< The area's localized display name.

	Common::UString _ambientDay;   ///< Ambient sound that plays by day.
	Common::UString _ambientNight; ///< Ambient sound that plays by night.

	uint32_t _musicDayTrack;    ///< Music track ID that plays by day.
	uint32_t _musicNightTrack;  ///< Music track ID that plays by night.
	uint32_t _musicBattleTrack; ///< Music track ID that plays in battle.

	Common::UString _musicDay;    ///< Music that plays by day.
	Common::UString _musicNight;  ///< Music that plays by night.
	Common::UString _musicBattle; ///< Music that plays in battle.

	std::vector<Common::UString> _musicBattleStinger; ///< Battle music stingers.

	float _ambientDayVol;   ///< Day ambient sound volume.
	float _ambientNightVol; ///< Night ambient sound volume.

	bool _visible; ///< Is the area currently visible?

	float _worldPt1X, _worldPt1Y, _worldPt2X, _worldPt2Y;
	float _mapPt1X, _mapPt1Y, _mapPt2X, _mapPt2Y;
	int _northAxis;

	Sound::ChannelHandle _ambientSound; ///< Sound handle of the currently playing sound.
	Sound::ChannelHandle _ambientMusic; ///< Sound handle of the currently playing music.

	Aurora::LYTFile _lyt; ///< The area's layout description.
	Aurora::VISFile _vis; ///< The area's inter-room visibility description.

	RoomList _rooms; ///< All rooms in the area.

	ObjectList _objects;   ///< List of all objects in the area.
	ObjectMap  _objectMap; ///< Map of all non-static objects in the area.

	std::vector<Creature *> _creatures;

	Object *_activeObject; ///< The currently active (highlighted) object.

	bool _highlightAll; ///< Are we currently highlighting all objects?

	std::list<Events::Event> _eventQueue; ///< The event queue.

	std::recursive_mutex _mutex; ///< Mutex securing access to the area.

	// Triggers

	std::vector<Trigger *> _triggers;
	bool _triggersVisible;
	Trigger *_activeTrigger;


	CameraStyle _cameraStyle;
	bool _walkmeshInvisible;
	std::list<Situated *> _situatedObjects;

	Pathfinding *_pathfinding;
	Engines::LocalPathfinding *_localPathfinding;

	// Loading helpers

	void clear();
	void load();

	void loadLYT();
	void loadVIS();

	void loadARE(const Aurora::GFF3Struct &are);
	void loadGIT(const Aurora::GFF3Struct &git);

	void loadCameraStyle(uint32_t id);

	void loadRooms();

	void loadProperties(const Aurora::GFF3Struct &props);

	void loadObject(Object &object);

	void loadWaypoints (const Aurora::GFF3List &list);
	void loadPlaceables(const Aurora::GFF3List &list);
	void loadDoors     (const Aurora::GFF3List &list);
	void loadCreatures (const Aurora::GFF3List &list);
	void loadSounds    (const Aurora::GFF3List &list);
	void loadTriggers  (const Aurora::GFF3List &list);

	// Highlight / active helpers

	Object *getObjectAt(int x, int y);

	void setActive(Object *object);

	void checkActive(int x = -1, int y = -1);
	void highlightAll(bool enabled);
	void click(int x, int y);


	void updateRoomsVisiblity();
	void updatePerception(Creature &subject);


	friend class Console;
	friend class ActionExecutor;
};

} // End of namespace KotORBase

} // End of namespace Engines

#endif // ENGINES_KOTORBASE_AREA_H
