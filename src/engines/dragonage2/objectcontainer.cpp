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
 *  A container of Dragon Age II objects.
 */

#include "src/common/types.h"

#include "src/engines/dragonage2/objectcontainer.h"
#include "src/engines/dragonage2/scriptobject.h"
#include "src/engines/dragonage2/area.h"
#include "src/engines/dragonage2/object.h"
#include "src/engines/dragonage2/waypoint.h"
#include "src/engines/dragonage2/placeable.h"
#include "src/engines/dragonage2/creature.h"

namespace Engines {

namespace DragonAge2 {

class SearchType : public ::Aurora::NWScript::SearchRange< std::list<ScriptObject *> > {
public:
	SearchType(const iterator &a, const iterator &b) : SearchRange(std::make_pair(a, b)) { }
	SearchType(const type &l) : SearchRange(std::make_pair(l.begin(), l.end())) { }
	~SearchType() { }

	::Aurora::NWScript::Object *getObject(const iterator &t) { return *t; }
};


ObjectContainer::ObjectContainer() {
}

ObjectContainer::~ObjectContainer() {
}

void ObjectContainer::clearObjects() {
	lock();

	for (size_t i = 0; i < kObjectTypeMAX; i++)
		_objects[i].clear();

	::Aurora::NWScript::ObjectContainer::clearObjects();

	unlock();
}

void ObjectContainer::addObject(ScriptObject &object) {
	lock();

	::Aurora::NWScript::ObjectContainer::addObject(object);

	ObjectType type = object.getObjectType();
	if (((uint) type) < kObjectTypeMAX)
		_objects[type].push_back(&object);

	unlock();
}

void ObjectContainer::removeObject(ScriptObject &object) {
	lock();

	ObjectType type = object.getObjectType();
	if (((uint) type) < kObjectTypeMAX)
		_objects[type].remove(&object);

	::Aurora::NWScript::ObjectContainer::removeObject(object);

	unlock();
}

::Aurora::NWScript::Object *ObjectContainer::getFirstObjectByType(ObjectType type) const {
	if (((uint) type) >= kObjectTypeMAX)
		return 0;

	SearchType ctx(_objects[type]);

	return ctx.get();
}

::Aurora::NWScript::ObjectSearch *ObjectContainer::findObjectsByType(ObjectType type) const {
	if (((uint) type) >= kObjectTypeMAX)
		return new SearchType(_objects[0].end(), _objects[0].end());

	return new SearchType(_objects[type]);
}

ScriptObject *ObjectContainer::toScriptObject(::Aurora::NWScript::Object *object) {
	return dynamic_cast<ScriptObject *>(object);
}

DragonAge2::Object *ObjectContainer::toObject(::Aurora::NWScript::Object *object) {
	return dynamic_cast<DragonAge2::Object *>(object);
}

Area *ObjectContainer::toArea(::Aurora::NWScript::Object *object) {
	return dynamic_cast<Area *>(object);
}

Waypoint *ObjectContainer::toWaypoint(::Aurora::NWScript::Object *object) {
	return dynamic_cast<Waypoint *>(object);
}

Placeable *ObjectContainer::toPlaceable(::Aurora::NWScript::Object *object) {
	return dynamic_cast<Placeable *>(object);
}

Creature *ObjectContainer::toCreature(::Aurora::NWScript::Object *object) {
	return dynamic_cast<Creature *>(object);
}

} // End of namespace DragonAge2

} // End of namespace Engines
