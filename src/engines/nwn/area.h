/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/area.h
 *  NWN area.
 */

#ifndef ENGINES_NWN_AREA_H
#define ENGINES_NWN_AREA_H

#include <vector>

#include "common/types.h"
#include "common/ustring.h"

#include "aurora/types.h"

#include "sound/types.h"

#include "graphics/aurora/types.h"

#include "engines/nwn/tileset.h"

namespace Engines {

namespace NWN {

class Module;
class Placeable;

class Area {
public:
	Area(Module &module, const Common::UString &resRef);
	~Area();

	/** Return the area's resref (resource ID). */
	const Common::UString &getResRef();
	/** Return the area's localized name. */
	const Common::UString &getName();
	/** Return the area's localized display name. */
	const Common::UString &getDisplayName();

	void show();
	void hide();

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

	Module *_module;

	Common::UString _resRef;
	Common::UString _name;

	Common::UString _displayName;

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

	uint32 _width;
	uint32 _height;

	Common::UString _tilesetName;
	Tileset *_tileset;

	std::vector<Tile> _tiles;
	std::vector<Placeable *> _placeables;


	void loadARE(const Aurora::GFFStruct &are);
	void loadGIT(const Aurora::GFFStruct &git);

	void loadProperties(const Aurora::GFFStruct &props);

	void loadTiles(const Aurora::GFFList &tiles);
	void loadTile(const Aurora::GFFStruct &t, Tile &tile);

	void loadTileset();

	void initTiles();

	void loadPlaceables(const Aurora::GFFList &list);

	Common::UString createDisplayName(const Common::UString &name);
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_AREA_H
