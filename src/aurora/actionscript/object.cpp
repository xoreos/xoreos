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

std::vector<Common::UString> Object::getSlots() const {
	std::vector<Common::UString> slots;
	for (std::map<Common::UString, Variable>::const_iterator iter = _members.begin(); iter != _members.end() ; iter++) {
		slots.push_back(iter->first);
	}
	return slots;
}

bool Object::hasMember(const Common::UString &id) const {
	std::map<Common::UString, Variable>::const_iterator iter = _members.find("constructor");
	if (iter != _members.end())
		if (iter->second.asObject()->hasMember(id))
			return true;
	return _members.find(id) != _members.end();
}

Variable Object::getMember(const Variable &id) {
	if (!id.isString())
		throw Common::Exception("Object::getMember id is not a string");

	const Common::UString idString = id.asString();

	std::map<Common::UString, Variable>::iterator iter = _members.find("constructor");
	if (hasMember(idString) && iter != _members.end() && iter->second.asObject()->hasMember(idString))
		return iter->second.asObject()->getMember(id);
	else if (_members.find(idString) != _members.end())
		return _members[idString];
	else {
		_members.insert(std::make_pair(idString, ObjectPtr(new Object)));
		return _members[idString];
	}
}

void Object::setMember(const Variable &id, const Variable &value) {
	if (!id.isString())
		throw Common::Exception("Object::setMember id is not a string");

	_members[id.asString()] = value;
}

void Object::setMember(const Common::UString &id, Function *function) {
	_members[id] = ObjectPtr(function);
}

Variable Object::call(const Common::UString &function, AVM &avm, const std::vector<Variable> &arguments) {
	if (!hasMember(function))
		throw Common::Exception("object has no member %s", function.c_str());

	if (!getMember(function).isFunction())
		throw Common::Exception("%s is no method", function.c_str());

	Function *f = reinterpret_cast<Function *>(getMember(function).asObject().get());

	byte counter = 1;
	if (f->getPreloadRootFlag()) {
		avm.storeRegister(avm.getVariable("_root"), counter);
		counter += 1;
	}

	if (f->getPreloadThisFlag()) {
		avm.storeRegister(shared_from_this(), counter);
		counter += 1;
	}

	for (size_t i = 0; i < arguments.size(); ++i) {
		avm.storeRegister(arguments[i], counter);
		counter += 1;
	}

	avm.setReturnValue();
	(*f)(avm);
	return avm.getReturnValue();
}

} // End of namespace ActionScript

} // End of namespace Aurora
