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
 *  An NWScript object container.
 */

#include <cassert>

#include <algorithm>

#include "src/common/error.h"

#include "src/aurora/types.h"

#include "src/aurora/nwscript/objectcontainer.h"

namespace Aurora {

namespace NWScript {

ObjectContainer::ObjectContainer() {
}

ObjectContainer::~ObjectContainer() {
}

void ObjectContainer::clearObjects() {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	_objects.clear();
	_objectsByID.clear();
	_objectsByTag.clear();
}

void ObjectContainer::addObject(Object &object) {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	assert(std::find(_objects.begin(), _objects.end(), &object) == _objects.end());

	_objects.push_back(&object);
	_objectsByID.insert(std::make_pair(object.getID(), &object));
	_objectsByTag.insert(std::make_pair(object.getTag(), &object));
}

void ObjectContainer::removeObject(Object &object) {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	_objects.remove(&object);
	_objectsByID.erase(object.getID());

	std::pair<ObjectTagMap::iterator, ObjectTagMap::iterator> tag =
		_objectsByTag.equal_range(object.getTag());

	for (ObjectTagMap::iterator o = tag.first; o != tag.second; ++o) {
		if (o->second == &object) {
			_objectsByTag.erase(o);
			break;
		}
	}
}

Object *ObjectContainer::getObjectByID(uint32_t id) const {
	ObjectIDMap::const_iterator o = _objectsByID.find(id);
	if (o != _objectsByID.end())
		return o->second;

	return 0;
}

Object *ObjectContainer::getFirstObject() const {
	SearchList ctx(_objects);

	return ctx.get();
}

Object *ObjectContainer::getFirstObjectByTag(const Common::UString &tag) const {
	SearchTagMap ctx(_objectsByTag, tag);

	return ctx.get();
}

ObjectSearch *ObjectContainer::findObjects() const {
	return new SearchList(_objects);
}

ObjectSearch *ObjectContainer::findObjectsByTag(const Common::UString &tag) const {
	return new SearchTagMap(_objectsByTag, tag);
}

void ObjectContainer::lock() {
	_mutex.lock();
}

void ObjectContainer::unlock() {
	_mutex.unlock();
}

} // End of namespace NWScript

} // End of namespace Aurora
