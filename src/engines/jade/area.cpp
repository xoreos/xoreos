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

#include "common/util.h"
#include "common/error.h"
#include "common/stream.h"

#include "aurora/resman.h"
#include "aurora/gfffile.h"

#include "graphics/graphics.h"
#include "graphics/renderable.h"

#include "graphics/aurora/cursorman.h"

#include "engines/jade/area.h"
#include "engines/jade/room.h"
#include "engines/jade/artplaceable.h"

namespace Engines {

namespace Jade {

Area::Area() : _loaded(false), _visible(false), _activeObject(0), _highlightAll(false) {
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

	GfxMan.lockFrame();

	// Hide objects
	for (ObjectList::iterator o = _objects.begin(); o != _objects.end(); ++o)
		(*o)->hide();

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
	loadArtPlaceables();

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

void Area::loadObject(Object &object) {
	_objects.push_back(&object);

	if (!object.isStatic()) {
		const std::list<uint32> &ids = object.getIDs();

		for (std::list<uint32>::const_iterator id = ids.begin(); id != ids.end(); ++id)
			_objectMap.insert(std::make_pair(*id, &object));
	}
}

void Area::loadArtPlaceables() {
	const Aurora::LYTFile::ArtPlaceableArray &objects = _lyt.getArtPlaceables();
	for (uint32 i = 0; i < objects.size(); i++) {
		ArtPlaceable *object = new ArtPlaceable(objects[i].model, i, objects[i].x, objects[i].y, objects[i].z);

		loadObject(*object);
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
}

} // End of namespace Jade

} // End of namespace Engines
