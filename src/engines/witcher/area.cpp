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
 *  An area in The Witcher.
 */

#include "src/common/util.h"
#include "src/common/error.h"

#include "src/aurora/locstring.h"
#include "src/aurora/gfffile.h"
#include "src/aurora/2dafile.h"
#include "src/aurora/2dareg.h"

#include "src/graphics/graphics.h"

#include "src/graphics/aurora/model.h"

#include "src/sound/sound.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/model.h"

#include "src/engines/witcher/area.h"
#include "src/engines/witcher/module.h"
#include "src/engines/witcher/object.h"
#include "src/engines/witcher/waypoint.h"
#include "src/engines/witcher/placeable.h"
#include "src/engines/witcher/door.h"

namespace Engines {

namespace Witcher {

Area::Area(Module &module, const Common::UString &resRef) : _module(&module), _loaded(false),
	_resRef(resRef), _visible(false), _model(0) {

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

	// Delete area geometry model
	delete _model;
	_model = 0;
}

Common::UString Area::getName(const Common::UString &resRef) {
	try {
		Aurora::GFFFile are(resRef, Aurora::kFileTypeARE, MKTAG('A', 'R', 'E', ' '));

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
	_musicBattle      = TwoDAReg.get("ambientmusic").getRow(_musicBattleTrack).getString("Resource");
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

	_ambientMusic = playSound(music, Sound::kSoundTypeMusic, true);
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

void Area::loadARE(const Aurora::GFFStruct &are) {
	// Tag

	_tag = are.getString("Tag");

	// Name

	Aurora::LocString name;
	are.getLocString("Name", name);

	_name = name.getString();

	// Generic properties

	if (are.hasField("AreaProperties"))
		loadProperties(are.getStruct("AreaProperties"));

	// Area geometry model

	_modelName = are.getString("Tileset");
}

void Area::loadGIT(const Aurora::GFFStruct &git) {
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

void Area::loadProperties(const Aurora::GFFStruct &props) {
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

	unloadAreaModel();
}

void Area::loadAreaModel() {
	if (_modelName.empty())
		return;

	_model = loadModelObject(_modelName);
	if (!_model)
		throw Common::Exception("Can't load area geometry model \"%s\"", _modelName.c_str());
}

void Area::unloadAreaModel() {
	delete _model;
	_model = 0;
}

void Area::loadObject(Engines::Witcher::Object &object) {
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

void Area::loadDoors(const Aurora::GFFList &list) {
	for (Aurora::GFFList::const_iterator d = list.begin(); d != list.end(); ++d) {
		Door *door = new Door(*_module, **d);

		loadObject(*door);
	}
}

} // End of namespace Witcher

} // End of namespace Engines
