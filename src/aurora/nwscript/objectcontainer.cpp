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

bool ObjectContainer::findFirstObject(SearchContext &ctx) const {
	ctx._empty  = false;
	ctx._object = 0;
	ctx._tag    = "";
	ctx._range  = std::make_pair(_objects.begin(), _objects.end());

	if (ctx._range.first == ctx._range.second)
		return false;

	ctx._object = ctx._range.first->second;
	return true;
}

bool ObjectContainer::findNextObject(SearchContext &ctx) const {
	return findNextObject(ctx._tag, ctx);
}

Object *ObjectContainer::findFirstObject() const {
	if (_objects.empty())
		return 0;

	return _objects.begin()->second;
}

bool ObjectContainer::findFirstObject(const Common::UString &tag, SearchContext &ctx) const {
	ctx._empty  = false;
	ctx._object = 0;
	ctx._tag    = tag;
	ctx._range  = _objects.equal_range(tag);

	if (ctx._range.first == ctx._range.second)
		return false;

	ctx._object = ctx._range.first->second;
	return true;
}

bool ObjectContainer::findNextObject(const Common::UString &tag, SearchContext &ctx) const {
	ctx._object = 0;

	if (ctx._empty)
		return false;

	if (tag != ctx._tag)
		throw Common::Exception("ObjectContainer::findNextObject(): Tags mismatch");

	if (++ctx._range.first == ctx._range.second) {
		ctx._empty  = true;
		ctx._object = 0;
		return false;
	}

	ctx._object = ctx._range.first->second;
	return true;
}

Object *ObjectContainer::findFirstObject(const Common::UString &tag) const {
	SearchContext ctx;
	findFirstObject(tag, ctx);

	return ctx._object;
}

} // End of namespace NWScript

} // End of namespace Aurora
