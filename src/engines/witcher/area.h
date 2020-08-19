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
 *  The context holding a The Witcher area.
 */

#ifndef ENGINES_WITCHER_AREA_H
#define ENGINES_WITCHER_AREA_H

#include <list>
#include <map>
#include <memory>

#include "src/common/types.h"
#include "src/common/ptrlist.h"
#include "src/common/ustring.h"
#include "src/common/mutex.h"

#include "src/aurora/types.h"
#include "src/aurora/locstring.h"

#include "src/graphics/aurora/types.h"

#include "src/sound/types.h"

#include "src/events/types.h"
#include "src/events/notifyable.h"

#include "src/engines/witcher/object.h"

namespace Engines {

namespace Witcher {

class Module;

/** An area in The Witcher, holding all objects and area geometry within, as
 *  well as general area properties like the current background music and
 *  ambient sounds.
 *
 *  Note: the coordinate system of the in-world graphics is oriented as viewed
 *  from the top down. The negative Z axis goes down into the ground, while the
 *  positive Y axis points due north and the positive X axis points due east.
 */
class Area : public Witcher::Object, public Events::Notifyable {
public:
	Area(Module &module, const Common::UString &resRef);
	~Area();

	// General properties

	/** Return the area's resref (resource ID). */
	const Common::UString &getResRef() const;
	/** Return the area's name. */
	const Aurora::LocString &getName() const;

	/** Refresh all localized strings. */
	void refreshLocalized();

	// Visibility

	void show(); ///< Show the area.
	void hide(); ///< Hide the area.

	// Music/Sound

	uint32_t getMusicDayTrack   () const; ///< Return the music track ID playing by day.
	uint32_t getMusicNightTrack () const; ///< Return the music track ID playing by night.
	uint32_t getMusicBattleTrack() const; ///< Return the music track ID playing in battle.

	void setMusicDayTrack   (uint32_t track); ///< Set the music track ID playing by day.
	void setMusicNightTrack (uint32_t track); ///< Set the music track ID playing by night.
	void setMusicBattleTrack(uint32_t track); ///< Set the music track ID playing in battle.

	void stopAmbientMusic(); ///< Stop the ambient music.

	/** Play the specified music (or the area's default) as ambient music. */
	void playAmbientMusic(Common::UString music = "");

	// Events

	/** Add a single event for consideration into the area event queue. */
	void addEvent(const Events::Event &event);
	/** Process the current event queue. */
	void processEventQueue();

	/** Forcibly remove the focus from the currently highlighted object. */
	void removeFocus();


	/** Return the name of an area. */
	static Aurora::LocString getName(const Common::UString &resRef);


protected:
	/** Notify the area that the camera has been moved. */
	void notifyCameraMoved();


private:
	typedef Common::PtrList<Engines::Witcher::Object> ObjectList;
	typedef std::map<uint32_t, Engines::Witcher::Object *> ObjectMap;


	Module *_module;

	Common::UString _resRef; ///< The area's resref (resource ID).

	uint32_t _musicDayTrack;    ///< Music track ID that plays by day.
	uint32_t _musicNightTrack;  ///< Music track ID that plays by night.
	uint32_t _musicBattleTrack; ///< Music track ID that plays in battle.

	Common::UString _musicDay;    ///< Music that plays by day.
	Common::UString _musicNight;  ///< Music that plays by night.
	Common::UString _musicBattle; ///< Music that plays in battle.

	bool _visible; ///< Is the area currently visible?

	Sound::ChannelHandle _ambientMusic; ///< Sound handle of the currently playing music.

	Common::UString _modelName; ///< Name of area geometry ("tile") model.
	std::unique_ptr<Graphics::Aurora::Model> _model; ///< The actual area geometry model.

	ObjectList _objects;   ///< List of all objects in the area.
	ObjectMap  _objectMap; ///< Map of all non-static objects in the area.

	/** The currently active (highlighted) object. */
	Engines::Witcher::Object *_activeObject;

	bool _highlightAll; ///< Are we currently highlighting all objects?

	std::list<Events::Event> _eventQueue; ///< The event queue.

	std::recursive_mutex _mutex; ///< Mutex securing access to the area.


	// Loading helpers

	void clear();

	void loadARE(const Aurora::GFF3Struct &are);
	void loadGIT(const Aurora::GFF3Struct &git);

	void loadProperties(const Aurora::GFF3Struct &props);

	void loadObject(Engines::Witcher::Object &object);
	void loadWaypoints (const Aurora::GFF3List &list);
	void loadPlaceables(const Aurora::GFF3List &list);
	void loadDoors     (const Aurora::GFF3List &list);

	// Model loading/unloading helpers

	void loadModels();
	void unloadModels();

	void loadAreaModel();
	void unloadAreaModel();

	// Highlight / active helpers

	void checkActive(int x = -1, int y = -1);
	void setActive(Engines::Witcher::Object *object);
	Engines::Witcher::Object *getObjectAt(int x, int y);

	void highlightAll(bool enabled);

	void click(int x, int y);
};

} // End of namespace Witcher

} // End of namespace Engines

#endif // ENGINES_WITCHER_AREA_H
