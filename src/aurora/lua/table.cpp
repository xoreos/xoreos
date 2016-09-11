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
 *  A reference to a Lua table.
 */

#include "toluapp/tolua++.h"

#include "src/common/error.h"
#include "src/common/util.h"

#include "src/aurora/lua/table.h"
#include "src/aurora/lua/stack.h"
#include "src/aurora/lua/stackguard.h"
#include "src/aurora/lua/variable.h"
#include "src/aurora/lua/function.h"

namespace Aurora {

namespace Lua {

TableRef::TableRef() : _luaState(0),
	_ref(LUA_REFNIL), _metaAccessEnabled(true) {
}

TableRef::TableRef(const Stack &stack, int index) : _luaState(&stack.getLuaState()),
	_ref(LUA_REFNIL), _metaAccessEnabled(true) {

	lua_pushvalue(_luaState, index);
	_ref = lua_ref(_luaState, true);
}

TableRef::TableRef(lua_State &state, int index) : _luaState(&state),
	_ref(LUA_REFNIL), _metaAccessEnabled(true) {

	lua_pushvalue(_luaState, index);
	_ref = lua_ref(_luaState, true);
}

TableRef::TableRef(const TableRef &table) : _luaState(table._luaState),
	_ref(LUA_REFNIL), _metaAccessEnabled(table._metaAccessEnabled) {

	if (table._luaState && table._ref != LUA_REFNIL) {
		lua_getref(_luaState, table._ref);
		_ref = lua_ref(_luaState, true);
	} else {
		_luaState = 0;
	}
}

TableRef::~TableRef() {
	lua_unref(_luaState, _ref);
}

const TableRef& TableRef::operator=(const TableRef &table) {
	if (this == &table) {
		return *this;
	}

	assert(table._luaState && table._ref != LUA_REFNIL);

	lua_State *oldState = _luaState;
	const int oldRef = _ref;

	lua_getref(table._luaState, table._ref);
	_luaState = table._luaState;
	_ref = lua_ref(_luaState, true);
	_metaAccessEnabled = table._metaAccessEnabled;

	if (oldState && oldRef != LUA_REFNIL) {
		lua_unref(oldState, oldRef);
	}
	return *this;
}

int TableRef::getSize() const {
	StackGuard guard(*_luaState);

	pushSelf();

	int size = 0;
	const int top = lua_gettop(_luaState);
	for (lua_pushnil(_luaState); lua_next(_luaState, top); lua_pop(_luaState, 1)) {
		++size;
	}
	return size;
}

Common::UString TableRef::getExactType() const {
	StackGuard guard(*_luaState);

	pushSelf();

	Stack stack(*_luaState);
	return stack.getExactTypeAt(-1);
}

TableRef TableRef::getMetaTable() const {
	StackGuard guard(*_luaState);

	pushSelf();
	lua_getmetatable(_luaState, -1);

	Stack stack(*_luaState);
	return stack.getTableAt(-1);
}

void TableRef::setMetaAccessEnabled(bool enabled) {
	_metaAccessEnabled = enabled;
}

bool TableRef::isMetaAccessEnabled() const {
	return _metaAccessEnabled;
}

void TableRef::removeAt(int index) {
	StackGuard guard(*_luaState);

	pushSelf();

	lua_pushnil(_luaState);
	lua_rawseti(_luaState, -2, index);
}

void TableRef::removeAt(const Common::UString &key) {
	StackGuard guard(*_luaState);

	pushSelf();

	lua_pushstring(_luaState, key.c_str());
	lua_pushnil(_luaState);

	if (_metaAccessEnabled) {
		lua_settable(_luaState, -3);
	} else {
		lua_rawset(_luaState, -3);
	}
}

void TableRef::setBooleanAt(int index, bool value) {
	setVariableAt(index, value);
}

void TableRef::setBooleanAt(const Common::UString &key, bool value) {
	setVariableAt(key, value);
}

void TableRef::setFloatAt(int index, float value) {
	setVariableAt(index, value);
}

void TableRef::setFloatAt(const Common::UString &key, float value) {
	setVariableAt(key, value);
}

void TableRef::setIntAt(int index, int value) {
	setVariableAt(index, value);
}

void TableRef::setIntAt(const Common::UString &key, int value) {
	setVariableAt(key, value);
}

void TableRef::setStringAt(int index, const char *value) {
	setVariableAt(index, value);
}

void TableRef::setStringAt(const Common::UString &key, const char *value) {
	setVariableAt(key, value);
}

void TableRef::setStringAt(int index, const Common::UString &value) {
	setVariableAt(index, value);
}

void TableRef::setStringAt(const Common::UString &key, const Common::UString &value) {
	setVariableAt(key, value);
}

void TableRef::setTableAt(int index, const TableRef &value) {
	setVariableAt(index, value);
}

void TableRef::setTableAt(const Common::UString &key, const TableRef &value) {
	setVariableAt(key, value);
}

void TableRef::setFunctionAt(int index, const FunctionRef &value) {
	setVariableAt(index, value);
}

void TableRef::setFunctionAt(const Common::UString &key, const FunctionRef &value) {
	setVariableAt(key, value);
}

void TableRef::setUserTypeAt(int index, void *value, const Common::UString &type) {
	setVariableAt(index, Variable(value, type));
}

void TableRef::setUserTypeAt(const Common::UString &key, void *value, const Common::UString &type) {
	setVariableAt(key, Variable(value, type));
}

void TableRef::setVariableAt(int index, const Variable &value) {
	StackGuard guard(*_luaState);

	pushSelf();

	Stack stack(*_luaState);
	stack.pushVariable(value);
	lua_rawseti(_luaState, -2, index);
}

void TableRef::setVariableAt(const Common::UString &key, const Variable &value) {
	StackGuard guard(*_luaState);

	pushSelf();
	lua_pushstring(_luaState, key.c_str());

	Stack stack(*_luaState);
	stack.pushVariable(value);

	if (_metaAccessEnabled) {
		lua_settable(_luaState, -3);
	} else {
		lua_rawset(_luaState, -3);
	}
}

bool TableRef::getBooleanAt(int index) const {
	if (!isBooleanAt(index)) {
		throw Common::Exception("Failed get a boolean value from the table (index: %d)", index);
	}

	return getVariableAt(index).getBool();
}

bool TableRef::getBooleanAt(const Common::UString &key) const {
	if (!isBooleanAt(key)) {
		throw Common::Exception("Failed get a boolean value from the table (key: %s)", key.c_str());
	}

	return getVariableAt(key).getBool();
}

float TableRef::getFloatAt(int index) const {
	if (!isNumberAt(index)) {
		throw Common::Exception("Failed get a float value from the table (index: %d)", index);
	}

	return getVariableAt(index).getFloat();
}

float TableRef::getFloatAt(const Common::UString &key) const {
	if (!isNumberAt(key)) {
		throw Common::Exception("Failed get a float value from the table (key: %s)", key.c_str());
	}

	return getVariableAt(key).getFloat();
}

int TableRef::getIntAt(int index) const {
	if (!isNumberAt(index)) {
		throw Common::Exception("Failed get an integer value from the table (index: %d)", index);
	}

	return getVariableAt(index).getInt();
}

int TableRef::getIntAt(const Common::UString &key) const {
	if (!isNumberAt(key)) {
		throw Common::Exception("Failed get an integer value from the table (key: %s)", key.c_str());
	}

	return getVariableAt(key).getInt();
}

Common::UString TableRef::getStringAt(int index) const {
	if (!isStringAt(index)) {
		throw Common::Exception("Failed get a string value from the table (index: %d)", index);
	}

	return getVariableAt(index).getString();
}

Common::UString TableRef::getStringAt(const Common::UString &key) const {
	if (!isStringAt(key)) {
		throw Common::Exception("Failed get a string value from the table (key: %s)", key.c_str());
	}

	return getVariableAt(key).getString();
}

TableRef TableRef::getTableAt(int index) const {
	if (!isTableAt(index)) {
		throw Common::Exception("Failed get a table value from the table (index: %d)", index);
	}

	return getVariableAt(index).getTable();
}

TableRef TableRef::getTableAt(const Common::UString &key) const {
	if (!isTableAt(key)) {
		throw Common::Exception("Failed get a table value from the table (key: %s)", key.c_str());
	}

	return getVariableAt(key).getTable();
}

FunctionRef TableRef::getFunctionAt(int index) const {
	if (!isFunctionAt(index)) {
		throw Common::Exception("Failed get a function value from the table (index: %d)", index);
	}

	return getVariableAt(index).getFunction();
}

FunctionRef TableRef::getFunctionAt(const Common::UString &key) const {
	if (!isFunctionAt(key)) {
		throw Common::Exception("Failed get a function value from the table (key: %s)", key.c_str());
	}

	return getVariableAt(key).getFunction();
}

void *TableRef::getRawUserTypeAt(int index, const Common::UString &type) const {
	if (!isUserTypeAt(index, type)) {
		throw Common::Exception("Failed get a usertype value from the table (index: %d)", index);
	}

	return getVariableAt(index).getRawUserType();
}

void *TableRef::getRawUserTypeAt(const Common::UString &key, const Common::UString &type) const {
	if (!isUserTypeAt(key, type)) {
		throw Common::Exception("Failed get a usertype value from the table (key: %s)", key.c_str());
	}

	return getVariableAt(key).getRawUserType();
}

Variable TableRef::getVariableAt(int index) const {
	StackGuard guard(*_luaState);

	pushSelf();
	lua_rawgeti(_luaState, -1, index);

	Stack stack(*_luaState);
	return stack.getVariableAt(-1);
}

Variable TableRef::getVariableAt(const Common::UString &key) const {
	StackGuard guard(*_luaState);

	pushSelf();
	lua_pushstring(_luaState, key.c_str());

	if (_metaAccessEnabled) {
		lua_gettable(_luaState, -2);
	} else {
		lua_rawget(_luaState, -2);
	}

	Stack stack(*_luaState);
	return stack.getVariableAt(-1);
}

Common::UString TableRef::getExactTypeAt(int index) const {
	StackGuard guard(*_luaState);

	pushSelf();
	lua_rawgeti(_luaState, -1, index);

	Stack stack(*_luaState);
	return stack.getExactTypeAt(-1);
}

Common::UString TableRef::getExactTypeAt(const Common::UString &key) const {
	StackGuard guard(*_luaState);

	pushSelf();
	lua_pushstring(_luaState, key.c_str());

	if (_metaAccessEnabled) {
		lua_gettable(_luaState, -2);
	} else {
		lua_rawget(_luaState, -2);
	}

	Stack stack(*_luaState);
	return stack.getExactTypeAt(-1);
}

Type TableRef::getTypeAt(int index) const {
	StackGuard guard(*_luaState);

	pushSelf();
	lua_rawgeti(_luaState, -1, index);

	Stack stack(*_luaState);
	return stack.getTypeAt(-1);
}

Type TableRef::getTypeAt(const Common::UString &key) const {
	StackGuard guard(*_luaState);

	pushSelf();
	lua_pushstring(_luaState, key.c_str());

	if (_metaAccessEnabled) {
		lua_gettable(_luaState, -2);
	} else {
		lua_rawget(_luaState, -2);
	}

	Stack stack(*_luaState);
	return stack.getTypeAt(-1);
}

Variables TableRef::getVariables() const {
	StackGuard guard(*_luaState);

	pushSelf();

	Variables vars;
	const int top = lua_gettop(_luaState);
	for (lua_pushnil(_luaState); lua_next(_luaState, top); lua_pop(_luaState, 1)) {
		Stack stack(*_luaState);
		vars.push_back(stack.getVariableAt(-1));
	}
	return vars;
}

bool TableRef::isNilAt(int index) const {
	return getTypeAt(index) == kTypeNil;
}

bool TableRef::isNilAt(const Common::UString &key) const {
	return getTypeAt(key) == kTypeNil;
}

bool TableRef::isBooleanAt(int index) const {
	return getTypeAt(index) == kTypeBoolean;
}

bool TableRef::isBooleanAt(const Common::UString &key) const {
	return getTypeAt(key) == kTypeBoolean;
}

bool TableRef::isNumberAt(int index) const {
	return getTypeAt(index) == kTypeNumber;
}

bool TableRef::isNumberAt(const Common::UString &key) const {
	return getTypeAt(key) == kTypeNumber;
}

bool TableRef::isStringAt(int index) const {
	return getTypeAt(index) == kTypeString;
}

bool TableRef::isStringAt(const Common::UString &key) const {
	return getTypeAt(key) == kTypeString;
}

bool TableRef::isTableAt(int index) const {
	return getTypeAt(index) == kTypeTable;
}

bool TableRef::isTableAt(const Common::UString &key) const {
	return getTypeAt(key) == kTypeTable;
}

bool TableRef::isFunctionAt(int index) const {
	return getTypeAt(index) == kTypeFunction;
}

bool TableRef::isFunctionAt(const Common::UString &key) const {
	return getTypeAt(key) == kTypeFunction;
}

bool TableRef::isUserTypeAt(int index, const Common::UString &type) const {
	const bool isUT = getTypeAt(index) == kTypeUserType;
	if (type.empty()) {
		return isUT;
	}
	return isUT && getExactTypeAt(index) == type;
}

bool TableRef::isUserTypeAt(const Common::UString &key, const Common::UString &type) const {
	const bool isUT = getTypeAt(key) == kTypeUserType;
	if (type.empty()) {
		return isUT;
	}
	return isUT && getExactTypeAt(key) == type;
}

lua_State &TableRef::getLuaState() const {
	assert(_luaState);

	return *_luaState;
}

int TableRef::getRef() const {
	return _ref;
}

void TableRef::pushSelf() const {
	assert(_luaState && _ref != LUA_REFNIL);

	lua_getref(_luaState, _ref);
}

} // End of namespace Lua

} // End of namespace Aurora
