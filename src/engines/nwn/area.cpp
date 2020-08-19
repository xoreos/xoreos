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

#include <cassert>

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/maths.h"

#include "src/aurora/gff3file.h"
#include "src/aurora/2dafile.h"
#include "src/aurora/2dareg.h"

#include "src/graphics/graphics.h"

#include "src/graphics/aurora/cursorman.h"
#include "src/graphics/aurora/model.h"

#include "src/sound/sound.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/model.h"
#include "src/engines/aurora/localpathfinding.h"

#include "src/engines/nwn/area.h"
#include "src/engines/nwn/module.h"
#include "src/engines/nwn/waypoint.h"
#include "src/engines/nwn/pathfinding.h"
#include "src/engines/nwn/placeable.h"
#include "src/engines/nwn/door.h"
#include "src/engines/nwn/creature.h"

namespace Engines {

namespace NWN {

Area::Area(Module &module, const Common::UString &resRef) : Object(kObjectTypeArea),
	_module(&module), _resRef(resRef), _visible(false),
	_activeObject(0), _highlightAll(false), _walkmeshInvisible(true) {

	_pathfinding = new Pathfinding(_module->getWalkableSurfaces());
	_pathfinding->showWalkmesh(!_walkmeshInvisible);
	_localPathfinding = new Engines::LocalPathfinding(_pathfinding);
	_localPathfinding->showWalkmesh(!_walkmeshInvisible);

	try {
		load();
	} catch (...) {
		clear();
		throw;
	}

	// Tell the module that we exist
	_module->addObject(*this);
}

Area::~Area() {
	_module->removeObject(*this);

	hide();

	removeFocus();

	clear();
}

void Area::load() {
	Aurora::GFF3File are(_resRef, Aurora::kFileTypeARE, MKTAG('A', 'R', 'E', ' '), true);
	loadARE(are.getTopLevel());

	Aurora::GFF3File git(_resRef, Aurora::kFileTypeGIT, MKTAG('G', 'I', 'T', ' '), true);
	loadGIT(git.getTopLevel());
}

void Area::clear() {
	delete _localPathfinding;
	delete _pathfinding;

	// Delete objects
	for (ObjectList::iterator o = _objects.begin(); o != _objects.end(); ++o)
		_module->removeObject(**o);

	_objects.clear();

	// Delete tiles and tileset
	for (std::vector<Tile>::iterator t = _tiles.begin(); t != _tiles.end(); ++t)
		delete t->model;

	_tiles.clear();

	_tileset.reset();
}

Common::UString Area::getName(const Common::UString &resRef) {
	try {
		Aurora::GFF3File are(resRef, Aurora::kFileTypeARE, MKTAG('A', 'R', 'E', ' '), true);

		Common::UString name = are.getTopLevel().getString("Name");
		if (!name.empty() && (*--name.end() == '\n'))
			name.erase(--name.end());

		return name;

	} catch (...) {
	}

	return "";
}

const Common::UString &Area::getResRef() {
	return _resRef;
}

const Common::UString &Area::getName() {
	return NWN::Object::getName();
}

const Common::UString &Area::getDisplayName() {
	return _displayName;
}

const Common::UString &Area::getEnvironmentMap() const {
	static const Common::UString kEmptyString;

	return _tileset ? _tileset->getEnvironmentMap() : kEmptyString;
}

uint32_t Area::getMusicDayTrack() const {
	return _musicDayTrack;
}

uint32_t Area::getMusicNightTrack() const {
	return _musicNightTrack;
}

uint32_t Area::getMusicBattleTrack() const {
	return _musicBattleTrack;
}

void Area::setMusicDayTrack(uint32_t track) {
	_musicDayTrack = track;
	_musicDay      = TwoDAReg.get2DA("ambientmusic").getRow(track).getString("Resource");
}

void Area::setMusicNightTrack(uint32_t track) {
	_musicNightTrack = track;
	_musicNight      = TwoDAReg.get2DA("ambientmusic").getRow(track).getString("Resource");
}

void Area::setMusicBattleTrack(uint32_t track) {
	_musicBattleTrack = track;

	if (_musicBattleTrack != Aurora::kStrRefInvalid) {
		const Aurora::TwoDAFile &ambientMusic = TwoDAReg.get2DA("ambientmusic");

		// Normal battle music
		_musicBattle = ambientMusic.getRow(_musicBattleTrack).getString("Resource");

		// Battle stingers
		Common::UString stinger[3];
		stinger[0] = ambientMusic.getRow(_musicBattleTrack).getString("Stinger1");
		stinger[1] = ambientMusic.getRow(_musicBattleTrack).getString("Stinger2");
		stinger[2] = ambientMusic.getRow(_musicBattleTrack).getString("Stinger3");

		_musicBattleStinger.clear();
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

	_ambientMusic = ::Engines::playSound(music, Sound::kSoundTypeMusic, true);
}

void Area::playAmbientSound(Common::UString sound) {
	stopAmbientSound();

	// TODO: Area::playAmbientSound():  Day/Night
	if (sound.empty())
		sound = _ambientDay;

	if (sound.empty())
		return;

	_ambientSound = ::Engines::playSound(sound, Sound::kSoundTypeSFX, true, _ambientDayVol);
}

void Area::show() {
	if (_visible)
		return;

	loadModels();

	GfxMan.lockFrame();

	// Show tiles
	for (std::vector<Tile>::iterator t = _tiles.begin(); t != _tiles.end(); ++t)
		t->model->show();

	// Show objects
	for (ObjectList::iterator o = _objects.begin(); o != _objects.end(); ++o)
		(*o)->show();

	// Show walkmesh
	_pathfinding->showWalkmesh(!_walkmeshInvisible);
	_localPathfinding->showWalkmesh(!_walkmeshInvisible);

	GfxMan.unlockFrame();

	// Play music and sound
	playAmbientSound();
	playAmbientMusic();

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

	// Hide walkmesh
	_pathfinding->showWalkmesh(false);
	_localPathfinding->showWalkmesh(false);

	GfxMan.unlockFrame();

	unloadModels();

	_visible = false;
}

void Area::loadARE(const Aurora::GFF3Struct &are) {
	// Tag

	_tag = are.getString("Tag");

	// Name

	_name = are.getString("Name");
	if (!_name.empty() && (*--_name.end() == '\n'))
		_name.erase(--_name.end());

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

void Area::loadGIT(const Aurora::GFF3Struct &git) {
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

void Area::loadProperties(const Aurora::GFF3Struct &props) {
	// Ambient sound

	const Aurora::TwoDAFile &ambientSound = TwoDAReg.get2DA("ambientsound");

	uint32_t ambientDay   = props.getUint("AmbientSndDay"  , Aurora::kStrRefInvalid);
	uint32_t ambientNight = props.getUint("AmbientSndNight", Aurora::kStrRefInvalid);

	_ambientDay   = ambientSound.getRow(ambientDay  ).getString("Resource");
	_ambientNight = ambientSound.getRow(ambientNight).getString("Resource");

	uint32_t ambientDayVol   = CLIP<uint32_t>(props.getUint("AmbientSndDayVol"  , 127), 0, 127);
	uint32_t ambientNightVol = CLIP<uint32_t>(props.getUint("AmbientSndNightVol", 127), 0, 127);

	_ambientDayVol   = 1.25f * (1.0f - (1.0f / powf(5.0f, ambientDayVol   / 127.0f)));
	_ambientNightVol = 1.25f * (1.0f - (1.0f / powf(5.0f, ambientNightVol / 127.0f)));

	// TODO: PresetInstance0 - PresetInstance7


	// Ambient music

	setMusicDayTrack  (props.getUint("MusicDay"   , Aurora::kStrRefInvalid));
	setMusicNightTrack(props.getUint("MusicNight" , Aurora::kStrRefInvalid));

	// Battle music

	setMusicBattleTrack(props.getUint("MusicBattle", Aurora::kStrRefInvalid));
}

void Area::loadTiles(const Aurora::GFF3List &tiles) {
	size_t n = 0;
	for (Aurora::GFF3List::const_iterator t = tiles.begin(); t != tiles.end(); ++t, ++n) {
		assert(n < (_width * _height));

		loadTile(**t, _tiles[n]);
	}

	assert(n == _tiles.size());
}

void Area::loadTile(const Aurora::GFF3Struct &t, Tile &tile) {
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

	for (ObjectList::iterator o = _objects.begin(); o != _objects.end(); ++o) {
		NWN::Object &object = **o;

		object.loadModel();

		if (!object.isStatic()) {
			const std::list<uint32_t> &ids = object.getIDs();

			for (std::list<uint32_t>::const_iterator id = ids.begin(); id != ids.end(); ++id)
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
		_tileset = std::make_unique<Tileset>(_tilesetName);
	} catch (Common::Exception &e) {
		e.add("Failed loading tileset \"%s\"", _resRef.c_str());
		throw;
	}

	status("Loaded tileset \"%s\" (\"%s\")", _tileset->getName().c_str(), _tilesetName.c_str());
}

void Area::unloadTileset() {
	_tileset.reset();
}

void Area::loadTiles() {
	for (uint32_t y = 0; y < _height; y++) {
		for (uint32_t x = 0; x < _width; x++) {
			uint32_t n = y * _width + x;

			Tile &t = _tiles[n];

			t.tile = &_tileset->getTile(t.tileID);

			t.model = loadModelObject(t.tile->model);
			if (!t.model)
				throw Common::Exception("Can't load tile model \"%s\"", t.tile->model.c_str());

			// A tile is 10 units wide and deep.
			// There's extra special 5x5 tiles at the edges.
			const float tileX = x * 10.0f + 5.0f;
			const float tileY = y * 10.0f + 5.0f;

			// The actual height of a tile is dictated by the tileset.
			const float tileZ = t.height * _tileset->getTilesHeight();

			t.model->setPosition(tileX, tileY, tileZ);
			t.model->setOrientation(0.0f, 0.0f, 1.0f, ((int) t.orientation) * 90.0f);

			if (!_pathfinding->loaded()) {
				float position[3] = { tileX, tileY, tileZ };
				float orientation[4] = {0.0f, 0.0f, 1.0f, ((int) t.orientation) * (float) M_PI * 0.5f};
				_pathfinding->addTile(t.tile->model, orientation, position);
			}
		}
	}

	if (!_pathfinding->loaded())
		_pathfinding->finalize();
}

void Area::unloadTiles() {
	for (uint32_t y = 0; y < _height; y++) {
		for (uint32_t x = 0; x < _width; x++) {
			uint32_t n = y * _width + x;

			Tile &t = _tiles[n];

			t.tile = 0;

			delete t.model;
			t.model = 0;
		}
	}
}

void Area::loadObject(NWN::Object &object) {
	object.setArea(this);

	_objects.push_back(&object);
	_module->addObject(object);
}

void Area::loadWaypoints(const Aurora::GFF3List &list) {
	for (Aurora::GFF3List::const_iterator d = list.begin(); d != list.end(); ++d) {
		Waypoint *waypoint = new Waypoint(**d);

		loadObject(*waypoint);
	}
}

void Area::loadPlaceables(const Aurora::GFF3List &list) {
	for (Aurora::GFF3List::const_iterator p = list.begin(); p != list.end(); ++p) {
		Placeable *placeable = new Placeable(**p);

		loadObject(*placeable);
	}
}

void Area::loadDoors(const Aurora::GFF3List &list) {
	for (Aurora::GFF3List::const_iterator d = list.begin(); d != list.end(); ++d) {
		Door *door = new Door(*_module, **d);

		loadObject(*door);
	}
}

void Area::loadCreatures(const Aurora::GFF3List &list) {
	for (Aurora::GFF3List::const_iterator c = list.begin(); c != list.end(); ++c) {
		Creature *creature = new Creature(**c);

		loadObject(*creature);
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

				if (_activeObject)
					continue;
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

NWN::Object *Area::getObjectAt(int x, int y) {
	const Graphics::Renderable *obj = GfxMan.getObjectAt(x, y);
	if (!obj)
		return 0;

	ObjectMap::iterator o = _objectMap.find(obj->getID());
	if (o == _objectMap.end())
		return 0;

	return o->second;
}

void Area::setActive(NWN::Object *object) {
	if (object == _activeObject)
		return;

	if (_activeObject)
		_activeObject->leave();

	_activeObject = object;

	if (_activeObject)
		_activeObject->enter();
}

void Area::checkActive(int x, int y) {
	if (_highlightAll)
		return;

	std::lock_guard<std::recursive_mutex> lock(_mutex);

	if ((x < 0) || (y < 0))
		CursorMan.getPosition(x, y);

	setActive(getObjectAt(x, y));
}

void Area::click(int x, int y) {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	NWN::Object *o = getObjectAt(x, y);
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

void Area::toggleWalkmesh() {
	_walkmeshInvisible = !_walkmeshInvisible;
	_pathfinding->showWalkmesh(!_walkmeshInvisible);
	_localPathfinding->showWalkmesh(!_walkmeshInvisible);
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
