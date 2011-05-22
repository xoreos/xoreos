/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file aurora/nwscript/objectcontainer.cpp
 *  An NWScript object container.
 */

#include "common/error.h"

#include "aurora/types.h"

#include "aurora/nwscript/objectcontainer.h"

namespace Aurora {

namespace NWScript {

ObjectContainer::ObjectContainer() : _currentID(0) {
	_currentSearchRange = std::make_pair(_objectsByTag.end(), _objectsByTag.end());
}

ObjectContainer::~ObjectContainer() {
}

void ObjectContainer::add(Object &obj) {
	Common::StackLock lock(_mutex);

	remove(obj);

	obj._id = ++_currentID;

	obj._objectContainer    = this;
	obj._objectContainerID  = _objectsByID .end();
	obj._objectContainerTag = _objectsByTag.end();

	std::pair<ObjectIDMap::iterator, bool> result;

	result = _objectsByID.insert(std::make_pair(obj.getID(), &obj));
	if (!result.second) {
		remove(obj);
		throw Common::Exception("Object ID collision");
	}

	obj._objectContainerID  = result.first;
	obj._objectContainerTag = _objectsByTag.insert(std::make_pair(obj.getTag(), &obj));
}

void ObjectContainer::remove(Object &obj) {
	Common::StackLock lock(_mutex);

	if (!obj._objectContainer)
		return;

	obj._id = kObjectIDInvalid;

	obj._objectContainer = 0;

	_objectsByID .erase(obj._objectContainerID);
	_objectsByTag.erase(obj._objectContainerTag);

	obj._objectContainerID  = _objectsByID .end();
	obj._objectContainerTag = _objectsByTag.end();
}

Object *ObjectContainer::getObject(uint32 id) const {
	if (id == kObjectIDInvalid)
		return 0;

	ObjectIDMap::const_iterator o = _objectsByID.find(id);
	if (o == _objectsByID.end())
		return 0;

	return o->second;
}

const Object *ObjectContainer::getConstObject(uint32 id) const {
	if (id == kObjectIDInvalid)
		return 0;

	ObjectIDMap::const_iterator o = _objectsByID.find(id);
	if (o == _objectsByID.end())
		return 0;

	return o->second;
}

uint32 ObjectContainer::findFirstObject(const Common::UString &tag) {
	_currentSearchTag   = tag;
	_currentSearchRange = _objectsByTag.equal_range(tag);

	if (_currentSearchRange.first == _currentSearchRange.second)
		return kObjectIDInvalid;

	return _currentSearchRange.first->second->getID();
}

uint32 ObjectContainer::findNextObject(const Common::UString &tag) {
	if (_currentSearchTag != tag)
		throw Common::Exception("ObjectContainer::findNextObject(): Tags mismatch");

	if (++_currentSearchRange.first == _currentSearchRange.second)
		return kObjectIDInvalid;

	return _currentSearchRange.first->second->getID();
}

} // End of namespace NWScript

} // End of namespace Aurora
