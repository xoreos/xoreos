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

/** @file engines/kotor/area.cpp
 *  KotOR area.
 */

#include "common/util.h"
#include "common/error.h"
#include "common/stream.h"

#include "aurora/resman.h"
#include "aurora/locstring.h"
#include "aurora/gfffile.h"
#include "aurora/2dafile.h"
#include "aurora/2dareg.h"

#include "graphics/graphics.h"

#include "graphics/aurora/model_kotor.h"

#include "graphics/aurora/model_kotor.h"
#include "graphics/aurora/sceneman.h"

#include "sound/sound.h"

#include "engines/aurora/util.h"

#include "engines/kotor/area.h"
#include "engines/kotor/placeable.h"
#include "engines/kotor/door.h"
#include "engines/kotor/creature.h"

namespace Engines {

namespace KotOR {

Area::Room::Room(const Aurora::LYTFile::Room &lRoom) :
	lytRoom(&lRoom), model(0), visible(false) {
}

Area::Room::~Room() {
	destroyModel(model);
}


Area::Area() : _loaded(false), _visible(false), _highlightAll(false) {
}

Area::~Area() {
	setVisible(false);

	for (ObjectList::iterator o = _objects.begin(); o != _objects.end(); ++o)
		delete *o;

	for (std::vector<Room *>::iterator r = _rooms.begin(); r != _rooms.end(); ++r)
		delete *r;
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

void Area::setVisible(bool visible) {
	if (_visible == visible)
		return;

	assert(_loaded);

	if (visible) {
		playAmbientSound();
		playAmbientMusic();

		{
			LOCK_FRAME();

			// Show rooms
			for (std::vector<Room *>::iterator r = _rooms.begin(); r != _rooms.end(); ++r)
				(*r)->model->setVisible(true);

			// Show objects
			for (ObjectList::iterator o = _objects.begin(); o != _objects.end(); ++o)
				(*o)->setVisible(true);
		}

	} else {
		stopSound();

		{
			LOCK_FRAME();

			// Hide objects
			for (ObjectList::iterator o = _objects.begin(); o != _objects.end(); ++o)
				(*o)->setVisible(false);

			// Hide rooms
			for (std::vector<Room *>::iterator room = _rooms.begin(); room != _rooms.end(); ++room)
				(*room)->model->setVisible(false);
		}

	}

	_visible = visible;
}

void Area::load(const Common::UString &resRef) {
	_resRef = resRef;

	loadLYT(); // Room layout
	loadVIS(); // Room visibilities

	loadRoomModels();   // Room models
	loadRoomVisibles(); // Room model visibilities

	Aurora::GFFFile are(_resRef, Aurora::kFileTypeARE, MKTAG('A', 'R', 'E', ' '));
	loadARE(are.getTopLevel());

	Aurora::GFFFile git(_resRef, Aurora::kFileTypeGIT, MKTAG('G', 'I', 'T', ' '));
	loadGIT(git.getTopLevel());

	loadObjectModels();

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

void Area::loadARE(const Aurora::GFFStruct &are) {
	// Name

	Aurora::LocString name;
	are.getLocString("Name", name);

	_name = name.getString();
}

void Area::loadGIT(const Aurora::GFFStruct &git) {
	if (git.hasField("AreaProperties"))
		loadProperties(git.getStruct("AreaProperties"));

	if (git.hasField("Placeable List"))
		loadPlaceables(git.getList("Placeable List"));

	if (git.hasField("Door List"))
		loadDoors(git.getList("Door List"));

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

	const Aurora::TwoDAFile &ambientMusic = TwoDAReg.get("ambientmusic");

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

void Area::loadObject(Engines::KotOR::Object &object) {
	_objects.push_back(&object);
}

void Area::loadPlaceables(const Aurora::GFFList &list) {
	for (Aurora::GFFList::const_iterator p = list.begin(); p != list.end(); ++p) {
		Placeable *placeable = new Placeable(**p);

		loadObject(*placeable);
	}
}

void Area::loadDoors(const Aurora::GFFList &list) {
	for (Aurora::GFFList::const_iterator d = list.begin(); d != list.end(); ++d) {
		Door *door = new Door(**d);

		loadObject(*door);
	}
}

void Area::loadCreatures(const Aurora::GFFList &list) {
	for (Aurora::GFFList::const_iterator c = list.begin(); c != list.end(); ++c) {
		Creature *creature = new Creature(**c);

		loadObject(*creature);
	}
}

void Area::loadRoomModels() {
	const Aurora::LYTFile::RoomArray &rooms = _lyt.getRooms();
	_rooms.reserve(rooms.size());
	for (size_t i = 0; i < rooms.size(); i++) {
		const Aurora::LYTFile::Room &lytRoom = rooms[i];

		if (lytRoom.model == "****")
			// No model for that room
			continue;

		Room *room = new Room(lytRoom);

		try {
			room->model = createWorldModel(lytRoom.model);
		} catch (Common::Exception &e) {
			e.add("Failed loading area \"%s\"", _resRef.c_str());
			throw;
		}

		room->model->setPosition(lytRoom.x, lytRoom.z, -lytRoom.y);

		_rooms.push_back(room);
	}

}

void Area::loadRoomVisibles() {
	// Go through all rooms
	for (std::vector<Room *>::iterator room = _rooms.begin(); room != _rooms.end(); ++room) {
		// Get visibility information for that room
		const std::vector<Common::UString> &rooms = _vis.getVisibilityArray((*room)->lytRoom->model);

		if (rooms.empty()) {
			// If no info is available, assume all rooms are visible

			for (std::vector<Room *>::iterator iRoom = _rooms.begin(); iRoom != _rooms.end(); ++iRoom)
				(*room)->visibles.push_back(*iRoom);

			return;
		}

		// Otherwise, go through all rooms again, look for a match with the visibilities
		for (std::vector<Room *>::iterator iRoom = _rooms.begin(); iRoom != _rooms.end(); ++iRoom) {

			for (std::vector<Common::UString>::const_iterator vRoom = rooms.begin(); vRoom != rooms.end(); ++vRoom) {
				if (vRoom->equalsIgnoreCase((*iRoom)->lytRoom->model)) {
					// Mark that room as visible from the first room
					(*room)->visibles.push_back(*iRoom);
					break;
				}
			}

		}

	}

}

void Area::loadObjectModels() {
	for (ObjectList::iterator o = _objects.begin(); o != _objects.end(); ++o) {
		(*o)->loadModel();

		if (!(*o)->isStatic())
			for (std::vector<Common::UString>::const_iterator i = (*o)->getModelIDs().begin(); i != (*o)->getModelIDs().end(); ++i)
				_dynamicObjects.insert(std::make_pair(*i, *o));
	}
}

Engines::KotOR::Object *Area::getObjectAt(int x, int y, float &distance) const {
	Graphics::Renderable *r = SceneMan.getRenderableAt(x, y, Graphics::Aurora::kSelectableRenderable, distance);
	if (!r)
		return 0;

	ObjectMap::const_iterator o = _dynamicObjects.find(r->getID());
	if (o == _dynamicObjects.end())
		return 0;

	return o->second;
}

bool Area::getHighlightAll() const {
	return _highlightAll;
}

void Area::setHighlightAll(bool highlight) {
	for (ObjectList::iterator o = _objects.begin(); o != _objects.end(); ++o)
		if (!(*o)->isStatic())
			(*o)->setHighlight(highlight);

	_highlightAll = highlight;
}

} // End of namespace KotOR

} // End of namespace Engines
