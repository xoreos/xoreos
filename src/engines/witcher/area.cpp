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
 *  The context holding a The Witcher area.
 */

#include "src/common/util.h"
#include "src/common/error.h"

#include "src/aurora/gff3file.h"
#include "src/aurora/2dafile.h"
#include "src/aurora/2dareg.h"

#include "src/graphics/graphics.h"

#include "src/graphics/aurora/model.h"
#include "src/graphics/aurora/cursorman.h"

#include "src/sound/sound.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/model.h"

#include "src/engines/witcher/area.h"
#include "src/engines/witcher/module.h"
#include "src/engines/witcher/waypoint.h"
#include "src/engines/witcher/placeable.h"
#include "src/engines/witcher/door.h"
#include "src/engines/witcher/creature.h"

namespace Engines {

namespace Witcher {

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

	// Delete area geometry model
	_model.reset();
}

Aurora::LocString Area::getName(const Common::UString &resRef) {
	try {
		Aurora::GFF3File are(resRef, Aurora::kFileTypeARE, MKTAG('A', 'R', 'E', ' '));

		Aurora::LocString name;
		are.getTopLevel().getLocString("Name", name);

		return name;

	} catch (...) {
	}

	return Aurora::LocString();
}

const Common::UString &Area::getResRef() const {
	return _resRef;
}

const Aurora::LocString &Area::getName() const {
	return Witcher::Object::getName();
}

void Area::refreshLocalized() {
	for (ObjectList::iterator o = _objects.begin(); o != _objects.end(); ++o)
		(*o)->refreshLocalized();
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
	_musicBattle      = TwoDAReg.get2DA("ambientmusic").getRow(_musicBattleTrack).getString("Resource");
}

void Area::stopAmbientMusic() {
	SoundMan.stopChannel(_ambientMusic);
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

void Area::show() {
	if (_visible)
		return;

	loadModels();

	GfxMan.lockFrame();

	// Show area geometry model
	if (_model)
		_model->show();

	// Show objects
	for (ObjectList::iterator o = _objects.begin(); o != _objects.end(); ++o)
		(*o)->show();

	GfxMan.unlockFrame();

	playAmbientMusic();

	_visible = true;
}

void Area::hide() {
	if (!_visible)
		return;

	removeFocus();

	stopAmbientMusic();

	GfxMan.lockFrame();

	// Hide objects
	for (ObjectList::iterator o = _objects.begin(); o != _objects.end(); ++o)
		(*o)->hide();

	// Hide area geometry model
	if (_model)
		_model->show();

	GfxMan.unlockFrame();

	unloadModels();

	_visible = false;
}

void Area::loadARE(const Aurora::GFF3Struct &are) {
	// Tag

	_tag = are.getString("Tag");

	// Name

	are.getLocString("Name", _name);
	refreshLocalized();

	// Generic properties

	if (are.hasField("AreaProperties"))
		loadProperties(are.getStruct("AreaProperties"));

	// Area geometry model

	_modelName = are.getString("Tileset");

	// Scripts
	readScripts(are);
}

void Area::loadGIT(const Aurora::GFF3Struct &git) {
	// Waypoints
	if (git.hasField("WaypointList"))
		loadWaypoints(git.getList("WaypointList"));

	// Placeables
	if (git.hasField("Placeable List"))
		loadPlaceables(git.getList("Placeable List"));

	// Doors
	if (git.hasField("Door List"))
		loadDoors(git.getList("Door List"));
}

void Area::loadProperties(const Aurora::GFF3Struct &props) {
	setMusicDayTrack   (props.getUint("MusicDay"   , Aurora::kStrRefInvalid));
	setMusicNightTrack (props.getUint("MusicNight" , Aurora::kStrRefInvalid));
	setMusicBattleTrack(props.getUint("MusicBattle", Aurora::kStrRefInvalid));
}

void Area::loadModels() {
	loadAreaModel();

	for (ObjectList::iterator o = _objects.begin(); o != _objects.end(); ++o) {
		Engines::Witcher::Object &object = **o;

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

	unloadAreaModel();
}

void Area::loadAreaModel() {
	if (_modelName.empty())
		return;

	_model.reset(loadModelObject(_modelName));
	if (!_model)
		throw Common::Exception("Can't load area geometry model \"%s\"", _modelName.c_str());

	_model->setPosition(1500.0f, 1500.0f, 0.0f);
}

void Area::unloadAreaModel() {
	_model.reset();
}

void Area::loadObject(Engines::Witcher::Object &object) {
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

Engines::Witcher::Object *Area::getObjectAt(int x, int y) {
	const Graphics::Renderable *obj = GfxMan.getObjectAt(x, y);
	if (!obj)
		return 0;

	ObjectMap::iterator o = _objectMap.find(obj->getID());
	if (o == _objectMap.end())
		return 0;

	return o->second;
}

void Area::setActive(Engines::Witcher::Object *object) {
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

	Engines::Witcher::Object *o = getObjectAt(x, y);
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

} // End of namespace Witcher

} // End of namespace Engines
