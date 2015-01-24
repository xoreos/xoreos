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

/** @file engines/nwn2/area.cpp
 *  NWN2 area.
 */

#include "common/util.h"
#include "common/error.h"

#include "aurora/locstring.h"
#include "aurora/gfffile.h"
#include "aurora/2dafile.h"
#include "aurora/2dareg.h"

#include "graphics/graphics.h"

#include "graphics/aurora/model.h"
#include "graphics/aurora/modelnode.h"

#include "sound/sound.h"

#include "engines/aurora/util.h"
#include "engines/aurora/model.h"

#include "engines/nwn2/area.h"
#include "engines/nwn2/trxfile.h"
#include "engines/nwn2/module.h"
#include "engines/nwn2/object.h"
#include "engines/nwn2/waypoint.h"
#include "engines/nwn2/placeable.h"
#include "engines/nwn2/door.h"

namespace Engines {

namespace NWN2 {

Area::Area(Module &module, const Common::UString &resRef) : _module(&module), _loaded(false),
	_resRef(resRef), _visible(false), _terrain(0) {

	try {
		// Load ARE and GIT

		Aurora::GFFFile are(_resRef, Aurora::kFileTypeARE, MKTAG('A', 'R', 'E', ' '));
		loadARE(are.getTopLevel());

		Aurora::GFFFile git(_resRef, Aurora::kFileTypeGIT, MKTAG('G', 'I', 'T', ' '));
		loadGIT(git.getTopLevel());

	} catch (...) {
		clear();
		throw;
	}

	_loaded = true;

	// Tell the module that we exist
	_module->addObject(*this);
}

Area::~Area() {
	removeContainer();

	hide();

	clear();
}

void Area::clear() {
	// Delete objects
	for (ObjectList::iterator o = _objects.begin(); o != _objects.end(); ++o)
		delete *o;

	_objects.clear();

	// Delete tiles
	for (std::vector<Tile>::iterator t = _tiles.begin(); t != _tiles.end(); ++t)
		delete t->model;

	_tiles.clear();

	delete _terrain;
	_terrain = 0;
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

void Area::show() {
	if (_visible)
		return;

	loadModels();

	GfxMan.lockFrame();

	// Show tiles
	for (std::vector<Tile>::iterator t = _tiles.begin(); t != _tiles.end(); ++t)
		if (t->model)
			t->model->show();

	// Show terrain
	if (_terrain)
		_terrain->show();

	// Show objects
	for (ObjectList::iterator o = _objects.begin(); o != _objects.end(); ++o)
		(*o)->show();

	GfxMan.unlockFrame();

	// Play music and sound
	playAmbientSound();
	playAmbientMusic();

	_visible = true;
}

void Area::hide() {
	if (!_visible)
		return;

	stopSound();

	GfxMan.lockFrame();

	// Hide objects
	for (ObjectList::iterator o = _objects.begin(); o != _objects.end(); ++o)
		(*o)->hide();

	// Hide terrain
	if (_terrain)
		_terrain->hide();

	// Hide tiles
	for (std::vector<Tile>::iterator t = _tiles.begin(); t != _tiles.end(); ++t)
		if (t->model)
			t->model->hide();

	GfxMan.unlockFrame();

	unloadModels();

	_visible = false;
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

	// Tiles / Terrain

	_hasTerrain = are.getBool("HasTerrain");

	_width  = are.getUint("Width");
	_height = are.getUint("Height");

	if (_hasTerrain)
		loadTerrain();

	if (are.hasField("TileList")) {
		uint32 size;
		const Aurora::GFFList &tiles = are.getList("TileList", size);

		_tiles.resize(size);
		loadTiles(tiles);
	}
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

	// Environmental objects
	if (git.hasField("EnvironmentList"))
		loadEnvironment(git.getList("EnvironmentList"));

	// Doors
	if (git.hasField("Door List"))
		loadDoors(git.getList("Door List"));
}

void Area::loadProperties(const Aurora::GFFStruct &props) {
	// Ambient sound

	const Aurora::TwoDAFile &ambientSound = TwoDAReg.get("ambientsound");

	uint32 ambientDay   = props.getUint("AmbientSndDay"  , Aurora::kStrRefInvalid);
	uint32 ambientNight = props.getUint("AmbientSndNight", Aurora::kStrRefInvalid);

	_ambientDay   = ambientSound.getRow(ambientDay  ).getString("Resource");
	_ambientNight = ambientSound.getRow(ambientNight).getString("Resource");

	uint32 ambientDayVol   = CLIP<uint32>(props.getUint("AmbientSndDayVol"  , 127), 0, 127);
	uint32 ambientNightVol = CLIP<uint32>(props.getUint("AmbientSndNitVol", 127), 0, 127);

	_ambientDayVol   = 1.25 * (1.0 - (1.0 / powf(5.0, ambientDayVol   / 127.0)));
	_ambientNightVol = 1.25 * (1.0 - (1.0 / powf(5.0, ambientNightVol / 127.0)));

	// TODO: PresetInstance0 - PresetInstance7


	// Ambient music

	setMusicDayTrack  (props.getUint("MusicDay"   , Aurora::kStrRefInvalid));
	setMusicNightTrack(props.getUint("MusicNight" , Aurora::kStrRefInvalid));

	// Battle music

	setMusicBattleTrack(props.getUint("MusicBattle", Aurora::kStrRefInvalid));
}

void Area::loadTerrain() {
	try {
		_terrain = new TRXFile(_resRef);
	} catch (Common::Exception &e) {
		Common::printException(e, "WARNING: ");
	}
}

void Area::loadTiles(const Aurora::GFFList &tiles) {
	uint32 n = 0;
	for (Aurora::GFFList::const_iterator t = tiles.begin(); t != tiles.end(); ++t, ++n)
		loadTile(**t, _tiles[n]);
}

void Area::loadTile(const Aurora::GFFStruct &t, Tile &tile) {
	// ID
	tile.metaTile = t.getUint("MetaTile");
	tile.tileID   = t.getUint("Appearance");

	const Aurora::GFFStruct &pos = t.getStruct("Position");

	// Position
	tile.position[0] = pos.getDouble("x");
	tile.position[1] = pos.getDouble("y");
	tile.position[2] = pos.getDouble("z");

	// Orientation
	tile.orientation = (Orientation) t.getUint("Orientation", 0);

	tile.model = 0;

	const Aurora::TwoDAFile &tiles = TwoDAReg.get("tiles");

	Common::UString tileSet  = tiles.getRow(tile.tileID).getString("TileSet");
	Common::UString tileType = tiles.getRow(tile.tileID).getString("Tile_Type");
	int             tileVar  = t.getUint("Variation") + 1;

	tile.modelName = Common::UString::sprintf("tl_%s_%s_%02d", tileSet.c_str(), tileType.c_str(), tileVar);
}

void Area::loadModels() {
	loadTileModels();

	for (ObjectList::iterator o = _objects.begin(); o != _objects.end(); ++o) {
		Engines::NWN2::Object &object = **o;

		object.loadModel();

		if (!object.isStatic()) {
			const std::list<uint32> &ids = object.getIDs();

			for (std::list<uint32>::const_iterator id = ids.begin(); id != ids.end(); ++id)
				_objectMap.insert(std::make_pair(*id, &object));
		}
	}
}

void Area::unloadModels() {
	_objectMap.clear();

	for (ObjectList::iterator o = _objects.begin(); o != _objects.end(); ++o)
		(*o)->unloadModel();

	unloadTileModels();
}

void Area::loadTileModels() {
	for (std::vector<Tile>::iterator t = _tiles.begin(); t != _tiles.end(); ++t) {
		if (t->modelName.empty())
			continue;

		t->model = loadModelObject(t->modelName);
			if (!t->model)
				throw Common::Exception("Can't load tile model \"%s\"", t->modelName.c_str());

		float rotation = (((int) t->orientation) * 90.0f);

		t->model->setPosition(t->position[0], t->position[1], t->position[2]);
		t->model->setRotation(0.0, 0.0, rotation);

		// Rotate static floors back
		const std::list<Graphics::Aurora::ModelNode *> &nodes = t->model->getNodes();
		for (std::list<Graphics::Aurora::ModelNode *>::const_iterator n = nodes.begin(); n != nodes.end(); ++n) {
			if (!(*n)->getName().endsWith("_F"))
				continue;

			(*n)->rotate(0.0, 0.0, -rotation);
		}
	}
}

void Area::unloadTileModels() {
	for (std::vector<Tile>::iterator t = _tiles.begin(); t != _tiles.end(); ++t) {
		delete t->model;
		t->model = 0;
	}
}

void Area::loadObject(Engines::NWN2::Object &object) {
	object.setArea(this);

	_objects.push_back(&object);
	if (!object.isStatic())
		_module->addObject(object);
}

void Area::loadWaypoints(const Aurora::GFFList &list) {
	for (Aurora::GFFList::const_iterator d = list.begin(); d != list.end(); ++d) {
		Waypoint *waypoint = new Waypoint(**d);

		loadObject(*waypoint);
	}
}

void Area::loadPlaceables(const Aurora::GFFList &list) {
	for (Aurora::GFFList::const_iterator p = list.begin(); p != list.end(); ++p) {
		Placeable *placeable = new Placeable(**p);

		loadObject(*placeable);
	}
}

void Area::loadEnvironment(const Aurora::GFFList &list) {
	for (Aurora::GFFList::const_iterator p = list.begin(); p != list.end(); ++p) {
		Placeable *placeable = new Placeable(**p);

		loadObject(*placeable);
	}
}

void Area::loadDoors(const Aurora::GFFList &list) {
	for (Aurora::GFFList::const_iterator d = list.begin(); d != list.end(); ++d) {
		Door *door = new Door(*_module, **d);

		loadObject(*door);
	}
}

// "{0011}Farlong Downstairs" -> "Farlong Downstairs"
Common::UString Area::createDisplayName(const Common::UString &name) {
	bool inBrace = false;

	Common::UString displayName;
	for (Common::UString::iterator it = name.begin(); it != name.end(); ++it) {
		if (*it == '{') {
			inBrace = true;
			continue;
		}

		if (*it == '}') {
			inBrace = false;
			continue;
		}

		if (!inBrace)
			displayName += *it;
	}

	return displayName;
}

} // End of namespace NWN2

} // End of namespace Engines
