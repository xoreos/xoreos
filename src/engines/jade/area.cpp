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

/** @file engines/jade/area.cpp
 *  An area.
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

#include "graphics/aurora/cursorman.h"

#include "sound/sound.h"

#include "engines/aurora/util.h"
#include "engines/aurora/resources.h"
#include "engines/aurora/model.h"

#include "engines/jade/area.h"

namespace Engines {

namespace Jade {

Area::Room::Room(const Aurora::LYTFile::Room &lRoom) :
	lytRoom(&lRoom), model(0), visible(false) {
}

Area::Room::~Room() {
	delete model;
}


Area::Area() : _loaded(false), _visible(false) {
}

Area::~Area() {
	hide();

	removeFocus();

	unloadRooms();
}

const Common::UString &Area::getName() {
	return _resRef;
}

void Area::show() {
	assert(_loaded);

	if (_visible)
		return;

	GfxMan.lockFrame();

	// Show rooms
	for (std::vector<Room *>::iterator r = _rooms.begin(); r != _rooms.end(); ++r)
		(*r)->model->show();

	GfxMan.unlockFrame();

	_visible = true;
}

void Area::hide() {
	assert(_loaded);

	if (!_visible)
		return;

	removeFocus();

	GfxMan.lockFrame();

	// Hide rooms
	for (std::vector<Room *>::iterator room = _rooms.begin(); room != _rooms.end(); ++room)
		(*room)->model->hide();

	GfxMan.unlockFrame();

	_visible = false;
}

void Area::load(const Common::UString &resRef) {
	_resRef = resRef;

	loadLYT(); // Room layout
	loadVIS(); // Room visibilities

	loadModels();    // Room models
	loadVisibles();  // Room model visibilities

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

void Area::loadModels() {
	int resources = 0;

	const Aurora::LYTFile::RoomArray &rooms = _lyt.getRooms();
	_rooms.reserve(rooms.size());
	for (size_t i = 0; i < rooms.size(); i++) {
		const Aurora::LYTFile::Room &lytRoom = rooms[i];

		if (lytRoom.model == "****")
			// No model for that room
			continue;

		Aurora::ResourceManager::ChangeID change;

		indexOptionalArchive(Aurora::kArchiveRIM, lytRoom.model + "-a.rim", 500 + resources++, &change);
		_resources.push_back(change);

		Room *room = new Room(lytRoom);

		room->model = loadModelObject(lytRoom.model);
		if (!room->model) {
			delete room;
			throw Common::Exception("Can't load model \"%s\" for area \"%s\"",
			                        lytRoom.model.c_str(), _resRef.c_str());
		}

		room->model->setPosition(lytRoom.x, lytRoom.y, lytRoom.z);

		_rooms.push_back(room);
	}

}

void Area::loadVisibles() {
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

void Area::unloadRooms() {
	for (std::vector<Room *>::iterator r = _rooms.begin(); r != _rooms.end(); ++r)
		delete *r;

	_rooms.clear();

	std::list<Aurora::ResourceManager::ChangeID>::reverse_iterator r;
	for (r = _resources.rbegin(); r != _resources.rend(); ++r)
		ResMan.undo(*r);

	_resources.clear();
}

void Area::addEvent(const Events::Event &event) {
	_eventQueue.push_back(event);
}

void Area::processEventQueue() {
	_eventQueue.clear();
}

void Area::removeFocus() {
}

void Area::notifyCameraMoved() {
}

} // End of namespace Jade

} // End of namespace Engines
