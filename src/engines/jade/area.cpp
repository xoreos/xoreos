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
#include "aurora/gfffile.h"

#include "graphics/graphics.h"

#include "engines/jade/area.h"
#include "engines/jade/room.h"

namespace Engines {

namespace Jade {

Area::Area() : _loaded(false), _visible(false) {
}

Area::~Area() {
	unload();
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
	for (RoomList::iterator r = _rooms.begin(); r != _rooms.end(); ++r)
		(*r)->show();

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
	for (RoomList::iterator r = _rooms.begin(); r != _rooms.end(); ++r)
		(*r)->show();

	GfxMan.unlockFrame();

	_visible = false;
}

void Area::load(const Common::UString &resRef) {
	_resRef = resRef;

	loadLYT(); // Room layout
	loadVIS(); // Room visibilities

	loadRooms();

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

void Area::loadRooms() {
	const Aurora::LYTFile::RoomArray &rooms = _lyt.getRooms();
	for (uint32 i = 0; i < rooms.size(); i++)
		_rooms.push_back(new Room(rooms[i].model, i, rooms[i].x, rooms[i].y, rooms[i].z));
}

void Area::unload() {
	hide();
	removeFocus();

	for (RoomList::iterator r = _rooms.begin(); r != _rooms.end(); ++r)
		delete *r;

	_rooms.clear();

	_loaded = false;
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
