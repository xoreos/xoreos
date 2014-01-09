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

/** @file engines/nwn/area.cpp
 *  NWN area.
 */

#include "common/util.h"
#include "common/error.h"

#include "aurora/locstring.h"
#include "aurora/gfffile.h"
#include "aurora/2dafile.h"
#include "aurora/2dareg.h"

#include "graphics/graphics.h"

#include "graphics/aurora/model_nwn.h"
#include "graphics/aurora/sceneman.h"

#include "sound/sound.h"

#include "engines/aurora/util.h"

#include "engines/nwn/area.h"
#include "engines/nwn/module.h"

namespace Engines {

namespace NWN {

Area::Area(Module &module, const Common::UString &resRef) : _module(&module), _loaded(false),
	_resRef(resRef), _visible(false), _tileset(0) {

	// Load ARE and GIT

	Aurora::GFFFile are(_resRef, Aurora::kFileTypeARE, MKTAG('A', 'R', 'E', ' '));
	loadARE(are.getTopLevel());

	Aurora::GFFFile git(_resRef, Aurora::kFileTypeGIT, MKTAG('G', 'I', 'T', ' '));
	loadGIT(git.getTopLevel());

	_loaded = true;

	// Tell the module that we exist
	_module->addObject(*this);
}

Area::~Area() {
	removeContainer();

	setVisible(false);

	// Delete tiles and tileset
	for (std::vector<Tile>::iterator t = _tiles.begin(); t != _tiles.end(); ++t)
		destroyModel(t->model);
	delete _tileset;
}

Common::UString Area::getName(const Common::UString &resRef) {
	try {
		Aurora::GFFFile are(resRef, Aurora::kFileTypeARE, MKTAG('A', 'R', 'E', ' '));

		Aurora::LocString name;
		are.getTopLevel().getLocString("Name", name);

		Common::UString str = name.getString();
		if (!str.empty() && (*--str.end() == '\n'))
			str.erase(--str.end());

		return str;

	} catch (...) {
	}

	return "";
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

uint32 Area::getMusicDayTrack() const {
	return _musicDayTrack;
}

uint32 Area::getMusicNightTrack() const {
	return _musicNightTrack;
}

uint32 Area::getMusicBattleTrack() const {
	return _musicBattleTrack;
}

void Area::setMusicDayTrack(uint32 track) {
	_musicDayTrack = track;
	_musicDay      = TwoDAReg.get("ambientmusic").getRow(track).getString("Resource");
}

void Area::setMusicNightTrack(uint32 track) {
	_musicNightTrack = track;
	_musicNight      = TwoDAReg.get("ambientmusic").getRow(track).getString("Resource");
}

void Area::setMusicBattleTrack(uint32 track) {
	_musicBattleTrack = track;

	if (_musicBattleTrack != Aurora::kStrRefInvalid) {
		const Aurora::TwoDAFile &ambientMusic = TwoDAReg.get("ambientmusic");

		// Normal battle music
		_musicBattle = ambientMusic.getRow(_musicBattleTrack).getString("Resource");

		// Battle stingers
		Common::UString stinger[3];
		stinger[0] = ambientMusic.getRow(_musicBattleTrack).getString("Stinger1");
		stinger[1] = ambientMusic.getRow(_musicBattleTrack).getString("Stinger2");
		stinger[2] = ambientMusic.getRow(_musicBattleTrack).getString("Stinger3");

		for (int i = 0; i < 3; i++)
			if (!stinger[i].empty())
				_musicBattleStinger.push_back(stinger[i]);
	}
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

	// TODO: Area::playAmbientMusic(): Day/Night
	if (music.empty())
		music = _musicDay;

	if (music.empty())
		return;

	_ambientMusic = playSound(music, Sound::kSoundTypeMusic, true);
}

void Area::playAmbientSound(Common::UString sound) {
	stopAmbientSound();

	// TODO: Area::playAmbientSound():  Day/Night
	if (sound.empty())
		sound = _ambientDay;

	if (sound.empty())
		return;

	_ambientSound = playSound(sound, Sound::kSoundTypeSFX, true, _ambientDayVol);
}

void Area::setVisible(bool visible) {
	if (_visible == visible)
		return;

	if (visible) {
		loadModels();

		{
			LOCK_FRAME();

			// Show tiles
			for (std::vector<Tile>::iterator t = _tiles.begin(); t != _tiles.end(); ++t)
				t->model->setVisible(true);
		}

		// Play music and sound
		playAmbientSound();
		playAmbientMusic();
	} else {
		stopSound();

		{
			LOCK_FRAME();

			// Hide tiles
			for (std::vector<Tile>::iterator t = _tiles.begin(); t != _tiles.end(); ++t)
				t->model->setVisible(false);
		}

		unloadModels();
	}

	_visible = visible;
}

void Area::loadARE(const Aurora::GFFStruct &are) {
	// Tag

	_tag = are.getString("Tag");

	// Name

	Aurora::LocString name;
	are.getLocString("Name", name);

	_name = name.getString();
	if (!_name.empty() && (*--_name.end() == '\n'))
		_name.erase(--_name.end());

	_displayName = createDisplayName(_name);

	// Tiles

	_width  = are.getUint("Width");
	_height = are.getUint("Height");

	_tilesetName = are.getString("Tileset");

	_tiles.resize(_width * _height);

	loadTiles(are.getList("Tile_List"));
}

void Area::loadGIT(const Aurora::GFFStruct &git) {
	// Generic properties
	if (git.hasField("AreaProperties"))
		loadProperties(git.getStruct("AreaProperties"));
}

void Area::loadProperties(const Aurora::GFFStruct &props) {
	// Ambient sound

	const Aurora::TwoDAFile &ambientSound = TwoDAReg.get("ambientsound");

	uint32 ambientDay   = props.getUint("AmbientSndDay"  , Aurora::kStrRefInvalid);
	uint32 ambientNight = props.getUint("AmbientSndNight", Aurora::kStrRefInvalid);

	_ambientDay   = ambientSound.getRow(ambientDay  ).getString("Resource");
	_ambientNight = ambientSound.getRow(ambientNight).getString("Resource");

	uint32 ambientDayVol   = CLIP<uint32>(props.getUint("AmbientSndDayVol"  , 127), 0, 127);
	uint32 ambientNightVol = CLIP<uint32>(props.getUint("AmbientSndNightVol", 127), 0, 127);

	_ambientDayVol   = 1.25 * (1.0 - (1.0 / powf(5.0, ambientDayVol   / 127.0)));
	_ambientNightVol = 1.25 * (1.0 - (1.0 / powf(5.0, ambientNightVol / 127.0)));

	// TODO: PresetInstance0 - PresetInstance7


	// Ambient music

	setMusicDayTrack  (props.getUint("MusicDay"   , Aurora::kStrRefInvalid));
	setMusicNightTrack(props.getUint("MusicNight" , Aurora::kStrRefInvalid));

	// Battle music

	setMusicBattleTrack(props.getUint("MusicBattle", Aurora::kStrRefInvalid));
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
	// ID
	tile.tileID = t.getUint("Tile_ID");

	// Height transition
	tile.height = t.getUint("Tile_Height", 0);

	// Orientation
	tile.orientation = (Orientation) t.getUint("Tile_Orientation", 0);

	// Lights

	tile.mainLight[0] = t.getUint("Tile_MainLight1", 0);
	tile.mainLight[1] = t.getUint("Tile_MainLight2", 0);

	tile.srcLight[0] = t.getUint("Tile_SrcLight1", 0);
	tile.srcLight[1] = t.getUint("Tile_SrcLight2", 0);

	// Tile animations

	tile.animLoop[0] = t.getBool("Tile_AnimLoop1", false);
	tile.animLoop[1] = t.getBool("Tile_AnimLoop2", false);
	tile.animLoop[2] = t.getBool("Tile_AnimLoop3", false);

	tile.tile  = 0;
	tile.model = 0;
}

void Area::loadModels() {
	loadTileModels();
}

void Area::unloadModels() {
	unloadTileModels();
}

void Area::loadTileModels() {
	loadTileset();
	loadTiles();
}

void Area::unloadTileModels() {
	unloadTiles();
	unloadTileset();
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

void Area::unloadTileset() {
	delete _tileset;
	_tileset = 0;
}

void Area::loadTiles() {
	for (uint32 y = 0; y < _height; y++) {
		for (uint32 x = 0; x < _width; x++) {
			uint32 n = y * _width + x;

			Tile &t = _tiles[n];

			t.tile = &_tileset->getTile(t.tileID);

			try {
				t.model = createWorldModel(t.tile->model);
			} catch (Common::Exception &e) {
				e.add("Failed to load tile model for area \"%s\" (\"%s\")", _resRef.c_str(), _name.c_str());
				throw;
			}

			// A tile is 10 units wide and deep.
			// There's extra special 5x5 tiles at the edges.
			const float tileX = x * 10.0 + 5.0;
			const float tileZ = -(y * 10.0 + 5.0);

			// The actual height of a tile is dictated by the tileset.
			const float tileY = t.height * _tileset->getTilesHeight();

			t.model->setPosition(tileX, tileY, tileZ);
			t.model->setOrientation(Common::deg2rad((((int) t.orientation) * 90.0)), 0.0, 0.0, 1.0);
		}
	}
}

void Area::unloadTiles() {
	for (uint32 y = 0; y < _height; y++) {
		for (uint32 x = 0; x < _width; x++) {
			uint32 n = y * _width + x;

			Tile &t = _tiles[n];

			t.tile = 0;

			destroyModel(t.model);
			t.model = 0;
		}
	}
}

void Area::addEvent(const Events::Event &event) {
	_eventQueue.push_back(event);
}

void Area::processEventQueue() {
	for (std::list<Events::Event>::const_iterator e = _eventQueue.begin(); e != _eventQueue.end(); ++e) {
	}

	_eventQueue.clear();
}

void Area::notifyCameraMoved() {
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
