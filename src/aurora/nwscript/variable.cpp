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
 *  NWScript variable.
 */

#include <cassert>

#include <boost/make_shared.hpp>

#include "src/common/error.h"
#include "src/common/ustring.h"

#include "src/aurora/nwscript/variable.h"
#include "src/aurora/nwscript/enginetype.h"
#include "src/aurora/nwscript/objectref.h"

namespace Aurora {

namespace NWScript {

Variable::Variable(Type type) : _type(kTypeVoid) {
	setType(type);
}

Variable::Variable(int32_t value) : _type(kTypeVoid) {
	setType(kTypeInt);

	*this = value;
}

Variable::Variable(float value) : _type(kTypeVoid) {
	setType(kTypeFloat);

	*this = value;
}

Variable::Variable(const Common::UString &value) : _type(kTypeVoid) {
	setType(kTypeString);

	*this = value;
}

Variable::Variable(Object *value) : _type(kTypeVoid) {
	setType(kTypeObject);

	*this = value;
}

Variable::Variable(const ObjectReference &value) : _type(kTypeVoid) {
	setType(kTypeObject);

	*this = value;
}

Variable::Variable(const EngineType *value) : _type(kTypeVoid) {
	setType(kTypeEngineType);

	*this = value;
}

Variable::Variable(const EngineType &value) : _type(kTypeVoid) {
	setType(kTypeEngineType);

	*this = value;
}

Variable::Variable(float x, float y, float z) : _type(kTypeVoid) {
	setType(kTypeVector);

	setVector(x, y, z);
}

Variable::Variable(const Variable &var) : _type(kTypeVoid) {
	*this = var;
}

Variable::~Variable() {
	try {
		setType(kTypeVoid);
	} catch (...) {
	}
}

void Variable::setType(Type type) {
	_array.reset();

	if      (_type == kTypeString)
		delete _value._string;
	else if (_type == kTypeObject)
		delete _value._object;
	else if (_type == kTypeEngineType)
		delete _value._engineType;
	else if (_type == kTypeScriptState)
		delete _value._scriptState;

	_type = type;

	switch (_type) {
		case kTypeVoid:
		case kTypeAny:
			break;

		case kTypeArray:
			_array = boost::make_shared<Array>();
			break;

		case kTypeInt:
			_value._int = 0;
			break;

		case kTypeFloat:
			_value._float = 0.0f;
			break;

		case kTypeString:
			_value._string = new Common::UString;
			break;

		case kTypeObject:
			_value._object = new ObjectReference;
			break;

		case kTypeVector:
			_value._vector[0] = 0.0f;
			_value._vector[1] = 0.0f;
			_value._vector[2] = 0.0f;
			break;

		case kTypeEngineType:
			_value._engineType = 0;
			break;

		case kTypeScriptState:
			_value._scriptState = new ScriptState;
			break;

		case kTypeReference:
			_value._reference = 0;
			break;

		default:
			throw Common::Exception("Variable::setType(): Invalid type %d", type);
			break;
	}
}

Variable &Variable::operator=(const Variable &var) {
	if (&var == this)
		return *this;

	setType(var._type);

	if      (_type == kTypeString)
		*_value._string = *var._value._string;
	else if (_type == kTypeObject)
		*_value._object = *var._value._object;
	else if (_type == kTypeEngineType)
		*this = var._value._engineType;
	else if (_type == kTypeScriptState)
		*_value._scriptState = *var._value._scriptState;
	else if (_type == kTypeArray)
		_array = var._array;
	else
		_value = var._value;

	return *this;
}

Variable &Variable::operator=(int32_t value) {
	if (_type != kTypeInt)
		throw Common::Exception("Can't assign an int value to a non-int variable");

	_value._int = value;

	return *this;
}

Variable &Variable::operator=(float value) {
	if (_type != kTypeFloat)
		throw Common::Exception("Can't assign a float value to a non-float variable");

	_value._float = value;

	return *this;
}

Variable &Variable::operator=(const Common::UString &value) {
	if (_type != kTypeString)
		throw Common::Exception("Can't assign a string value to a non-string variable");

	*_value._string = value;

	return *this;
}

Variable &Variable::operator=(Object *value) {
	if (_type != kTypeObject)
		throw Common::Exception("Can't assign an object value to a non-object variable");

	*_value._object = value;

	return *this;
}

Variable &Variable::operator=(const ObjectReference &value) {
	if (_type != kTypeObject)
		throw Common::Exception("Can't assign an object value to a non-object variable");

	*_value._object = value;

	return *this;
}

Variable &Variable::operator=(const EngineType *value) {
	if (_type != kTypeEngineType)
		throw Common::Exception("Can't assign an engine-type value to a non-engine-type variable");

	EngineType *engineType = value ? value->clone() : 0;

	delete _value._engineType;

	_value._engineType = engineType;

	return *this;
}

Variable &Variable::operator=(const EngineType &value) {
	*this = &value;

	return *this;
}

bool Variable::operator==(const Variable &var) const {
	if (this == &var)
		return true;

	if (_type != var._type)
		return false;

	switch (_type) {
		case kTypeVoid:
			return true;

		case kTypeInt:
			return _value._int == var._value._int;

		case kTypeFloat:
			return _value._float == var._value._float;

		case kTypeString:
			return *_value._string == *var._value._string;

		case kTypeObject:
			return *_value._object == *var._value._object;

		case kTypeVector:
			return _value._vector[0] == var._value._vector[0] &&
			       _value._vector[1] == var._value._vector[1] &&
			       _value._vector[2] == var._value._vector[2];

		case kTypeArray:
			return _array.get() && var._array.get() && *_array == *var._array;

		default:
			break;
	}

	return false;
}

bool Variable::operator!=(const Variable &var) const {
	return !(*this == var);
}

Type Variable::getType() const {
	return _type;
}

int32_t Variable::getInt() const {
	if (_type != kTypeInt)
		throw Common::Exception("Can't get an int value from a non-int variable");

	return _value._int;
}

float Variable::getFloat() const {
	if (_type != kTypeFloat)
		throw Common::Exception("Can't get a float value from a non-float variable");

	return _value._float;
}

const Common::UString &Variable::getString() const {
	if (_type != kTypeString)
		throw Common::Exception("Can't get a string value from a non-string variable");

	return *_value._string;
}

Common::UString &Variable::getString() {
	if (_type != kTypeString)
		throw Common::Exception("Can't get a string value from a non-string variable");

	return *_value._string;
}

Object *Variable::getObject() const {
	if (_type != kTypeObject)
		throw Common::Exception("Can't get an object value from a non-object variable");

	return **_value._object;
}

EngineType *Variable::getEngineType() const {
	if (_type != kTypeEngineType)
		throw Common::Exception("Can't get an engine-type value from a non-engine-type variable");

	return _value._engineType;
}

void Variable::setVector(float x, float y, float z) {
	if (_type != kTypeVector)
		throw Common::Exception("Can't assign a vector value to a non-vector variable");

	_value._vector[0] = x;
	_value._vector[1] = y;
	_value._vector[2] = z;
}

void Variable::getVector(float &x, float &y, float &z) const {
	if (_type != kTypeVector)
		throw Common::Exception("Can't get a vector value from a non-vector variable");

	x = _value._vector[0];
	y = _value._vector[1];
	z = _value._vector[2];
}

const Variable::Array &Variable::getArray() const {
	if (_type != kTypeArray)
		throw Common::Exception("Can't get an array value from a non-array variable");

	assert(_array.get());

	return *_array;
}

Variable::Array &Variable::getArray() {
	if (_type != kTypeArray)
		throw Common::Exception("Can't get an array value from a non-array variable");

	assert(_array.get());

	return *_array;
}

size_t Variable::getArraySize() const {
	if (_type != kTypeArray)
		throw Common::Exception("Can't get an array size from a non-array variable");

	assert(_array.get());

	return _array->size();
}

void Variable::growArray(Type type, size_t size) {
	if (_type != kTypeArray)
		throw Common::Exception("Can't grow a non-array variable");

	assert(_array.get());

	if (!_array->empty() && (*_array)[0].get() && (*_array)[0]->getType() != type)
		throw Common::Exception("Array type mismatch (%d vs %d)", (*_array)[0]->getType(), type);

	_array->reserve(size);
	while (_array->size() < size)
		_array->push_back(boost::make_shared<Variable>(Variable(type)));
}

ScriptState &Variable::getScriptState() {
	if (_type != kTypeScriptState)
		throw Common::Exception("Can't get a script state value from a non-script-state variable");

	return *_value._scriptState;
}

const ScriptState &Variable::getScriptState() const {
	if (_type != kTypeScriptState)
		throw Common::Exception("Can't get a script state value from a non-script-state variable");

	return *_value._scriptState;
}

Variable *Variable::getReference() const {
	if (_type != kTypeReference)
		throw Common::Exception("Can't get a reference value from a non-reference variable");

	return _value._reference;
}

void Variable::setReference(Variable *reference) {
	if (_type != kTypeReference)
		throw Common::Exception("Can't assign a reference value to a non-reference variable");

	_value._reference = reference;
}

} // End of namespace NWScript

} // End of namespace Aurora
