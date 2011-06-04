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

#include "graphics/aurora/cursorman.h"
#include "graphics/aurora/model.h"

#include "sound/sound.h"

#include "engines/aurora/util.h"
#include "engines/aurora/model.h"

#include "engines/nwn/area.h"
#include "engines/nwn/module.h"
#include "engines/nwn/waypoint.h"
#include "engines/nwn/placeable.h"
#include "engines/nwn/door.h"
#include "engines/nwn/creature.h"

namespace Engines {

namespace NWN {

Area::Area(Module &module, const Common::UString &resRef) : _module(&module), _loaded(false),
	_resRef(resRef), _visible(false), _tileset(0),
	_activeObject(0), _highlightAll(false) {

	// Load ARE and GIT

	Aurora::GFFFile are(_resRef, Aurora::kFileTypeARE, MKID_BE('ARE '));
	loadARE(are.getTopLevel());

	Aurora::GFFFile git(_resRef, Aurora::kFileTypeGIT, MKID_BE('GIT '));
	loadGIT(git.getTopLevel());

	// Load the tiles

	loadTileset();
	initTiles();

	_loaded = true;

	// Tell the module that we exist
	_module->addObject(*this);
}

Area::~Area() {
	removeContainer();

	hide();

	removeFocus();

	// Delete objects
	for (ObjectList::iterator o = _objects.begin(); o != _objects.end(); ++o)
		delete *o;

	// Delete tiles and tileset
	for (std::vector<Tile>::iterator t = _tiles.begin(); t != _tiles.end(); ++t)
		delete t->model;
	delete _tileset;
}

Common::UString Area::getName(const Common::UString &resRef) {
	try {
		Aurora::GFFFile are(resRef, Aurora::kFileTypeARE, MKID_BE('ARE '));

		Aurora::LocString name;
		are.getTopLevel().getLocString("Name", name);

		return name.getString();
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

void Area::show() {
	if (_visible)
		return;

	// Play music and sound
	playAmbientSound();
	playAmbientMusic();

	GfxMan.lockFrame();

	// Show tiles
	for (std::vector<Tile>::iterator t = _tiles.begin(); t != _tiles.end(); ++t)
		t->model->show();

	// Show objects
	for (ObjectList::iterator o = _objects.begin(); o != _objects.end(); ++o)
		(*o)->show();

	GfxMan.unlockFrame();

	_visible = true;
}

void Area::hide() {
	if (!_visible)
		return;

	removeFocus();

	stopSound();

	GfxMan.lockFrame();

	// Hide objects
	for (ObjectList::iterator o = _objects.begin(); o != _objects.end(); ++o)
		(*o)->hide();

	// Hide tiles
	for (std::vector<Tile>::iterator t = _tiles.begin(); t != _tiles.end(); ++t)
		t->model->hide();

	GfxMan.unlockFrame();

	_visible = false;
}

void Area::loadARE(const Aurora::GFFStruct &are) {
	// Tag

	_tag = are.getString("Tag");

	// Name

	Aurora::LocString name;
	are.getLocString("Name", name);

	_name        = name.getString();
	_displayName = createDisplayName(_name);

	// Tiles

	_width  = are.getUint("Width");
	_height = are.getUint("Height");

	_tilesetName = are.getString("Tileset");

	_tiles.resize(_width * _height);

	loadTiles(are.getList("Tile_List"));

	// Scripts
	readScripts(are);
}

void Area::loadGIT(const Aurora::GFFStruct &git) {
	// Generic properties
	if (git.hasField("AreaProperties"))
		loadProperties(git.getStruct("AreaProperties"));

	// Waypoints
	if (git.hasField("WaypointList"))
		loadWaypoints(git.getList("WaypointList"));

	// Placeables
	if (git.hasField("Placeable List"))
		loadPlaceables(git.getList("Placeable List"));

	// Doors
	if (git.hasField("Door List"))
		loadDoors(git.getList("Door List"));

	// Creatures
	if (git.hasField("Creature List"))
		loadCreatures(git.getList("Creature List"));
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

			// A tile is 10 units wide and deep.
			// There's extra special 5x5 tiles at the edges.
			const float tileX = x * 10.0 + 5.0;
			const float tileY = y * 10.0 + 5.0;

			// The actual height of a tile is dictated by the tileset.
			const float tileZ = t.height * _tileset->getTilesHeight();

			t.model->setPosition(tileX, tileY, tileZ);
			t.model->setRotation(0.0, 0.0, -(((int) t.orientation) * 90.0));
		}
	}
}

void Area::loadWaypoints(const Aurora::GFFList &list) {
	for (Aurora::GFFList::const_iterator d = list.begin(); d != list.end(); ++d) {
		Waypoint *waypoint = new Waypoint(**d);

		waypoint->loadModel();
		waypoint->setArea(this);

		_objects.push_back(waypoint);

		if (!waypoint->isStatic()) {
			const std::list<uint32> &ids = waypoint->getIDs();

			_module->addObject(*waypoint);
			for (std::list<uint32>::const_iterator id = ids.begin(); id != ids.end(); ++id)
				_objectMap.insert(std::make_pair(*id, waypoint));
		}

	}
}

void Area::loadPlaceables(const Aurora::GFFList &list) {
	for (Aurora::GFFList::const_iterator p = list.begin(); p != list.end(); ++p) {
		Placeable *placeable = new Placeable(**p);

		placeable->loadModel();
		placeable->setArea(this);

		_objects.push_back(placeable);

		if (!placeable->isStatic()) {
			const std::list<uint32> &ids = placeable->getIDs();

			_module->addObject(*placeable);
			for (std::list<uint32>::const_iterator id = ids.begin(); id != ids.end(); ++id)
				_objectMap.insert(std::make_pair(*id, placeable));
		}

	}
}

void Area::loadDoors(const Aurora::GFFList &list) {
	for (Aurora::GFFList::const_iterator d = list.begin(); d != list.end(); ++d) {
		Door *door = new Door(**d);

		door->loadModel();
		door->setArea(this);

		_objects.push_back(door);

		if (!door->isStatic()) {
			const std::list<uint32> &ids = door->getIDs();

			_module->addObject(*door);
			for (std::list<uint32>::const_iterator id = ids.begin(); id != ids.end(); ++id)
				_objectMap.insert(std::make_pair(*id, door));
		}

	}
}

void Area::loadCreatures(const Aurora::GFFList &list) {
	for (Aurora::GFFList::const_iterator c = list.begin(); c != list.end(); ++c) {
		Creature *creature = new Creature(**c);

		creature->loadModel();
		creature->setArea(this);

		_objects.push_back(creature);

		if (!creature->isStatic()) {
			const std::list<uint32> &ids = creature->getIDs();

			_module->addObject(*creature);
			for (std::list<uint32>::const_iterator id = ids.begin(); id != ids.end(); ++id)
				_objectMap.insert(std::make_pair(*id, creature));
		}

	}
}

void Area::addEvent(const Events::Event &event) {
	_eventQueue.push_back(event);
}

void Area::processEventQueue() {
	bool hasMove = false;
	for (std::list<Events::Event>::const_iterator e = _eventQueue.begin();
	     e != _eventQueue.end(); ++e) {

		if        (e->type == Events::kEventMouseMove) { // Moving the mouse
			hasMove = true;
		} else if (e->type == Events::kEventMouseDown) { // Clicking
			if (e->button.button == SDL_BUTTON_LMASK) {
				checkActive(e->button.x, e->button.y);
				click(e->button.x, e->button.y);
			}
		} else if (e->type == Events::kEventKeyDown) { // Holding down TAB
			if (e->key.keysym.sym == SDLK_TAB)
				highlightAll(true);
		} else if (e->type == Events::kEventKeyUp) {   // Releasing TAB
			if (e->key.keysym.sym == SDLK_TAB)
				highlightAll(false);
		}
	}

	_eventQueue.clear();

	if (hasMove)
		checkActive();
}

Engines::NWN::Object *Area::getObjectAt(int x, int y) {
	const Graphics::Renderable *obj = GfxMan.getObjectAt(x, y);
	if (!obj)
		return 0;

	ObjectMap::iterator o = _objectMap.find(obj->getID());
	if (o == _objectMap.end())
		return 0;

	return o->second;
}

void Area::setActive(Engines::NWN::Object *object) {
	if (object == _activeObject)
		return;

	if (_activeObject)
		_activeObject->leave();

	_activeObject = object;

	if (_activeObject)
		_activeObject->enter();
}

void Area::checkActive(int x, int y) {
	if (!_loaded || _highlightAll)
		return;

	Common::StackLock lock(_mutex);

	if ((x < 0) || (y < 0))
		CursorMan.getPosition(x, y);

	setActive(getObjectAt(x, y));
}

void Area::click(int x, int y) {
	if (!_loaded)
		return;

	Common::StackLock lock(_mutex);

	Engines::NWN::Object *o = getObjectAt(x, y);
	if (!o)
		return;

	o->click(_module->getPC());
}

void Area::highlightAll(bool enabled) {
	if (_highlightAll == enabled)
		return;

	_highlightAll = enabled;

	for (ObjectMap::iterator o = _objectMap.begin(); o != _objectMap.end(); ++o)
		if (o->second->isClickable())
			o->second->highlight(enabled);
}

void Area::removeFocus() {
	if (_activeObject)
		_activeObject->leave();

	_activeObject = 0;
}

void Area::notifyCameraMoved() {
	checkActive();
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
