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
 *  The context holding a Neverwinter Nights 2 area.
 */

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/configman.h"

#include "src/aurora/gff3file.h"
#include "src/aurora/2dafile.h"
#include "src/aurora/2dareg.h"

#include "src/graphics/graphics.h"

#include "src/graphics/aurora/model.h"
#include "src/graphics/aurora/model_nwn2.h"
#include "src/graphics/aurora/modelnode.h"
#include "src/graphics/aurora/cursorman.h"

#include "src/sound/sound.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/model.h"

#include "src/engines/nwn2/area.h"
#include "src/engines/nwn2/util.h"
#include "src/engines/nwn2/trxfile.h"
#include "src/engines/nwn2/module.h"
#include "src/engines/nwn2/waypoint.h"
#include "src/engines/nwn2/placeable.h"
#include "src/engines/nwn2/door.h"
#include "src/engines/nwn2/creature.h"
#include "src/engines/nwn2/faction.h"
#include "src/engines/nwn2/store.h"

namespace Engines {

namespace NWN2 {

Area::Area(Module &module, const Common::UString &resRef) : Object(kObjectTypeArea),
	_module(&module), _resRef(resRef), _visible(false),
	_activeObject(0), _highlightAll(false) {

	try {
		// Load ARE and GIT

		Aurora::GFF3File are(_resRef, Aurora::kFileTypeARE, MKTAG('A', 'R', 'E', ' '));
		loadARE(are.getTopLevel());

		Aurora::GFF3File git(_resRef, Aurora::kFileTypeGIT, MKTAG('G', 'I', 'T', ' '));
		loadGIT(git.getTopLevel());

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

void Area::clear() {
	// Delete objects
	for (ObjectList::iterator o = _objects.begin(); o != _objects.end(); ++o)
		_module->removeObject(**o);

	_objects.clear();

	// Delete tiles
	for (std::vector<Tile>::iterator t = _tiles.begin(); t != _tiles.end(); ++t)
		delete t->model;

	_tiles.clear();

	_terrain.reset();
}

Common::UString Area::getName(const Common::UString &resRef) {
	try {
		Aurora::GFF3File are(resRef, Aurora::kFileTypeARE, MKTAG('A', 'R', 'E', ' '));

		Common::UString name = are.getTopLevel().getString("Name");
		if (!name.empty() && (*--name.end() == '\n'))
			name.erase(--name.end());

		return name;

	} catch (...) {
	}

	return "";
}

bool Area::getIsAreaNatural() const {
	// Bit flag for a natural area
	return (_flags & (1 << 2));
}

bool Area::getIsAreaAboveGround() const {
	// Bit flag for an underground area
	return !(_flags & (1 << 1));
}

bool Area::getIsAreaInterior() const {
	// Bit flag for an interior area
	return (_flags & 1);
}

uint8_t Area::getFactionReputation(Object *source, uint32_t faction) {
	return _module->getFactions().getReputation(source, faction);
}

const Common::UString &Area::getResRef() {
	return _resRef;
}

const Common::UString &Area::getName() {
	return NWN2::Object::getName();
}

const Common::UString &Area::getDisplayName() {
	return _displayName;
}

Module &Area::getModule() {
	return *_module;
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

	removeFocus();

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

void Area::loadARE(const Aurora::GFF3Struct &are) {
	// Tag

	_tag = are.getString("Tag");

	// Name

	_name = are.getString("Name");
	if (!_name.empty() && (*--_name.end() == '\n'))
		_name.erase(--_name.end());

	_displayName = createDisplayName(_name);

	// Tiles / Terrain

	_hasTerrain = are.getBool("HasTerrain");

	_width  = are.getUint("Width");
	_height = are.getUint("Height");

	_flags = are.getUint("Flags");

	if (_hasTerrain)
		loadTerrain();

	if (are.hasField("TileList")) {
		const Aurora::GFF3List &tiles = are.getList("TileList");
		_tiles.resize(tiles.size());

		loadTiles(tiles);
	}

	// Scripts and variables
	readScripts(are);
	readVarTable(are);
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

	// Environmental objects
	if (git.hasField("EnvironmentList"))
		loadEnvironment(git.getList("EnvironmentList"));

	// Doors
	if (git.hasField("Door List"))
		loadDoors(git.getList("Door List"));

	// Creatures
	if (git.hasField("Creature List"))
		loadCreatures(git.getList("Creature List"));

	// Stores
	if (git.hasField("StoreList"))
		loadStores(git.getList("StoreList"));
}

void Area::loadProperties(const Aurora::GFF3Struct &props) {
	// Ambient sound

	const Aurora::TwoDAFile &ambientSound = TwoDAReg.get2DA("ambientsound");

	uint32_t ambientDay   = props.getUint("AmbientSndDay"  , Aurora::kStrRefInvalid);
	uint32_t ambientNight = props.getUint("AmbientSndNight", Aurora::kStrRefInvalid);

	_ambientDay   = ambientSound.getRow(ambientDay  ).getString("Resource");
	_ambientNight = ambientSound.getRow(ambientNight).getString("Resource");

	uint32_t ambientDayVol   = CLIP<uint32_t>(props.getUint("AmbientSndDayVol"  , 127), 0, 127);
	uint32_t ambientNightVol = CLIP<uint32_t>(props.getUint("AmbientSndNitVol", 127), 0, 127);

	_ambientDayVol   = 1.25f * (1.0f - (1.0f / powf(5.0f, ambientDayVol   / 127.0f)));
	_ambientNightVol = 1.25f * (1.0f - (1.0f / powf(5.0f, ambientNightVol / 127.0f)));

	// TODO: PresetInstance0 - PresetInstance7


	// Ambient music

	setMusicDayTrack  (props.getUint("MusicDay"   , Aurora::kStrRefInvalid));
	setMusicNightTrack(props.getUint("MusicNight" , Aurora::kStrRefInvalid));

	// Battle music

	setMusicBattleTrack(props.getUint("MusicBattle", Aurora::kStrRefInvalid));
}

void Area::loadTerrain() {
	try {
		_terrain = std::make_unique<TRXFile>(_resRef);
	} catch (...) {
		Common::exceptionDispatcherWarning();
	}
}

void Area::loadTiles(const Aurora::GFF3List &tiles) {
	uint32_t n = 0;
	for (Aurora::GFF3List::const_iterator t = tiles.begin(); t != tiles.end(); ++t, ++n)
		loadTile(**t, _tiles[n]);
}

void Area::loadTile(const Aurora::GFF3Struct &t, Tile &tile) {
	// ID
	tile.metaTile = t.getUint("MetaTile") == 1;
	tile.tileID   = t.getUint("Appearance");

	const Aurora::GFF3Struct &pos = t.getStruct("Position");

	// Position
	tile.position[0] = pos.getDouble("x");
	tile.position[1] = pos.getDouble("y");
	tile.position[2] = pos.getDouble("z");

	// Orientation
	tile.orientation = (Orientation) t.getUint("Orientation", 0);

	// Tint
	readTint(t, "FloorTint", tile.floorTint);
	readTint(t, "WallTint" , tile.wallTint);

	tile.model = 0;


	if (!tile.metaTile) {
		// Normal tile

		const Aurora::TwoDAFile &tiles = TwoDAReg.get2DA("tiles");

		Common::UString tileSet  = tiles.getRow(tile.tileID).getString("TileSet");
		Common::UString tileType = tiles.getRow(tile.tileID).getString("Tile_Type");
		int             tileVar  = t.getUint("Variation") + 1;

		tile.modelName = Common::UString::format("tl_%s_%s_%02d", tileSet.c_str(), tileType.c_str(), tileVar);
	} else {
		// "Meta tile". Spreads over the space of several normal tiles

		const Aurora::TwoDAFile &metatiles = TwoDAReg.get2DA("metatiles");

		Common::UString tileSet = metatiles.getRow(tile.tileID).getString("TileSet");
		Common::UString name    = metatiles.getRow(tile.tileID).getString("Name");

		tile.modelName = Common::UString::format("tl_%s_%s", tileSet.c_str(), name.c_str());
	}
}

void Area::loadModels() {
	loadTileModels();

	for (ObjectList::iterator o = _objects.begin(); o != _objects.end(); ++o) {
		Engines::NWN2::Object &object = **o;

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
	for (std::vector<Tile>::iterator t = _tiles.begin(); t != _tiles.end(); ++t) {
		if (t->modelName.empty())
			continue;

		t->model = loadModelObject(t->modelName);
			if (!t->model)
				throw Common::Exception("Can't load tile model \"%s\"", t->modelName.c_str());

		// Tinting
		if (ConfigMan.getBool("tint")) {
			dynamic_cast<Graphics::Aurora::Model_NWN2 &>(*t->model).setTintFloor(t->floorTint);
			dynamic_cast<Graphics::Aurora::Model_NWN2 &>(*t->model).setTintWalls(t->wallTint);
		}

		const float rotation = -(((int) t->orientation) * 90.0f);

		t->model->setPosition(t->position[0], t->position[1], t->position[2]);
		t->model->setOrientation(0.0f, 0.0f, 1.0f, rotation);

		// Rotate static floors back
		const std::vector<Graphics::Aurora::ModelNode *> &nodes = t->model->getNodes();
		for (std::vector<Graphics::Aurora::ModelNode *>::const_iterator n = nodes.begin(); n != nodes.end(); ++n) {
			if (t->metaTile || !(*n)->getName().endsWith("_F"))
				continue;

			(*n)->rotate(0.0f, 0.0f, -rotation);
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

void Area::loadEnvironment(const Aurora::GFF3List &list) {
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

void Area::loadStores(const Aurora::GFF3List &list) {
	for (Aurora::GFF3List::const_iterator s = list.begin(); s != list.end(); ++s) {
		Store *store = new Store(**s);

		loadObject(*store);
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

Engines::NWN2::Object *Area::getObjectAt(int x, int y) {
	const Graphics::Renderable *obj = GfxMan.getObjectAt(x, y);
	if (!obj)
		return 0;

	ObjectMap::iterator o = _objectMap.find(obj->getID());
	if (o == _objectMap.end())
		return 0;

	return o->second;
}

void Area::setActive(Engines::NWN2::Object *object) {
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

	Engines::NWN2::Object *o = getObjectAt(x, y);
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

} // End of namespace NWN2

} // End of namespace Engines
