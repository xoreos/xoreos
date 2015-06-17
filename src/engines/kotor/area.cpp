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
 *  An area.
 */

#include <cassert>

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/readstream.h"

#include "src/aurora/resman.h"
#include "src/aurora/locstring.h"
#include "src/aurora/gff3file.h"
#include "src/aurora/2dafile.h"
#include "src/aurora/2dareg.h"

#include "src/graphics/graphics.h"
#include "src/graphics/renderable.h"

#include "src/graphics/aurora/cursorman.h"

#include "src/sound/sound.h"

#include "src/engines/aurora/util.h"

#include "src/engines/kotor/area.h"
#include "src/engines/kotor/room.h"
#include "src/engines/kotor/placeable.h"
#include "src/engines/kotor/door.h"
#include "src/engines/kotor/creature.h"

namespace Engines {

namespace KotOR {

Area::Area() : _loaded(false), _visible(false), _activeObject(0), _highlightAll(false) {
}

Area::~Area() {
	unload();
}

const Common::UString &Area::getName() {
	return _name;
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
	assert(_loaded);

	if (_visible)
		return;

	playAmbientSound();
	playAmbientMusic();

	GfxMan.lockFrame();

	// Show rooms
	for (RoomList::iterator r = _rooms.begin(); r != _rooms.end(); ++r)
		(*r)->show();

	// Show objects
	for (ObjectList::iterator o = _objects.begin(); o != _objects.end(); ++o)
		(*o)->show();

	GfxMan.unlockFrame();

	_visible = true;
}

void Area::hide() {
	assert(_loaded);

	if (!_visible)
		return;

	removeFocus();

	stopSound();

	GfxMan.lockFrame();

	// Hide objects
	for (ObjectList::iterator o = _objects.begin(); o != _objects.end(); ++o)
		(*o)->hide();

	// Hide rooms
	for (RoomList::iterator r = _rooms.begin(); r != _rooms.end(); ++r)
		(*r)->hide();

	GfxMan.unlockFrame();

	_visible = false;
}

void Area::load(const Common::UString &resRef) {
	_resRef = resRef;

	loadLYT(); // Room layout
	loadVIS(); // Room visibilities

	loadRooms();

	Aurora::GFF3File are(_resRef, Aurora::kFileTypeARE, MKTAG('A', 'R', 'E', ' '));
	loadARE(are.getTopLevel());

	Aurora::GFF3File git(_resRef, Aurora::kFileTypeGIT, MKTAG('G', 'I', 'T', ' '));
	loadGIT(git.getTopLevel());

	_loaded = true;
}

void Area::loadLYT() {
	Common::SeekableReadStream *lyt = 0;
	try {
		if (!(lyt = ResMan.getResource(_resRef, Aurora::kFileTypeLYT)))
			throw Common::Exception("No such LYT");

		_lyt.load(*lyt);

		delete lyt;
	} catch (Common::Exception &e) {
		delete lyt;
		e.add("Failed loading LYT \"%s\"", _resRef.c_str());
		throw;
	}
}

void Area::loadVIS() {
	Common::SeekableReadStream *vis = 0;
	try {
		if (!(vis = ResMan.getResource(_resRef, Aurora::kFileTypeVIS)))
			throw Common::Exception("No such VIS");

		_vis.load(*vis);

		delete vis;
	} catch (Common::Exception &e) {
		delete vis;
		e.add("Failed loading VIS \"%s\"", _resRef.c_str());
		throw;
	}
}

void Area::loadARE(const Aurora::GFF3Struct &are) {
	// Name

	Aurora::LocString name;
	are.getLocString("Name", name);

	_name = name.getString();
}

void Area::loadGIT(const Aurora::GFF3Struct &git) {
	if (git.hasField("AreaProperties"))
		loadProperties(git.getStruct("AreaProperties"));

	if (git.hasField("Placeable List"))
		loadPlaceables(git.getList("Placeable List"));

	if (git.hasField("Door List"))
		loadDoors(git.getList("Door List"));

	if (git.hasField("Creature List"))
		loadCreatures(git.getList("Creature List"));
}

void Area::loadProperties(const Aurora::GFF3Struct &props) {
	// Ambient sound

	const Aurora::TwoDAFile &ambientSound = TwoDAReg.get2DA("ambientsound");

	uint32 ambientDay   = props.getUint("AmbientSndDay"  , Aurora::kStrRefInvalid);
	uint32 ambientNight = props.getUint("AmbientSndNight", Aurora::kStrRefInvalid);

	_ambientDay   = ambientSound.getRow(ambientDay  ).getString("Resource");
	_ambientNight = ambientSound.getRow(ambientNight).getString("Resource");

	uint32 ambientDayVol   = CLIP<uint32>(props.getUint("AmbientSndDayVol"  , 127), 0, 127);
	uint32 ambientNightVol = CLIP<uint32>(props.getUint("AmbientSndNightVol", 127), 0, 127);

	_ambientDayVol   = 1.25f * (1.0f - (1.0f / powf(5.0f, ambientDayVol   / 127.0f)));
	_ambientNightVol = 1.25f * (1.0f - (1.0f / powf(5.0f, ambientNightVol / 127.0f)));

	// TODO: PresetInstance0 - PresetInstance7


	// Ambient music

	const Aurora::TwoDAFile &ambientMusic = TwoDAReg.get2DA("ambientmusic");

	uint32 musicDay   = props.getUint("MusicDay"   , Aurora::kStrRefInvalid);
	uint32 musicNight = props.getUint("MusicNight" , Aurora::kStrRefInvalid);

	_musicDay   = ambientMusic.getRow(musicDay  ).getString("Resource");
	_musicNight = ambientMusic.getRow(musicNight).getString("Resource");


	// Battle music

	uint32 musicBattle = props.getUint("MusicBattle", Aurora::kStrRefInvalid);

	if (musicBattle != Aurora::kStrRefInvalid) {
		_musicBattle = ambientMusic.getRow(musicBattle).getString("Resource");

		// Battle stingers
		Common::UString stinger[3];
		stinger[0] = ambientMusic.getRow(musicBattle).getString("Stinger1");
		stinger[1] = ambientMusic.getRow(musicBattle).getString("Stinger2");
		stinger[2] = ambientMusic.getRow(musicBattle).getString("Stinger3");

		for (int i = 0; i < 3; i++)
			if (!stinger[i].empty())
				_musicBattleStinger.push_back(stinger[i]);
	}
}

void Area::loadRooms() {
	const Aurora::LYTFile::RoomArray &rooms = _lyt.getRooms();
	for (Aurora::LYTFile::RoomArray::const_iterator r = rooms.begin(); r != rooms.end(); ++r)
		_rooms.push_back(new Room(r->model, r->x, r->y, r->z));
}

void Area::loadObject(Object &object) {
	_objects.push_back(&object);

	if (!object.isStatic()) {
		const std::list<uint32> &ids = object.getIDs();

		for (std::list<uint32>::const_iterator id = ids.begin(); id != ids.end(); ++id)
			_objectMap.insert(std::make_pair(*id, &object));
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
		Door *door = new Door(**d);

		loadObject(*door);
	}
}

void Area::loadCreatures(const Aurora::GFF3List &list) {
	for (Aurora::GFF3List::const_iterator c = list.begin(); c != list.end(); ++c) {
		Creature *creature = new Creature(**c);

		loadObject(*creature);
	}
}

void Area::unload() {
	hide();
	removeFocus();

	for (ObjectList::iterator o = _objects.begin(); o != _objects.end(); ++o)
		delete *o;

	for (RoomList::iterator r = _rooms.begin(); r != _rooms.end(); ++r)
		delete *r;

	_objects.clear();
	_rooms.clear();

	_loaded = false;
}

void Area::addEvent(const Events::Event &event) {
	_eventQueue.push_back(event);
}

void Area::processEventQueue() {
	bool hasMove = false;
	for (std::list<Events::Event>::const_iterator e = _eventQueue.begin();
	     e != _eventQueue.end(); ++e) {

		if        (e->type == Events::kEventMouseMove) {
			hasMove = true;
		} else if (e->type == Events::kEventKeyDown) {
			if (e->key.keysym.sym == SDLK_TAB)
				highlightAll(true);
		} else if (e->type == Events::kEventKeyUp) {
			if (e->key.keysym.sym == SDLK_TAB)
				highlightAll(false);
		}
	}

	_eventQueue.clear();

	if (hasMove)
		checkActive();
}

Object *Area::getObjectAt(int x, int y) {
	const Graphics::Renderable *obj = GfxMan.getObjectAt(x, y);
	if (!obj)
		return 0;

	ObjectMap::iterator o = _objectMap.find(obj->getID());
	if (o == _objectMap.end())
		return 0;

	return o->second;
}

void Area::setActive(Object *object) {
	if (object == _activeObject)
		return;

	if (_activeObject)
		_activeObject->leave();

	_activeObject = object;

	if (_activeObject)
		_activeObject->enter();
}

void Area::checkActive() {
	if (!_loaded || _highlightAll)
		return;

	Common::StackLock lock(_mutex);

	int x, y;
	CursorMan.getPosition(x, y);

	setActive(getObjectAt(x, y));
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

} // End of namespace KotOR

} // End of namespace Engines
