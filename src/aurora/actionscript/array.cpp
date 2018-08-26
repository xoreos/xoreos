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
 *  Implementation for an actionscript Array class.
 */

#include <boost/bind.hpp>

#include "src/aurora/actionscript/array.h"

namespace Aurora {

namespace ActionScript {

static Variable as_push(AVM &avm){
	ArrayPtr array = avm.getRegister(1).as<Array>();
	if (!array)
		throw Common::Exception("Array::pop this is not an Array object");
	array->push(avm.getRegister(2));
	return Variable();
}

static Variable as_pop(AVM &avm) {
	ArrayPtr array = avm.getRegister(1).as<Array>();
	if (!array)
		throw Common::Exception("Array::pop this is not an Array object");
	return array->pop();
}

Array::Array(const std::list<Variable> &values) : _values(values) {
	setMember("push", new NativeFunction(boost::bind(as_push, _1), true, false));
	setMember("pop", new NativeFunction(boost::bind(as_pop, _1), true, false));
}

size_t Array::length() const {
	return _values.size();
}

void Array::push(const Variable &v) {
	_values.push_back(v);
}

Variable Array::pop() {
	Variable v = _values.back();
	_values.pop_back();
	return v;
}

Variable Array::getMember(const Variable &id) {
	if (id.isNumber()) {
		std::list<Variable>::iterator iter = _values.begin();
		std::advance(iter, static_cast<size_t>(id.asNumber()));
		return *iter;
	}

	if (id.isString() && id.asString() == "length")
		return Variable((unsigned long)_values.size());

	return Object::getMember(id);
}

void Array::setMember(const Variable &id, const Variable &value) {
	if (id.isNumber()) {
		std::list<Variable>::iterator iter = _values.begin();
		std::advance(iter, static_cast<size_t>(id.asNumber()));
		*iter = value;
		return;
	}

	Object::setMember(id, value);
}

} // End of namespace ActionScript

} // End of namespace Aurora
