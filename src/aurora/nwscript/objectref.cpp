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
 *  Reference to an NWScript object.
 */

#include "src/aurora/nwscript/objectref.h"
#include "src/aurora/nwscript/object.h"
#include "src/aurora/nwscript/objectman.h"

namespace Aurora {

namespace NWScript {

ObjectReference::ObjectReference(const Object *object) : _id(object ? object->getID() : kObjectIDInvalid) {
}

uint32_t ObjectReference::getId() const {
	return _id;
}

Object *ObjectReference::operator*() const {
	if (_id == kObjectIDInvalid)
		return 0;

	return ObjectMan.findObject(_id);
}

ObjectReference &ObjectReference::operator=(const Object *object) {
	_id = object ? object->getID() : kObjectIDInvalid;
	return *this;
}

bool ObjectReference::operator==(const ObjectReference &objref) {
	return _id == objref._id;
}

} // End of namespace NWScript

} // End of namespace Aurora
