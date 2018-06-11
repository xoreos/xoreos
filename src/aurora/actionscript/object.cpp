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
 *  Abstract object which is inherited by every other class.
 */

#include <boost/weak_ptr.hpp>

#include "src/common/error.h"

#include "src/aurora/actionscript/object.h"
#include "src/aurora/actionscript/avm.h"
#include "src/aurora/actionscript/function.h"

namespace Aurora {

namespace ActionScript {

Object::Object() {
}

Object::Object(Object *object) {
	_members = object->_members;
}

Object::~Object() {
}

bool Object::hasMember(Common::UString id) {
	return _members.find(id) != _members.end();
}

Variable Object::getMember(Common::UString id) {
	if (_members.find(id) != _members.end())
		return _members[id];
	else {
		_members.insert(std::make_pair(id, ObjectPtr(new Object)));
		return _members[id];
	}
}

void Object::setMember(Common::UString id, Variable member) {
	_members[id] = member;
}

Variable Object::call(Common::UString function, AVM &avm) {
	if (!hasMember(function))
		throw Common::Exception("object has no member %s", function.c_str());

	if (!getMember(function).isFunction())
		throw Common::Exception("%s is no method", function.c_str());

	Function *f = reinterpret_cast<Function *>(getMember(function).asObject().get());

	byte counter = 1;
	if (f->preloadThisFlag()) {
		avm.storeRegister(shared_from_this(), counter);
		counter += 1;
	}

	avm.setReturnValue();
	(*f)(avm);
	return avm.getReturnValue();
}

} // End of namespace ActionScript

} // End of namespace Aurora
