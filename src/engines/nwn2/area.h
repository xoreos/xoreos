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
 *  The context holding a Neverwinter Nights 2 area.
 */

#ifndef ENGINES_NWN2_AREA_H
#define ENGINES_NWN2_AREA_H

#include <vector>
#include <list>
#include <map>
#include <memory>

#include "src/common/types.h"
#include "src/common/ptrlist.h"
#include "src/common/ustring.h"
#include "src/common/mutex.h"

#include "src/aurora/types.h"

#include "src/graphics/aurora/types.h"

#include "src/sound/types.h"

#include "src/events/types.h"
#include "src/events/notifyable.h"

#include "src/engines/nwn2/object.h"

namespace Engines {

namespace NWN2 {

class TRXFile;

class Module;

/** An area in Neverwinter Nights 2, holding all objects, room tiles and terrain
 *  within, as well as general area properties like the current background music
 *  and ambient sounds.
 *
 *  Note: the coordinate system of the in-world graphics is oriented as viewed
 *  from the top down. The negative Z axis goes down into the ground, while the
 *  positive Y axis points due north and the positive X axis points due east.
 */
class Area : public NWN2::Object, public Events::Notifyable {
public:
	Area(Module &module, const Common::UString &resRef);
	~Area();

	// General properties

	/** Return the area's resref (resource ID). */
	const Common::UString &getResRef();
	/** Return the area's localized name. */
	const Common::UString &getName();
	/** Return the area's localized display name. */
	const Common::UString &getDisplayName();
	/** Back link to the area's module. */
	Module &getModule();

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


	/** Return the localized name of an area. */
	static Common::UString getName(const Common::UString &resRef);
	/** Return true if the Natural flag is set. */
	bool getIsAreaNatural() const;
	/** Return true if the Underground flag is unset. */
	bool getIsAreaAboveGround() const;
	/** Return true if the Interior flag is set. */
	bool getIsAreaInterior() const;

	// Factions

	/** Get the source's reputation with the faction. */
	uint8_t getFactionReputation(Object *source, uint32_t faction);

protected:
	/** Notify the area that the camera has been moved. */
	void notifyCameraMoved();


private:
	/** Tile orientation. */
	enum Orientation {
		kOrientation0   = 0, ///< Rotated by   0°.
		kOrientation90  = 1, ///< Rotated by  90°.
		kOrientation180 = 2, ///< Rotated by 180°.
		kOrientation270 = 3  ///< Rotated by 270°.
	};

	/** A tile. */
	struct Tile {
		bool metaTile; ///< Is this tile a meta tile?

		uint32_t tileID; ///< ID of the tile's appearance.

		float position[3];       ///< The tile's position.
		Orientation orientation; ///< The tile's orientation.

		/** ResRef of the model. */
		Common::UString modelName;

		float floorTint[3][4]; ///< Tint color of the floor parts.
		float wallTint[3][4];  ///< Tint color of the wall parts.

		/** The tile's model. */
		Graphics::Aurora::Model *model;
	};

	typedef Common::PtrList<Engines::NWN2::Object> ObjectList;
	typedef std::map<uint32_t, Engines::NWN2::Object *> ObjectMap;


	Module *_module;

	Common::UString _resRef; ///< The area's resref (resource ID).

	Common::UString _displayName; ///< The area's localized display name.

	Common::UString _ambientDay;   ///< Ambient sound that plays by day.
	Common::UString _ambientNight; ///< Ambient sound that plays by night.

	uint32_t _musicDayTrack;    ///< Music track ID that plays by day.
	uint32_t _musicNightTrack;  ///< Music track ID that plays by night.
	uint32_t _musicBattleTrack; ///< Music track ID that plays in battle.

	Common::UString _musicDay;    ///< Music that plays by day.
	Common::UString _musicNight;  ///< Music that plays by night.
	Common::UString _musicBattle; ///< Music that plays in battle.

	/** Battle music stingers. */
	std::vector<Common::UString> _musicBattleStinger;

	float _ambientDayVol;   ///< Day ambient sound volume.
	float _ambientNightVol; ///< Night ambient sound volume.

	bool _visible; ///< Is the area currently visible?

	uint32_t _flags;   ///< Natural/Underground bit flags.

	Sound::ChannelHandle _ambientSound; ///< Sound handle of the currently playing sound.
	Sound::ChannelHandle _ambientMusic; ///< Sound handle of the currently playing music.

	bool _hasTerrain; ///< Does area have terrain or is it purely tile-based?

	uint32_t _width;  ///< Width  of the area in tiles, as seen from top-down.
	uint32_t _height; ///< Height of the area in tiles, as seen from top-down.

	std::unique_ptr<TRXFile> _terrain; ///< The area's terrain.
	std::vector<Tile>          _tiles;   ///< The area's tiles.

	ObjectList _objects;   ///< List of all objects in the area.
	ObjectMap  _objectMap; ///< Map of all non-static objects in the area.

	/** The currently active (highlighted) object. */
	Engines::NWN2::Object *_activeObject;

	bool _highlightAll; ///< Are we currently highlighting all objects?

	std::list<Events::Event> _eventQueue; ///< The event queue.

	std::recursive_mutex _mutex; ///< Mutex securing access to the area.


	// Loading helpers

	void clear();

	void loadARE(const Aurora::GFF3Struct &are);
	void loadGIT(const Aurora::GFF3Struct &git);

	void loadProperties(const Aurora::GFF3Struct &props);

	void loadTerrain();
	void loadTiles(const Aurora::GFF3List &tiles);
	void loadTile(const Aurora::GFF3Struct &t, Tile &tile);

	void loadObject(Engines::NWN2::Object &object);
	void loadWaypoints  (const Aurora::GFF3List &list);
	void loadPlaceables (const Aurora::GFF3List &list);
	void loadEnvironment(const Aurora::GFF3List &list);
	void loadDoors      (const Aurora::GFF3List &list);
	void loadCreatures  (const Aurora::GFF3List &list);
	void loadStores     (const Aurora::GFF3List &list);

	// Model loading/unloading helpers

	void loadModels();
	void unloadModels();

	void loadTileModels();
	void unloadTileModels();

	// Highlight / active helpers

	void checkActive(int x = -1, int y = -1);
	void setActive(Engines::NWN2::Object *object);
	Engines::NWN2::Object *getObjectAt(int x, int y);

	void highlightAll(bool enabled);

	void click(int x, int y);
};

} // End of namespace NWN2

} // End of namespace Engines

#endif // ENGINES_NWN2_AREA_H
