/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
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
 */

/** @file aurora/nwscript/objectcontainer.cpp
 *  An NWScript object container.
 */

#include "common/error.h"

#include "aurora/types.h"

#include "aurora/nwscript/objectcontainer.h"

namespace Aurora {

namespace NWScript {

ObjectContainer::SearchContext::SearchContext() : _empty(true), _object(0) {
}

ObjectContainer::SearchContext::~SearchContext() {
}

Object *ObjectContainer::SearchContext::getObject() const {
	return _object;
}


ObjectContainer::ObjectContainer() : _currentID(0) {
}

ObjectContainer::~ObjectContainer() {
}

void ObjectContainer::addObject(Object &obj) {
	Common::StackLock lock(_mutex);

	removeObject(obj);

	obj._id = ++_currentID;

	obj._objectContainer    = this;
	obj._objectContainerTag = _objects.insert(std::make_pair(obj.getTag(), &obj));
}

void ObjectContainer::removeObject(Object &obj) {
	Common::StackLock lock(_mutex);

	if (!obj._objectContainer)
		return;

	obj._id = kObjectIDInvalid;

	obj._objectContainer = 0;

	_objects.erase(obj._objectContainerTag);
	obj._objectContainerTag = _objects.end();
}

bool ObjectContainer::findObjectInit(SearchContext &ctx) const {
	ctx._object = 0;
	ctx._tag    = "";
	ctx._range  = std::make_pair(_objects.begin(), _objects.end());
	ctx._empty  = ctx._range.first == ctx._range.second;

	return !ctx._empty;
}

bool ObjectContainer::findObjectInit(SearchContext &ctx, const Common::UString &tag) const {
	ctx._object = 0;
	ctx._tag    = tag;
	ctx._range  = _objects.equal_range(tag);
	ctx._empty  = ctx._range.first == ctx._range.second;

	return !ctx._empty;
}

Object *ObjectContainer::findNextObject(SearchContext &ctx) const {
	if (ctx._empty || (ctx._range.first == ctx._range.second)) {
		ctx._empty  = true;
		ctx._object = 0;
		return 0;
	}

	ctx._object = ctx._range.first->second;

	++ctx._range.first;

	return ctx._object;
}

Object *ObjectContainer::findObject() const {
	if (_objects.empty())
		return 0;

	return _objects.begin()->second;
}

Object *ObjectContainer::findObject(const Common::UString &tag) const {
	SearchContext ctx;
	findObjectInit(ctx, tag);

	return findNextObject(ctx);
}

} // End of namespace NWScript

} // End of namespace Aurora
