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
 *  The context holding a Neverwinter Nights area.
 */

#ifndef ENGINES_NWN_AREA_H
#define ENGINES_NWN_AREA_H

#include <vector>
#include <list>
#include <map>

#include "src/common/types.h"
#include "src/common/ptrlist.h"
#include "src/common/ustring.h"
#include "src/common/mutex.h"

#include "src/aurora/types.h"

#include "src/graphics/aurora/types.h"

#include "src/sound/types.h"

#include "src/events/types.h"
#include "src/events/notifyable.h"

#include "src/engines/nwn/tileset.h"
#include "src/engines/nwn/object.h"

namespace Engines {

class LocalPathfinding;

namespace NWN {

class Module;
class Pathfinding;

/** An area in Neverwinter Nights, holding all objects and room tiles within, as
 *  well as general area properties like the current background music and
 *  ambient sounds.
 *
 *  Note: the coordinate system of the in-world graphics is oriented as viewed
 *  from the top down. The negative Z axis goes down into the ground, while the
 *  positive Y axis points due north and the positive X axis points due east.
 */
class Area : public NWN::Object, public Events::Notifyable {
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

	/** Return the area's environment map. */
	const Common::UString &getEnvironmentMap() const;

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

	// Walkmesh
	void toggleWalkmesh();


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
		uint32_t tileID; ///< The ID of the tile within the tileset.

		uint32_t height; ///< The number of height transitions the tile is shifted up.
		Orientation orientation; ///< The orientation of the tile.

		uint8_t mainLight[2]; ///< Overall colored lighting effects.
		uint8_t  srcLight[2]; ///< Flaming light sources.

		bool animLoop[3]; ///< Should the tile's AnimLoop0[123] play?

		const Tileset::Tile *tile; ///< The actual tile within the tileset.

		Graphics::Aurora::Model *model; ///< The tile's model.
	};

	typedef Common::PtrList<NWN::Object> ObjectList;
	typedef std::map<uint32_t, NWN::Object *> ObjectMap;


	Module *_module; ///< The module this area is in.

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

	Sound::ChannelHandle _ambientSound; ///< Sound handle of the currently playing sound.
	Sound::ChannelHandle _ambientMusic; ///< Sound handle of the currently playing music.

	uint32_t _width;  ///< Width  of the area in tiles, as seen from top-down.
	uint32_t _height; ///< Height of the area in tiles, as seen from top-down.

	Common::UString _tilesetName; ///< Name of the tileset.
	std::unique_ptr<Tileset> _tileset; ///< The actual tileset.

	std::vector<Tile> _tiles; ///< The area's tiles.

	ObjectList _objects;   ///< List of all objects in the area.
	ObjectMap  _objectMap; ///< Map of all non-static objects in the area.

	/** The currently active (highlighted) object. */
	NWN::Object *_activeObject;

	bool _highlightAll; ///< Are we currently highlighting all objects?

	std::list<Events::Event> _eventQueue; ///< The event queue.

	std::recursive_mutex _mutex; ///< Mutex securing access to the area.

	bool _walkmeshInvisible;
	Pathfinding *_pathfinding;
	Engines::LocalPathfinding *_localPathfinding;

	// Loading helpers

	void clear();
	void load();

	void loadARE(const Aurora::GFF3Struct &are);
	void loadGIT(const Aurora::GFF3Struct &git);

	void loadProperties(const Aurora::GFF3Struct &props);

	void loadTiles(const Aurora::GFF3List &tiles);
	void loadTile(const Aurora::GFF3Struct &t, Tile &tile);

	void loadObject(NWN::Object &object);
	void loadWaypoints (const Aurora::GFF3List &list);
	void loadPlaceables(const Aurora::GFF3List &list);
	void loadDoors     (const Aurora::GFF3List &list);
	void loadCreatures (const Aurora::GFF3List &list);

	// Model loading/unloading helpers

	void loadModels();
	void unloadModels();

	void loadTileModels();
	void unloadTileModels();

	void loadTileset();
	void unloadTileset();

	void loadTiles();
	void unloadTiles();

	// Highlight / active helpers

	void checkActive(int x = -1, int y = -1);
	void setActive(NWN::Object *object);
	NWN::Object *getObjectAt(int x, int y);

	void highlightAll(bool enabled);

	void click(int x, int y);


	static Common::UString createDisplayName(const Common::UString &name);

	friend class Console;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_AREA_H
