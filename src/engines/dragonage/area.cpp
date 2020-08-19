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
 *  The context holding a Dragon Age: Origins area.
 */

#include "src/common/util.h"
#include "src/common/strutil.h"
#include "src/common/error.h"

#include "src/aurora/resman.h"
#include "src/aurora/rimfile.h"
#include "src/aurora/gff3file.h"
#include "src/aurora/gff4file.h"

#include "src/graphics/graphics.h"
#include "src/graphics/renderable.h"

#include "src/graphics/aurora/cursorman.h"

#include "src/events/events.h"

#include "src/engines/dragonage/area.h"
#include "src/engines/dragonage/campaign.h"
#include "src/engines/dragonage/room.h"
#include "src/engines/dragonage/object.h"
#include "src/engines/dragonage/waypoint.h"
#include "src/engines/dragonage/placeable.h"
#include "src/engines/dragonage/creature.h"

namespace Engines {

namespace DragonAge {

static const uint32_t kARLID     = MKTAG('A', 'R', 'L', ' ');
static const uint32_t kAREID     = MKTAG('A', 'R', 'E', ' ');
static const uint32_t kVersion40 = MKTAG('V', '4', '.', '0');

static const uint32_t kROOMID    = MKTAG('R', 'O', 'O', 'M');

using ::Aurora::GFF3File;
using ::Aurora::GFF3Struct;
using ::Aurora::GFF3List;

using ::Aurora::GFF4File;
using ::Aurora::GFF4Struct;
using ::Aurora::GFF4List;

using namespace ::Aurora::GFF4FieldNamesEnum;

Area::Area(Campaign &campaign, const Common::UString &resRef,
           const Common::UString &env, const Common::UString &rim) :
	Object(kObjectTypeArea), _campaign(&campaign), _resRef(resRef), _environmentID(0xFFFFFFFF), _activeObject(0), _highlightAll(0) {

	try {

		load(resRef, env, rim);

	} catch (Common::Exception &e) {
		clean();

		e.add("Failed loading area \"%s\"", resRef.c_str());
		throw e;
	}
}

Area::~Area() {
	hide();
	clean();
}

const Common::UString &Area::getResRef() const {
	return _resRef;
}

const Aurora::LocString &Area::getName() const {
	return _name;
}

void Area::clean() {
	try {
		for (Objects::iterator o = _objects.begin(); o != _objects.end(); ++o)
			_campaign->removeObject(**o);

		deindexResources(_resources);
	} catch (...) {
	}
}

void Area::load(const Common::UString &resRef, const Common::UString &env, const Common::UString &rim) {
	indexOptionalArchive(rim + ".rim", 11000, _resources);

	loadEnvironment(env);
	loadARE(resRef);
}

void Area::loadEnvironment(const Common::UString &resRef) {
	GFF4File arl(resRef, Aurora::kFileTypeARL, kARLID);
	if (arl.getTypeVersion() != kVersion40)
		throw Common::Exception("Unsupported ARL version %s", Common::debugTag(arl.getTypeVersion()).c_str());

	const GFF4Struct &arlTop = arl.getTopLevel();

	_environmentID = (uint32_t) ((int32_t) arlTop.getSint(kGFF4EnvAreaID, -1));

	_environmentName = arlTop.getString(kGFF4EnvAreaName);
	_skyDome         = arlTop.getString(kGFF4EnvAreaSkydomeModel);

	_startPoint = arlTop.getString(kGFF4EnvAreaStartPointName);

	const Common::UString layout = arlTop.getString(kGFF4EnvAreaLayoutName);
	if (!layout.empty()) {
		indexOptionalArchive(layout + ".rim"    , 11001, _resources);
		indexOptionalArchive(layout + ".gpu.rim", 11002, _resources);
	}

	const GFF4List &rooms = arlTop.getList(kGFF4EnvAreaRoomList);
	_rooms.reserve(rooms.size());

	for (GFF4List::const_iterator r = rooms.begin(); r != rooms.end(); ++r) {
		if (EventMan.quitRequested())
			break;

		if (!*r || ((*r)->getLabel() != kROOMID))
			continue;

		_rooms.push_back(new Room(**r));
	}
}

void Area::loadARE(const Common::UString &resRef) {
	GFF3File are(resRef, Aurora::kFileTypeARE, kAREID);

	const GFF3Struct &areTop = are.getTopLevel();

	_tag = areTop.getString("Tag");

	areTop.getLocString("Name", _name);

	readVarTable(areTop);
	readScript(areTop);
	enableEvents(true);

	if (areTop.hasField("WaypointList"))
		loadWaypoints (areTop.getList("WaypointList"));
	if (areTop.hasField("PlaceableList"))
		loadPlaceables(areTop.getList("PlaceableList"));
	if (areTop.hasField("CreatureList"))
		loadCreatures (areTop.getList("CreatureList"));
}

void Area::loadObject(DragonAge::Object &object) {
	_objects.push_back(&object);

	_campaign->addObject(object);

	if (!object.isStatic()) {
		const std::list<uint32_t> &ids = object.getIDs();

		for (std::list<uint32_t>::const_iterator id = ids.begin(); id != ids.end(); ++id)
			_objectMap.insert(std::make_pair(*id, &object));
	}
}

void Area::loadWaypoints(const Aurora::GFF3List &list) {
	for (Aurora::GFF3List::const_iterator w = list.begin(); w != list.end(); ++w) {
		Waypoint *waypoint = new Waypoint(**w);

		loadObject(*waypoint);
	}
}

void Area::loadPlaceables(const Aurora::GFF3List &list) {
	for (Aurora::GFF3List::const_iterator p = list.begin(); p != list.end(); ++p) {
		Placeable *placeable = new Placeable(**p);

		loadObject(*placeable);
	}
}

void Area::loadCreatures(const Aurora::GFF3List &list) {
	for (Aurora::GFF3List::const_iterator c = list.begin(); c != list.end(); ++c) {
		Creature *creature = new Creature(**c);

		loadObject(*creature);
	}
}

void Area::show() {
	_eventQueue.clear();

	GfxMan.lockFrame();

	for (Rooms::iterator r = _rooms.begin(); r != _rooms.end(); ++r)
		(*r)->show();
	for (Objects::iterator o = _objects.begin(); o != _objects.end(); ++o)
		(*o)->show();

	GfxMan.unlockFrame();
}

void Area::hide() {
	GfxMan.lockFrame();

	removeFocus();

	for (Objects::iterator o = _objects.begin(); o != _objects.end(); ++o)
		(*o)->hide();
	for (Rooms::iterator r = _rooms.begin(); r != _rooms.end(); ++r)
		(*r)->hide();

	GfxMan.unlockFrame();
}

Common::UString Area::getName(const Common::UString &resRef, const Common::UString &rimFile) {
	if (!rimFile.empty()) {

		try {
			Common::SeekableReadStream *rimStream = ResMan.getResource(rimFile, Aurora::kFileTypeRIM);
			if (!rimStream)
				throw 0;

			const Aurora::RIMFile rim(rimStream);
			const uint32_t areIndex = rim.findResource(resRef, Aurora::kFileTypeARE);

			const GFF3File are(rim.getResource(areIndex), kAREID);

			return are.getTopLevel().getString("Name");
		} catch (...) {
		}

	}

	try {
		const GFF3File are(resRef, Aurora::kFileTypeARE, kAREID);

		return are.getTopLevel().getString("Name");
	} catch (...) {
	}

	return "";
}

void Area::getEntryLocation(float &posX, float &posY, float &posZ,
                            float &orientX, float &orientY, float &orientZ, float &orientAngle) const {

	DragonAge::Object *object = 0;

	object = ObjectContainer::toObject(_campaign->getFirstObjectByTag(_startPoint));
	if (!object)
		object = ObjectContainer::toObject(_campaign->getFirstObjectByType(kObjectTypeWaypoint));
	if (!object)
		object = ObjectContainer::toObject(_campaign->getFirstObject());

	if (object) {
		object->getPosition(posX, posY, posZ);
		object->getOrientation(orientX, orientY, orientZ, orientAngle);
		return;
	}

	posX = 0.0f;
	posY = 0.0f;
	posZ = 0.0f;

	orientX = 0.0f;
	orientY = 0.0f;
	orientZ = 0.0f;

	orientAngle = 0.0f;
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

DragonAge::Object *Area::getObjectAt(int x, int y) {
	const Graphics::Renderable *obj = GfxMan.getObjectAt(x, y);
	if (!obj)
		return 0;

	ObjectMap::iterator o = _objectMap.find(obj->getID());
	if (o == _objectMap.end())
		return 0;

	return o->second;
}

void Area::setActive(DragonAge::Object *object) {
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

	DragonAge::Object *o = getObjectAt(x, y);
	if (!o)
		return;

	o->click(_campaign->getPC());
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

} // End of namespace DragonAge

} // End of namespace Engines
