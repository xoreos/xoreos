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

/** @file engines/nwn2/area.h
 *  NWN2 area.
 */

#ifndef ENGINES_NWN2_AREA_H
#define ENGINES_NWN2_AREA_H

#include <vector>

#include "common/types.h"
#include "common/ustring.h"

#include "aurora/types.h"

#include "aurora/nwscript/object.h"

#include "graphics/aurora/types.h"

#include "sound/types.h"

namespace Engines {

namespace NWN2 {

class Module;

class Object;

class Area : public Aurora::NWScript::Object {
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

	void show(); ///< Show the area.
	void hide(); ///< Hide the area.

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



	/** Return the localized name of an area. */
	static Common::UString getName(const Common::UString &resRef);


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
		uint32 metaTile; ///< The meta tile used, if any.
		uint32 tileID;   ///< ID of the tile's appearance.

		float position[3];       ///< The tile's position.
		Orientation orientation; ///< The tile's orientation.

		/** ResRef of the model. */
		Common::UString modelName;

		/** The tile's model. */
		Graphics::Aurora::Model *model;
	};

	typedef std::list<Engines::NWN2::Object *> ObjectList;
	typedef std::map<uint32, Engines::NWN2::Object *> ObjectMap;


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

	bool _hasTerrain; ///< Does area have terrain or is it tile-based?

	uint32 _width;  ///< Width  of the area in tiles, as seen from top-down.
	uint32 _height; ///< Height of the area in tiles, as seen from top-down.

	std::vector<Tile> _tiles; ///< The area's tiles.

	ObjectList _objects;   ///< List of all objects in the area.
	ObjectMap  _objectMap; ///< Map of all non-static objects in the area.


	// Loading helpers

	void clear();

	void loadARE(const Aurora::GFFStruct &are);
	void loadGIT(const Aurora::GFFStruct &git);

	void loadProperties(const Aurora::GFFStruct &props);

	void loadTiles(const Aurora::GFFList &tiles);
	void loadTile(const Aurora::GFFStruct &t, Tile &tile);

	void loadObject(Engines::NWN2::Object &object);
	void loadWaypoints  (const Aurora::GFFList &list);
	void loadPlaceables (const Aurora::GFFList &list);
	void loadEnvironment(const Aurora::GFFList &list);
	void loadDoors      (const Aurora::GFFList &list);

	// Model loading/unloading helpers

	void loadModels();
	void unloadModels();

	void loadTileModels();
	void unloadTileModels();


	static Common::UString createDisplayName(const Common::UString &name);
};

} // End of namespace NWN2

} // End of namespace Engines

#endif // ENGINES_NWN2_AREA_H
