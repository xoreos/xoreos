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
 *  A variable used in the execution context.
 */

#include "src/aurora/actionscript/variable.h"
#include "src/aurora/actionscript/object.h"
#include "src/aurora/actionscript/function.h"

namespace Aurora {

namespace ActionScript {

Variable::Variable() : _type(kTypeUndefined) {
}

Variable::Variable(const Common::UString &value) : _type(kTypeString) {
	_value.string = value;
}

Variable::Variable(const char *value) : _type(kTypeString) {
	_value.string = value;
}

Variable::Variable(Object *value) : _type(kTypeObject) {
	_value.object = ObjectPtr(value);
}

Variable::Variable(double value) : _type(kTypeNumber) {
	_value.number = value;
}

Variable::Variable(unsigned int value) : _type(kTypeNumber) {
	_value.number = value;
}

Variable::Variable(unsigned long value) : _type(kTypeNumber) {
	_value.number = value;
}

Variable::Variable(bool value) : _type(kTypeBoolean) {
	_value.boolean = value;
}

Variable::Variable(ObjectPtr value) : _type(kTypeObject) {
	_value.object = value;
}

Variable::Variable(const Variable &variable) {
	_type = variable._type;

	switch (variable._type) {
		case kTypeObject:
			_value.object = variable._value.object;
			break;
		case kTypeString:
			_value.string = variable._value.string;
			break;
		case kTypeBoolean:
			_value.boolean = variable._value.boolean;
			break;
		case kTypeNumber:
			_value.number = variable._value.number;
			break;
		default:
			_type = variable._type;
	}
}

Variable::~Variable() {
}

bool Variable::isUndefined() const {
	return _type == kTypeUndefined;
}

bool Variable::isObject() const {
	return _type == kTypeObject;
}

bool Variable::isString() const {
	return _type == kTypeString;
}

bool Variable::isNumber() const {
	return _type == kTypeNumber;
}

bool Variable::isFunction() const {
	return _type == kTypeObject && dynamic_cast<ScriptedFunction *>(_value.object.get());
}

double Variable::asNumber() const {
	switch (_type) {
		case kTypeNumber:
			return _value.number;
		case kTypeBoolean:
			return _value.boolean ? 1 : 0;
		default:
			return 0;
	}
}

ObjectPtr Variable::asObject() {
	if (_type == kTypeUndefined) {
		_type = kTypeObject;
		_value.object = ObjectPtr(new Object());
	}
	return _value.object;
}

const Common::UString &Variable::asString() const {
	return _value.string;
}

bool Variable::asBoolean() const {
	if (_type == kTypeNumber)
		return _value.number != 0;
	else if (_type == kTypeBoolean)
		return _value.boolean;
	else if (_type == kTypeObject)
		return static_cast<bool>(_value.object.get());
	else
		return false;
}

void Variable::operator=(Variable v) {
	_type = v._type;
	switch (_type) {
		case kTypeString:
			_value.string = v._value.string;
			break;
		case kTypeObject:
			_value.object = v._value.object;
			break;
		case kTypeNumber:
			_value.number = v._value.number;
			break;
		case kTypeBoolean:
			_value.boolean = v._value.boolean;
			break;
		default:
			_type = v._type;
	}
}

bool Variable::operator!() {
	switch (_type) {
		case kTypeNumber:
			return !_value.number;
		case kTypeBoolean:
			return !_value.boolean;
		default:
			return true;
	}
}

Variable Variable::operator&&(Variable v) {
	if (v._type == kTypeNumber && _type == kTypeNumber)
		return v.asNumber() && asNumber();
	else
		return false;
}

Variable Variable::operator||(Variable v) {
	if (v._type == kTypeNumber && _type == kTypeNumber)
		return v.asNumber() || asNumber();
	else
		return false;
}

Variable Variable::operator==(Variable v) {
	if (_type != v._type)
		return false;

	switch (_type) {
		case kTypeNumber:
			return v._value.number == _value.number;
		default:
			return false;
	}
}

Variable Variable::operator<(Aurora::ActionScript::Variable v) {
	if (v._type == kTypeNumber && _type == kTypeNumber)
		return asNumber() < v.asNumber();
	else
		return false;
}

Variable Variable::operator-(Variable v) {
	if (v._type == kTypeNumber && _type == kTypeNumber)
		return asNumber() - v.asNumber();
	else
		return 0.0;
}

Variable Variable::operator+(Variable v) {
	if (v._type == kTypeNumber && _type == kTypeNumber)
		return asNumber() + v.asNumber();
	else
		return 0.0;
}

Variable Variable::operator*(Variable v) {
	return asNumber() * v.asNumber();
}

Variable Variable::operator/(Variable v) {
	return asNumber() / v.asNumber();
}

Variable Variable::operator++() {
	return asNumber() + 1;
}

} // End of namespace ActionScript

} // End of namespace Aurora
