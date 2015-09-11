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
#include "src/aurora/gff3file.h"

#include "src/graphics/graphics.h"
#include "src/graphics/renderable.h"

#include "src/graphics/aurora/cursorman.h"

#include "src/engines/aurora/resources.h"

#include "src/engines/jade/area.h"
#include "src/engines/jade/room.h"
#include "src/engines/jade/creature.h"
#include "src/engines/jade/placeable.h"
#include "src/engines/jade/trigger.h"
#include "src/engines/jade/waypoint.h"

namespace Engines {

namespace Jade {

Area::Area(Module &module, const Common::UString &resRef) : Object(kObjectTypeArea),
	_module(&module), _resRef(resRef), _visible(false), _activeObject(0), _highlightAll(false) {

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
	Aurora::GFF3File are(_resRef, Aurora::kFileTypeARE, MKTAG('A', 'R', 'E', ' '));
	loadARE(are.getTopLevel());

	loadResources();

	Aurora::GFF3File sav(_resRef, Aurora::kFileTypeSAV, MKTAG('S', 'A', 'V', ' '));
	loadSAV(sav.getTopLevel());

	loadLYT(); // Room layout
	loadVIS(); // Room visibilities

	loadRooms();
}

void Area::clear() {
	// Delete objects
	for (ObjectList::iterator o = _objects.begin(); o != _objects.end(); ++o) {
		_module->removeObject(**o);

		delete *o;
	}

	for (RoomList::iterator r = _rooms.begin(); r != _rooms.end(); ++r)
		delete *r;

	_objects.clear();
	_rooms.clear();
}

const Common::UString &Area::getResRef() {
	return _resRef;
}

void Area::show() {
	if (_visible)
		return;

	GfxMan.lockFrame();

	// Show rooms
	for (RoomList::iterator r = _rooms.begin(); r != _rooms.end(); ++r)
		(*r)->show();

	// Show objects
	for (ObjectList::iterator o = _objects.begin(); o != _objects.end(); ++o) {
		(*o)->show();
		(*o)->runScript(kScriptOnSpawn, *o, this);
	}

	GfxMan.unlockFrame();

	_visible = true;
}

void Area::hide() {
	if (!_visible)
		return;

	removeFocus();

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

void Area::loadARE(const Aurora::GFF3Struct &are) {
	_layout = are.getString("Layout");

	// Scripts
	readScripts(are);
}

void Area::loadSAV(const Aurora::GFF3Struct &sav) {
	if (sav.hasField("CreatureList")) {
		const Aurora::GFF3Struct &creatures = sav.getStruct("CreatureList");
		loadCreatures(creatures.getList("StaticList"));
		loadCreatures(creatures.getList("DynamicList"));
	}

	// TODO load crowd list

	if (sav.hasField("PlaceableList")) {
		const Aurora::GFF3Struct &placeables = sav.getStruct("PlaceableList");
		loadPlaceables(placeables.getList("StaticList"));
		loadPlaceables(placeables.getList("DynamicList"));
	}

	// TODO load sound list

	if (sav.hasField("TriggerList")) {
		const Aurora::GFF3Struct &trigger = sav.getStruct("TriggerList");
		loadTriggers(trigger.getList("StaticList"));
		loadTriggers(trigger.getList("DynamicList"));
	}

	if (sav.hasField("WaypointList")) {
		const Aurora::GFF3Struct &waypoints = sav.getStruct("WaypointList");
		loadWaypoints(waypoints.getList("StaticList"));
		loadWaypoints(waypoints.getList("DynamicList"));
	}

	// TODO load projectile list

	// TODO load area of effect list

	// TODO load store list

	// TODO load apple list

	// TODO load camera list
}

void Area::loadResources() {
	Common::ChangeID change;

	_resources.push_back(Common::ChangeID());
	indexMandatoryArchive(_resRef + "/" + _layout + ".rim"  , 1000, &_resources.back());

	_resources.push_back(Common::ChangeID());
	indexMandatoryArchive(_resRef + "/" + _layout + "-a.rim", 1001, &_resources.back());
}

void Area::loadLYT() {
	Common::SeekableReadStream *lyt = 0;
	try {
		if (!(lyt = ResMan.getResource(_layout, Aurora::kFileTypeLYT)))
			throw Common::Exception("No such LYT");

		_lyt.load(*lyt);

		delete lyt;
	} catch (Common::Exception &e) {
		delete lyt;
		e.add("Failed loading LYT \"%s\"", _layout.c_str());
		throw;
	}
}

void Area::loadVIS() {
	Common::SeekableReadStream *vis = 0;
	try {
		if (!(vis = ResMan.getResource(_layout, Aurora::kFileTypeVIS)))
			throw Common::Exception("No such VIS");

		_vis.load(*vis);

		delete vis;
	} catch (Common::Exception &e) {
		delete vis;
		e.add("Failed loading VIS \"%s\"", _layout.c_str());
		throw;
	}
}

void Area::loadRooms() {
	const Aurora::LYTFile::RoomArray &rooms = _lyt.getRooms();
	for (size_t i = 0; i < rooms.size(); i++)
		_rooms.push_back(new Room(rooms[i].model, i, rooms[i].x, rooms[i].y, rooms[i].z));
}

void Area::loadObject(Object &object) {
	_objects.push_back(&object);
	_module->addObject(object);

	if (!object.isStatic()) {
		const std::list<uint32> &ids = object.getIDs();

		for (std::list<uint32>::const_iterator id = ids.begin(); id != ids.end(); ++id)
			_objectMap.insert(std::make_pair(*id, &object));
	}
}

void Area::loadWaypoints(const Aurora::GFF3List &list) {
	for (Aurora::GFF3List::const_iterator w = list.begin(); w != list.end(); ++w) {
		Waypoint *waypoint = new Waypoint(**w);

		loadObject(*waypoint);
	}
}

void Area::loadCreatures(const Aurora::GFF3List &list) {
	for (Aurora::GFF3List::const_iterator c = list.begin(); c != list.end(); ++c) {
		Creature *creature = new Creature(**c);

		loadObject(*creature);
	}
}

void Area::loadPlaceables(const Aurora::GFF3List &list) {
	for (Aurora::GFF3List::const_iterator c = list.begin(); c != list.end(); ++c) {
		Placeable *placeable = new Placeable(**c);

		loadObject(*placeable);
	}
}

void Area::loadTriggers(const Aurora::GFF3List &list) {
	for (Aurora::GFF3List::const_iterator c = list.begin(); c != list.end(); ++c) {
		Trigger *trigger = new Trigger(**c);

		loadObject(*trigger);
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

Jade::Object *Area::getObjectAt(int x, int y) {
	const Graphics::Renderable *obj = GfxMan.getObjectAt(x, y);
	if (!obj)
		return 0;

	ObjectMap::iterator o = _objectMap.find(obj->getID());
	if (o == _objectMap.end())
		return 0;

	return o->second;
}

void Area::setActive(Jade::Object *object) {
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

	Common::StackLock lock(_mutex);

	if ((x < 0) || (y < 0))
		CursorMan.getPosition(x, y);

	setActive(getObjectAt(x, y));
}

void Area::click(int x, int y) {
	Common::StackLock lock(_mutex);

	Jade::Object *o = getObjectAt(x, y);
	if (!o)
		return;

	o->runScript(kScriptOnClick, o, _module->getPC());
	o->runScript(kScriptOnUse, o, _module->getPC());
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

} // End of namespace Jade

} // End of namespace Engines
