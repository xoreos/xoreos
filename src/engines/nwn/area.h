/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
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
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
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

#include "engines/nwn/scriptcontainer.h"
#include "engines/nwn/tileset.h"

namespace Engines {

namespace NWN {

class Module;

class Object;

class Area : public Aurora::NWScript::Object, public Events::Notifyable,
             public ScriptContainer {
public:
	Area(Module &module, const Common::UString &resRef);
	~Area();

	/** Return the area's resref (resource ID). */
	const Common::UString &getResRef();
	/** Return the area's localized name. */
	const Common::UString &getName();
	/** Return the area's localized display name. */
	const Common::UString &getDisplayName();

	uint32 getMusicDayTrack   () const;
	uint32 getMusicNightTrack () const;
	uint32 getMusicBattleTrack() const;

	void setMusicDayTrack   (uint32 track);
	void setMusicNightTrack (uint32 track);
	void setMusicBattleTrack(uint32 track);

	void stopSound();
	void stopAmbientMusic();
	void stopAmbientSound();

	void playAmbientMusic(Common::UString music = "");
	void playAmbientSound(Common::UString sound = "");

	void show();
	void hide();

	static Common::UString getName(const Common::UString &resRef);

	Aurora::NWScript::Object *findObject(const Common::UString &tag);
	const Aurora::NWScript::Object *findObject(const Common::UString &tag) const;

	void addEvent(const Events::Event &event);
	void processEventQueue();

	void removeFocus();



protected:
	void notifyCameraMoved();


private:
	enum Orientation {
		kOrientation0   = 0,
		kOrientation90  = 1,
		kOrientation180 = 2,
		kOrientation270 = 3
	};

	struct Tile {
		uint32 tileID;

		uint32 height;
		Orientation orientation;

		uint8 mainLight[2];
		uint8  srcLight[2];

		bool animLoop[3];

		const Tileset::Tile *tile;

		Graphics::Aurora::Model *model;
	};

	typedef std::list<Engines::NWN::Object *> ObjectList;
	typedef std::map<uint32, Engines::NWN::Object *> ObjectMap;


	Module *_module;

	bool _loaded;

	Common::UString _resRef;
	Common::UString _name;

	Common::UString _displayName;

	Common::UString _ambientDay;
	Common::UString _ambientNight;

	uint32 _musicDayTrack;
	uint32 _musicNightTrack;
	uint32 _musicBattleTrack;

	Common::UString _musicDay;
	Common::UString _musicNight;
	Common::UString _musicBattle;

	std::vector<Common::UString> _musicBattleStinger;

	float _ambientDayVol;
	float _ambientNightVol;

	bool _visible;

	Sound::ChannelHandle _ambientSound;
	Sound::ChannelHandle _ambientMusic;

	uint32 _width;
	uint32 _height;

	Common::UString _tilesetName;
	Tileset *_tileset;

	std::vector<Tile> _tiles;

	ObjectList _objects;

	ObjectMap _objectMap;

	Engines::NWN::Object *_activeObject;

	bool _highlightAll;

	std::list<Events::Event> _eventQueue;

	Common::Mutex _mutex;


	void loadARE(const Aurora::GFFStruct &are);
	void loadGIT(const Aurora::GFFStruct &git);

	void loadProperties(const Aurora::GFFStruct &props);

	void loadTiles(const Aurora::GFFList &tiles);
	void loadTile(const Aurora::GFFStruct &t, Tile &tile);

	void loadTileset();

	void initTiles();

	void loadWaypoints (const Aurora::GFFList &list);
	void loadPlaceables(const Aurora::GFFList &list);
	void loadDoors     (const Aurora::GFFList &list);
	void loadCreatures (const Aurora::GFFList &list);


	void checkActive(int x = -1, int y = -1);
	void setActive(Engines::NWN::Object *object);
	Engines::NWN::Object *getObjectAt(int x, int y);

	void highlightAll(bool enabled);

	void click(int x, int y);


	static Common::UString createDisplayName(const Common::UString &name);

	friend class Console;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_AREA_H
