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
 *  A container of Sonic Chronicles: The Dark Brotherhood objects.
 */

#include "src/common/types.h"
#include "src/common/util.h"

#include "src/engines/sonic/objectcontainer.h"
#include "src/engines/sonic/object.h"
#include "src/engines/sonic/module.h"
#include "src/engines/sonic/area.h"
#include "src/engines/sonic/placeable.h"

namespace Engines {

namespace Sonic {

ObjectDistanceSort::ObjectDistanceSort(const Sonic::Object &target) {
	target.getPosition(xt, yt, zt);
}

bool ObjectDistanceSort::operator()(Sonic::Object *a, Sonic::Object *b) {
	return getDistance(*a) < getDistance(*b);
}

float ObjectDistanceSort::getDistance(Sonic::Object &a) {
	float x, y, z;
	a.getPosition(x, y, z);

	return ABS(x - xt) + ABS(y - yt) + ABS(z - zt);
}


class SearchType : public ::Aurora::NWScript::SearchRange< std::list<Sonic::Object *> > {
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

void ObjectContainer::addObject(Sonic::Object &object) {
	lock();

	::Aurora::NWScript::ObjectContainer::addObject(object);

	_objects[object.getType()].push_back(&object);

	unlock();
}

void ObjectContainer::removeObject(Sonic::Object &object) {
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

Sonic::Object *ObjectContainer::toObject(::Aurora::NWScript::Object *object) {
	return dynamic_cast<Sonic::Object *>(object);
}

Module *ObjectContainer::toModule(Aurora::NWScript::Object *object) {
	return dynamic_cast<Module *>(object);
}

Area *ObjectContainer::toArea(Aurora::NWScript::Object *object) {
	return dynamic_cast<Area *>(object);
}

Placeable *ObjectContainer::toPlaceable(Aurora::NWScript::Object *object) {
	return dynamic_cast<Placeable *>(object);
}

} // End of namespace Sonic

} // End of namespace Engines
