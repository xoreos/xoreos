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
 *  A container of Dragon Age: Origins objects.
 */

#include "src/common/types.h"

#include "src/engines/dragonage/objectcontainer.h"
#include "src/engines/dragonage/scriptobject.h"
#include "src/engines/dragonage/area.h"
#include "src/engines/dragonage/object.h"
#include "src/engines/dragonage/waypoint.h"
#include "src/engines/dragonage/placeable.h"
#include "src/engines/dragonage/creature.h"

namespace Engines {

namespace DragonAge {

class SearchType : public ::Aurora::NWScript::SearchRange< std::list<ScriptObject *> > {
public:
	SearchType(const iterator &a, const iterator &b) : ::Aurora::NWScript::SearchRange<type>(std::make_pair(a, b)) { }
	SearchType(const type &l) : ::Aurora::NWScript::SearchRange<type>(std::make_pair(l.begin(), l.end())) { }
	~SearchType() { }

	::Aurora::NWScript::Object *getObject(const iterator &t) { return *t; }
};


ObjectContainer::ObjectContainer() {
}

ObjectContainer::~ObjectContainer() {
}

void ObjectContainer::clearObjects() {
	lock();

	_objects.clear();

	::Aurora::NWScript::ObjectContainer::clearObjects();

	unlock();
}

void ObjectContainer::addObject(ScriptObject &object) {
	lock();

	::Aurora::NWScript::ObjectContainer::addObject(object);

	_objects[object.getObjectType()].push_back(&object);

	unlock();
}

void ObjectContainer::removeObject(ScriptObject &object) {
	lock();

	_objects[object.getObjectType()].remove(&object);

	::Aurora::NWScript::ObjectContainer::removeObject(object);

	unlock();
}

::Aurora::NWScript::Object *ObjectContainer::getFirstObjectByType(ObjectType type) const {
	ObjectMap::const_iterator l = _objects.find(type);
	if (l == _objects.end())
		return 0;

	SearchType ctx(l->second);

	return ctx.get();
}

::Aurora::NWScript::ObjectSearch *ObjectContainer::findObjectsByType(ObjectType type) const {
	static const ObjectList kEmptyObjectList;

	ObjectMap::const_iterator l = _objects.find(type);
	if (l == _objects.end())
		return new SearchType(kEmptyObjectList.begin(), kEmptyObjectList.end());

	return new SearchType(l->second);
}

ScriptObject *ObjectContainer::toScriptObject(::Aurora::NWScript::Object *object) {
	return dynamic_cast<ScriptObject *>(object);
}

DragonAge::Object *ObjectContainer::toObject(::Aurora::NWScript::Object *object) {
	return dynamic_cast<DragonAge::Object *>(object);
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

} // End of namespace DragonAge

} // End of namespace Engines
