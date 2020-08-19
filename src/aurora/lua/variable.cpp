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
 *  Lua variable.
 */

#include <cassert>

#include "src/common/error.h"

#include "src/aurora/lua/variable.h"
#include "src/aurora/lua/table.h"
#include "src/aurora/lua/function.h"

namespace Aurora {

namespace Lua {

Variable::Variable(Type type, const Common::UString &exactType) : _type(kTypeNone) {
	setType(type, exactType);
}

Variable::Variable(bool value) : _type(kTypeNone) {
	setType(kTypeBoolean);
	*this = value;
}

Variable::Variable(int32_t value) : _type(kTypeNone) {
	setType(kTypeNumber);
	*this = value;
}

Variable::Variable(float value) : _type(kTypeNone) {
	setType(kTypeNumber);
	*this = value;
}

Variable::Variable(const char *value) : _type(kTypeNone) {
	setType(kTypeString);
	*this = value;
}

Variable::Variable(const Common::UString &value) : _type(kTypeNone) {
	setType(kTypeString);
	*this = value;
}

Variable::Variable(const TableRef &value) : _type(kTypeNone) {
	setType(kTypeTable);
	*this = value;
}

Variable::Variable(const FunctionRef &value) : _type(kTypeNone) {
	setType(kTypeFunction);
	*this = value;
}

Variable::Variable(void *value, const Common::UString &exactType) : _type(kTypeNone) {
	assert(!exactType.empty());

	setType(kTypeUserType, exactType);
	*this = value;
}

Variable::Variable(const Variable &var) : _type(kTypeNone) {
	*this = var;
}

Variable::~Variable() {
	try {
		setType(kTypeNone);
	} catch (...) {
	}
}

void Variable::setType(Type type, const Common::UString &exactType) {
	switch (_type) {
		case kTypeString:
			delete _value._string;
			_value._string = 0;
			break;
		case kTypeTable:
			delete _value._table;
			_value._table = 0;
			break;
		case kTypeFunction:
			delete _value._function;
			_value._function = 0;
			break;
		default:
			break;
	}

	_type = type;

	switch (_type) {
		case kTypeNone:
			_exactType = "none";
			break;
		case kTypeNil:
			_exactType = "nil";
			break;
		case kTypeBoolean:
			_value._bool = false;
			_exactType = "boolean";
			break;
		case kTypeNumber:
			_value._float = 0.0f;
			_exactType = "number";
			break;
		case kTypeString:
			_value._string = new Common::UString();
			_exactType = "string";
			break;
		case kTypeTable:
			_value._table = new TableRef();
			_exactType = "table";
			break;
		case kTypeFunction:
			_value._function = new FunctionRef();
			_exactType = "function";
			break;
		case kTypeUserType:
			assert(!exactType.empty());

			_value._data = 0;
			_exactType = exactType;
			break;
		default:
			throw Common::Exception("Variable::setType(): Invalid type %d", type);
			break;
	}
}

Variable &Variable::operator=(const Variable &var) {
	if (this == &var) {
		return *this;
	}

	setType(var._type, var._exactType);

	switch (_type) {
		case kTypeString:
			*_value._string = *var._value._string;
			break;
		case kTypeTable:
			*_value._table = *var._value._table;
			_exactType = _value._table->getExactType();
			break;
		case kTypeFunction:
			*_value._function = *var._value._function;
			break;
		default:
			_value = var._value;
			break;
	}
	return *this;
}

Variable &Variable::operator=(bool value) {
	if (_type != kTypeBoolean) {
		throw Common::Exception("Can't assign a boolean value to a non-boolean variable");
	}

	_value._bool = value;
	return *this;
}

Variable &Variable::operator=(int32_t value) {
	if (_type != kTypeNumber) {
		throw Common::Exception("Can't assign an int value to a non-number variable");
	}

	_value._float = value;
	return *this;
}

Variable &Variable::operator=(float value) {
	if (_type != kTypeNumber) {
		throw Common::Exception("Can't assign a float value to a non-number variable");
	}

	_value._float = value;
	return *this;
}

Variable &Variable::operator=(const char *value) {
	if (_type != kTypeString) {
		throw Common::Exception("Can't assign a string value to a non-string variable");
	}

	*_value._string = value;
	return *this;
}

Variable &Variable::operator=(const Common::UString &value) {
	if (_type != kTypeString) {
		throw Common::Exception("Can't assign a string value to a non-string variable");
	}

	*_value._string = value;
	return *this;
}

Variable &Variable::operator=(const TableRef &value) {
	if (_type != kTypeTable) {
		throw Common::Exception("Can't assign a table value to a non-table variable");
	}

	*_value._table = value;
	_exactType = _value._table->getExactType();
	return *this;
}

Variable &Variable::operator=(const FunctionRef &value) {
	if (_type != kTypeFunction) {
		throw Common::Exception("Can't assign a function value to a non-function variable");
	}

	*_value._function = value;
	return *this;
}

Variable &Variable::operator=(void *value) {
	if (_type != kTypeUserType) {
		throw Common::Exception("Can't assign a raw value to a non-usertype variable");
	}

	_value._data = value;
	return *this;
}

bool Variable::operator==(const Variable &var) const {
	if (this == &var) {
		return true;
	}

	if (_type != var._type) {
		return false;
	}

	switch (_type) {
		case kTypeNil:
			return true;
		case kTypeBoolean:
			return _value._bool == var._value._bool;
		case kTypeNumber:
			return _value._float == var._value._float;
		case kTypeString:
			return *_value._string == *var._value._string;
		case kTypeTable:
		case kTypeFunction:
			return false; // TODO
		case kTypeUserType:
			return _value._data == var._value._data;
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

const Common::UString &Variable::getExactType() const {
	return _exactType;
}

bool Variable::getBool() const {
	if (_type != kTypeBoolean) {
		throw Common::Exception("Can't get a boolean value from a non-boolean variable");
	}

	return _value._bool;
}

int32_t Variable::getInt() const {
	if (_type != kTypeNumber) {
		throw Common::Exception("Can't get an int value from a non-number variable");
	}

	return _value._float;
}

float Variable::getFloat() const {
	if (_type != kTypeNumber) {
		throw Common::Exception("Can't get a float value from a non-number variable");
	}

	return _value._float;
}

const Common::UString &Variable::getString() const {
	if (_type != kTypeString) {
		throw Common::Exception("Can't get a string value from a non-string variable");
	}

	return *_value._string;
}

Common::UString &Variable::getString() {
	if (_type != kTypeString) {
		throw Common::Exception("Can't get a string value from a non-string variable");
	}

	return *_value._string;
}

TableRef &Variable::getTable() {
	if (_type != kTypeTable) {
		throw Common::Exception("Can't get a table value from a non-table variable");
	}

	return *_value._table;
}

const TableRef &Variable::getTable() const {
	if (_type != kTypeTable) {
		throw Common::Exception("Can't get a table value from a non-table variable");
	}

	return *_value._table;
}

const FunctionRef &Variable::getFunction() const {
	if (_type != kTypeFunction) {
		throw Common::Exception("Can't get a function value from a non-function variable");
	}

	return *_value._function;
}

void *Variable::getRawUserType() const {
	if (_type != kTypeUserType) {
		throw Common::Exception("Can't get a raw usertype value from a non-usertype variable");
	}

	return _value._data;
}

} // End of namespace Lua

} // End of namespace Aurora
