/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/area.cpp
 *  NWN area.
 */

#include "common/endianness.h"
#include "common/util.h"
#include "common/error.h"

#include "aurora/types.h"
#include "aurora/gfffile.h"
#include "aurora/2dafile.h"
#include "aurora/2dareg.h"

#include "sound/sound.h"

#include "graphics/graphics.h"

#include "graphics/aurora/model.h"

#include "engines/aurora/util.h"
#include "engines/aurora/model.h"

#include "engines/nwn/area.h"
#include "engines/nwn/module.h"
#include "engines/nwn/placeable.h"
#include "engines/nwn/door.h"

namespace Engines {

namespace NWN {

Area::Area(Module &module, const Common::UString &resRef) :
	_module(&module), _resRef(resRef), _visible(false), _tileset(0) {

	Aurora::GFFFile are;
	loadGFF(are, _resRef, Aurora::kFileTypeARE, MKID_BE('ARE '));
	loadARE(are.getTopLevel());

	Aurora::GFFFile git;
	loadGFF(git, _resRef, Aurora::kFileTypeGIT, MKID_BE('GIT '));
	loadGIT(git.getTopLevel());

	loadTileset();

	initTiles();
}

Area::~Area() {
	hide();

	for (std::vector<Door *>::iterator d = _doors.begin(); d != _doors.end(); ++d)
		delete *d;

	for (std::vector<Placeable *>::iterator p = _placeables.begin(); p != _placeables.end(); ++p)
		delete *p;

	for (std::vector<Tile>::iterator t = _tiles.begin(); t != _tiles.end(); ++t)
		delete t->model;

	delete _tileset;
}

Common::UString Area::getName(const Common::UString &resRef) {
	Aurora::GFFFile are;
	loadGFF(are, resRef, Aurora::kFileTypeARE, MKID_BE('ARE '));

	Aurora::LocString name;
	are.getTopLevel().getLocString("Name", name);

	return name.getFirstString();
}

const Common::UString &Area::getResRef() {
	return _resRef;
}

const Common::UString &Area::getName() {
	return _name;
}

const Common::UString &Area::getDisplayName() {
	return _displayName;
}

void Area::stopSound() {
	stopAmbientMusic();
	stopAmbientSound();
}

void Area::stopAmbientMusic() {
	SoundMan.stopChannel(_ambientMusic);
}

void Area::stopAmbientSound() {
	SoundMan.stopChannel(_ambientSound);
}

void Area::playAmbientMusic(Common::UString music) {
	stopAmbientMusic();

	// TODO Day/Night
	if (music.empty())
		music = _musicDay;

	if (music.empty())
		return;

	_ambientMusic = playSound(music, Sound::kSoundTypeMusic, true);
}

void Area::playAmbientSound(Common::UString sound) {
	stopAmbientSound();

	// TODO Day/Night
	if (sound.empty())
		sound = _ambientDay;

	if (sound.empty())
		return;

	_ambientSound = playSound(sound, Sound::kSoundTypeSFX, true, _ambientDayVol);
}

void Area::show() {
	if (_visible)
		return;

	playAmbientSound();
	playAmbientMusic();

	GfxMan.lockFrame();

	// Show tiles
	for (std::vector<Tile>::iterator t = _tiles.begin(); t != _tiles.end(); ++t)
		t->model->show();

	// Show placeables
	for (std::vector<Placeable *>::iterator p = _placeables.begin(); p != _placeables.end(); ++p)
		(*p)->show();

	// Show doors
	for (std::vector<Door *>::iterator d = _doors.begin(); d != _doors.end(); ++d)
		(*d)->show();

	GfxMan.unlockFrame();

	_visible = true;
}

void Area::hide() {
	if (!_visible)
		return;

	stopSound();

	GfxMan.lockFrame();

	// Hide doors
	for (std::vector<Door *>::iterator d = _doors.begin(); d != _doors.end(); ++d)
		(*d)->hide();

	// Hide placeables
	for (std::vector<Placeable *>::iterator p = _placeables.begin(); p != _placeables.end(); ++p)
		(*p)->hide();

	// Hide tiles
	for (std::vector<Tile>::iterator t = _tiles.begin(); t != _tiles.end(); ++t)
		t->model->hide();

	GfxMan.unlockFrame();

	_visible = false;
}

void Area::loadARE(const Aurora::GFFStruct &are) {
	// Name

	Aurora::LocString name;
	are.getLocString("Name", name);

	_name        = name.getFirstString();
	_displayName = createDisplayName(_name);

	// Tiles

	_width  = are.getUint("Width");
	_height = are.getUint("Height");

	_tilesetName = are.getString("Tileset");

	_tiles.resize(_width * _height);

	loadTiles(are.getList("Tile_List"));
}

void Area::loadGIT(const Aurora::GFFStruct &git) {
	if (git.hasField("AreaProperties"))
		loadProperties(git.getStruct("AreaProperties"));

	if (git.hasField("Placeable List"))
		loadPlaceables(git.getList("Placeable List"));

	if (git.hasField("Door List"))
		loadDoors(git.getList("Door List"));
}

void Area::loadProperties(const Aurora::GFFStruct &props) {
	// Ambient sound

	const Aurora::TwoDAFile &ambientSound = TwoDAReg.get("ambientsound");

	uint32 ambientDay   = props.getUint("AmbientSndDay"  , Aurora::kStrRefInvalid);
	uint32 ambientNight = props.getUint("AmbientSndNight", Aurora::kStrRefInvalid);

	if (ambientDay   != Aurora::kStrRefInvalid)
		_ambientDay   = ambientSound.getCellString(ambientDay  , "Resource");
	if (ambientNight != Aurora::kStrRefInvalid)
		_ambientNight = ambientSound.getCellString(ambientNight, "Resource");

	uint32 ambientDayVol   = CLIP<uint32>(props.getUint("AmbientSndDayVol"  , 127), 0, 127);
	uint32 ambientNightVol = CLIP<uint32>(props.getUint("AmbientSndNightVol", 127), 0, 127);

	_ambientDayVol   = 1.25 * (1.0 - (1.0 / powf(5.0, ambientDayVol   / 127.0)));
	_ambientNightVol = 1.25 * (1.0 - (1.0 / powf(5.0, ambientNightVol / 127.0)));

	// TODO: PresetInstance0 - PresetInstance7


	// Ambient music

	const Aurora::TwoDAFile &ambientMusic = TwoDAReg.get("ambientmusic");

	uint32 musicDay   = props.getUint("MusicDay"   , Aurora::kStrRefInvalid);
	uint32 musicNight = props.getUint("MusicNight" , Aurora::kStrRefInvalid);

	if (musicDay   != Aurora::kStrRefInvalid)
		_musicDay   = ambientMusic.getCellString(musicDay  , "Resource");
	if (musicNight != Aurora::kStrRefInvalid)
		_musicNight = ambientMusic.getCellString(musicNight, "Resource");


	// Battle music

	uint32 musicBattle = props.getUint("MusicBattle", Aurora::kStrRefInvalid);

	if (musicBattle != Aurora::kStrRefInvalid) {
		_musicBattle = ambientMusic.getCellString(musicBattle, "Resource");

		// Battle stingers
		Common::UString stinger[3];
		stinger[0] = ambientMusic.getCellString(musicBattle, "Stinger1");
		stinger[1] = ambientMusic.getCellString(musicBattle, "Stinger2");
		stinger[2] = ambientMusic.getCellString(musicBattle, "Stinger3");

		for (int i = 0; i < 3; i++)
			if (!stinger[i].empty())
				_musicBattleStinger.push_back(stinger[i]);
	}
}

void Area::loadTiles(const Aurora::GFFList &tiles) {
	uint32 n = 0;
	for (Aurora::GFFList::const_iterator t = tiles.begin(); t != tiles.end(); ++t, ++n) {
		assert(n < (_width * _height));

		loadTile(**t, _tiles[n]);
	}

	assert(n == _tiles.size());
}

void Area::loadTile(const Aurora::GFFStruct &t, Tile &tile) {
	tile.tileID = t.getUint("Tile_ID");
	tile.height = t.getUint("Tile_Height", 0);

	tile.orientation = (Orientation) t.getUint("Tile_Orientation", 0);

	tile.mainLight[0] = t.getUint("Tile_MainLight1", 0);
	tile.mainLight[1] = t.getUint("Tile_MainLight2", 0);

	tile.srcLight[0] = t.getUint("Tile_SrcLight1", 0);
	tile.srcLight[1] = t.getUint("Tile_SrcLight2", 0);

	tile.animLoop[0] = t.getUint("Tile_AnimLoop1", 0) != 0;
	tile.animLoop[1] = t.getUint("Tile_AnimLoop2", 0) != 0;
	tile.animLoop[2] = t.getUint("Tile_AnimLoop3", 0) != 0;

	tile.tile  = 0;
	tile.model = 0;
}

void Area::loadTileset() {
	if (_tilesetName.empty())
		throw Common::Exception("Area \"%s\" has no tileset", _resRef.c_str());

	try {
		_tileset = new Tileset(_tilesetName);
	} catch (Common::Exception &e) {
		e.add("Failed loading tileset \"%s\"", _resRef.c_str());
		throw;
	}

	status("Loaded tileset \"%s\" (\"%s\")", _tileset->getName().c_str(), _tilesetName.c_str());
}

void Area::initTiles() {
	for (uint32 y = 0; y < _height; y++) {
		for (uint32 x = 0; x < _width; x++) {
			uint32 n = y * _width + x;

			Tile &t = _tiles[n];

			t.tile = &_tileset->getTile(t.tileID);

			t.model = loadModelObject(t.tile->model);
			if (!t.model)
				throw Common::Exception("Can't load tile model \"%s\"", t.tile->model.c_str());

			const float tileX = x * 10.0 + 5.0;
			const float tileY = y * 10.0 + 5.0;
			const float tileZ = t.height * _tileset->getTilesHeight();

			t.model->setPosition(tileX, tileY, tileZ);
			t.model->setRotation(0.0, 0.0, -(((int) t.orientation) * 90.0));
		}
	}
}

void Area::loadPlaceables(const Aurora::GFFList &list) {
	for (Aurora::GFFList::const_iterator p = list.begin(); p != list.end(); ++p) {
		Placeable *placeable = new Placeable;

		placeable->load(**p);

		_placeables.push_back(placeable);
	}
}

void Area::loadDoors(const Aurora::GFFList &list) {
	for (Aurora::GFFList::const_iterator d = list.begin(); d != list.end(); ++d) {
		Door *door = new Door;

		door->load(**d);

		_doors.push_back(door);
	}
}

// "Elfland: The Woods" -> "The Woods"
Common::UString Area::createDisplayName(const Common::UString &name) {
	for (Common::UString::iterator it = name.begin(); it != name.end(); ++it) {
		if (*it == ':') {
			if (++it == name.end())
				break;

			if (*it == ' ')
				if (++it == name.end())
					break;

			return Common::UString(it, name.end());
		}
	}

	return name;
}

} // End of namespace NWN

} // End of namespace Engines
