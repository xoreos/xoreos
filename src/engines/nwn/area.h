/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file engines/nwn/area.h
 *  NWN area.
 */

#ifndef ENGINES_NWN_AREA_H
#define ENGINES_NWN_AREA_H

#include <vector>
#include <list>
#include <map>

#include "common/types.h"
#include "common/ustring.h"
#include "common/mutex.h"

#include "aurora/types.h"

#include "aurora/nwscript/object.h"

#include "sound/types.h"

#include "events/types.h"
#include "events/notifyable.h"

#include "engines/nwn/tileset.h"
#include "engines/nwn/model.h"

namespace Engines {

namespace NWN {

class Module;

class Area : public Aurora::NWScript::Object, public Events::Notifyable {
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

	// Visibility

	void setVisible(bool visible);

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


	/** Return the localized name of an area. */
	static Common::UString getName(const Common::UString &resRef);


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
		uint32 tileID; ///< The ID of the tile within the tileset.

		uint32 height; ///< The number of height transitions the tile is shifted up.
		Orientation orientation; ///< The orientation of the tile.

		uint8 mainLight[2]; ///< Overall colored lighting effects.
		uint8  srcLight[2]; ///< Flaming light sources.

		bool animLoop[3]; ///< Should the tile's AnimLoop0[123] play?

		const Tileset::Tile *tile; ///< The actual tile within the tileset.

		Graphics::Aurora::Model_NWN *model; ///< The tile's model.
	};


	Module *_module;

	bool _loaded;

	Common::UString _resRef; ///< The area's resref (resource ID).
	Common::UString _name;   ///< The area's localized name.

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

	uint32 _width;  ///< Width  of the area in tiles, as seen from top-down.
	uint32 _height; ///< Height of the area in tiles, as seen from top-down.

	Common::UString _tilesetName; ///< Name of the tileset.
	Tileset *_tileset; ///< The actual tileset.

	std::vector<Tile> _tiles; ///< The area's tiles.

	std::list<Events::Event> _eventQueue; ///< The event queue.

	Common::Mutex _mutex; ///< Mutex securing access to the area.


	// Loading helpers

	void loadARE(const Aurora::GFFStruct &are);
	void loadGIT(const Aurora::GFFStruct &git);

	void loadProperties(const Aurora::GFFStruct &props);

	void loadTiles(const Aurora::GFFList &tiles);
	void loadTile(const Aurora::GFFStruct &t, Tile &tile);

	// Model loading/unloading helpers

	void loadModels();
	void unloadModels();

	void loadTileModels();
	void unloadTileModels();

	void loadTileset();
	void unloadTileset();

	void loadTiles();
	void unloadTiles();


	static Common::UString createDisplayName(const Common::UString &name);
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_AREA_H
