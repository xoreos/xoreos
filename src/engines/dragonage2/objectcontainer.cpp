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
#include "src/common/util.h"

#include "src/engines/dragonage2/objectcontainer.h"
#include "src/engines/dragonage2/object.h"
#include "src/engines/dragonage2/area.h"
#include "src/engines/dragonage2/waypoint.h"
#include "src/engines/dragonage2/placeable.h"
#include "src/engines/dragonage2/creature.h"
#include "src/engines/dragonage2/event.h"

namespace Engines {

namespace DragonAge2 {

ObjectDistanceSort::ObjectDistanceSort(const DragonAge2::Object &target) {
	target.getPosition(xt, yt, zt);
}

bool ObjectDistanceSort::operator()(DragonAge2::Object *a, DragonAge2::Object *b) {
	return getDistance(*a) < getDistance(*b);
}

float ObjectDistanceSort::getDistance(DragonAge2::Object &a) {
	float x, y, z;
	a.getPosition(x, y, z);

	return ABS(x - xt) + ABS(y - yt) + ABS(z - zt);
}


class SearchType : public ::Aurora::NWScript::SearchRange< std::list<DragonAge2::Object *> > {
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

void ObjectContainer::addObject(DragonAge2::Object &object) {
	lock();

	::Aurora::NWScript::ObjectContainer::addObject(object);

	_objects[object.getType()].push_back(&object);

	unlock();
}

void ObjectContainer::removeObject(DragonAge2::Object &object) {
	lock();

	_objects[object.getType()].remove(&object);

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

DragonAge2::Object *ObjectContainer::toObject(::Aurora::NWScript::Object *object) {
	return dynamic_cast<DragonAge2::Object *>(object);
}

Area *ObjectContainer::toArea(Aurora::NWScript::Object *object) {
	return dynamic_cast<Area *>(object);
}

Waypoint *ObjectContainer::toWaypoint(Aurora::NWScript::Object *object) {
	return dynamic_cast<Waypoint *>(object);
}

Placeable *ObjectContainer::toPlaceable(Aurora::NWScript::Object *object) {
	return dynamic_cast<Placeable *>(object);
}

Creature *ObjectContainer::toCreature(Aurora::NWScript::Object *object) {
	return dynamic_cast<Creature *>(object);
}

Event *ObjectContainer::toEvent(Aurora::NWScript::EngineType *engineType) {
	return dynamic_cast<Event *>(engineType);
}

} // End of namespace DragonAge2

} // End of namespace Engines
