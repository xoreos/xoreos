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
 *  A Lua stack wrapper.
 */

#include "toluapp/tolua++.h"

#include "src/common/error.h"
#include "src/common/ustring.h"
#include "src/common/util.h"

#include "src/aurora/lua/stack.h"
#include "src/aurora/lua/variable.h"
#include "src/aurora/lua/table.h"
#include "src/aurora/lua/function.h"

namespace Aurora {

namespace Lua {

Stack::Stack(lua_State &state) : _luaState(state) {

}

Stack::~Stack() {

}

int Stack::getSize() const {
	return lua_gettop(&_luaState);
}

void Stack::pushNil() {
	lua_pushnil(&_luaState);
}

void Stack::pushBoolean(bool value) {
	lua_pushboolean(&_luaState, value ? 1 : 0);
}

void Stack::pushFloat(float value) {
	lua_pushnumber(&_luaState, value);
}

void Stack::pushInt(int value) {
	lua_pushnumber(&_luaState, value);
}

void Stack::pushString(const char *value) {
	lua_pushstring(&_luaState, value);
}

void Stack::pushString(const Common::UString &value) {
	lua_pushstring(&_luaState, value.c_str());
}

void Stack::pushTable(const TableRef &value) {
	const int ref = value.getRef();
	if (ref != LUA_REFNIL) {
		lua_getref(&_luaState, ref);
	} else {
		lua_pushnil(&_luaState);
	}
}

void Stack::pushFunction(const FunctionRef &value) {
	const int ref = value.getRef();
	if (ref != LUA_REFNIL) {
		lua_getref(&_luaState, ref);
	} else {
		lua_pushnil(&_luaState);
	}
}

void Stack::pushRawUserType(void *value, const Common::UString &type) {
	tolua_pushusertype(&_luaState, value, type.c_str());
}

void Stack::pushVariable(const Variable &var) {
	switch (var.getType()) {
		case kTypeNil:
			pushNil();
			break;
		case kTypeBoolean:
			pushBoolean(var.getBool());
			break;
		case kTypeNumber:
			pushFloat(var.getFloat());
			break;
		case kTypeString:
			pushString(var.getString());
			break;
		case kTypeTable:
			pushTable(var.getTable());
			break;
		case kTypeFunction:
			pushFunction(var.getFunction());
			break;
		case kTypeUserType:
			pushRawUserType(var.getRawUserType(), var.getExactType());
			break;
		default:
			warning("Pushing a varible of type \"%s\" not supported",
			        var.getExactType().c_str());
			break;
	}
}

void Stack::pushVariables(const Variables &vars) {
	for (Variables::const_iterator v = vars.begin(); v != vars.end(); ++v) {
		pushVariable(*v);
	}
}

bool Stack::getBooleanAt(int index) const {
	if (!isBooleanAt(index)) {
		throw Common::Exception("Failed to get a boolean value from the Lua stack (index: %d)", index);
	}
	return lua_toboolean(&_luaState, index) != 0;
}

float Stack::getFloatAt(int index) const {
	if (!isNumberAt(index)) {
		throw Common::Exception("Failed to get a number from the Lua stack (index: %d)", index);
	}
	return lua_tonumber(&_luaState, index);
}

int Stack::getIntAt(int index) const {
	if (!isNumberAt(index)) {
		throw Common::Exception("Failed to get a number from the Lua stack (index: %d)", index);
	}
	return lua_tonumber(&_luaState, index);
}

Common::UString Stack::getStringAt(int index) const {
	if (!isStringAt(index)) {
		throw Common::Exception("Failed to get a string from the Lua stack (index: %d)", index);
	}
	return lua_tostring(&_luaState, index);
}

TableRef Stack::getTableAt(int index) const {
	if (!isTableAt(index)) {
		throw Common::Exception("Failed to get a table from the Lua stack (index: %d)", index);
	}
	return TableRef(_luaState, index);
}

FunctionRef Stack::getFunctionAt(int index) const {
	if (!isFunctionAt(index)) {
		throw Common::Exception("Failed to get a function from the Lua stack (index: %d)", index);
	}
	return FunctionRef(_luaState, index);
}

void *Stack::getRawUserTypeAt(int index, const Common::UString &type) const {
	if (!isUserTypeAt(index, type)) {
		const char *msg = "Failed to get a usertype value from the Lua stack (type: %s, index: %d)";
		throw Common::Exception(msg, type.c_str(), index);
	}

	return tolua_tousertype(&_luaState, index, 0);
}

Variable Stack::getVariableAt(int index) const {
	switch (getTypeAt(index)) {
		case kTypeNil:
			return Variable(kTypeNil);
		case kTypeBoolean:
			return getBooleanAt(index);
		case kTypeNumber:
			return getFloatAt(index);
		case kTypeString:
			return getStringAt(index);
		case kTypeTable:
			return getTableAt(index);
		case kTypeFunction:
			return getFunctionAt(index);
		case kTypeUserType: {
			const Common::UString exactType = getExactTypeAt(index);
			return Variable(getRawUserTypeAt(index, exactType), exactType);
		}
		default:
			return Variable(kTypeNone);
	}
}

Common::UString Stack::getExactTypeAt(int index) const {
	if (!checkIndex(index)) {
		return "none";
	}

	const Common::UString type = tolua_typename(&_luaState, index);
	lua_pop(&_luaState, 1);
	return type;
}

Type Stack::getTypeAt(int index) const {
	if (!checkIndex(index)) {
		return kTypeNone;
	}

	switch (lua_type(&_luaState, index)) {
		case LUA_TNONE:
			return kTypeNone;
		case LUA_TNIL:
			return kTypeNil;
		case LUA_TBOOLEAN:
			return kTypeBoolean;
		case LUA_TNUMBER:
			return kTypeNumber;
		case LUA_TSTRING:
			return kTypeString;
		case LUA_TTABLE:
			return kTypeTable;
		case LUA_TFUNCTION:
			return kTypeFunction;
		case LUA_TUSERDATA:
			return kTypeUserType;
		default:
			warning("Unhandled Lua type: %s", lua_typename(&_luaState, index));
			return kTypeNone;
	}
}

Variables Stack::getVariables() const {
	Variables vars;
	for (int i = 1; i <= getSize(); ++i) {
		vars.push_back(getVariableAt(i));
	}
	return vars;
}

Aurora::Lua::Variables Aurora::Lua::Stack::getVariablesFromTop(int count) const {
	Variables vars;
	const int start = std::max(0, getSize() - count) + 1;
	for (int i = start; i <= getSize(); ++i) {
		vars.push_back(getVariableAt(i));
	}
	return vars;
}

bool Stack::isNilAt(int index) const {
	return checkIndex(index) && lua_isnil(&_luaState, index);
}

bool Stack::isBooleanAt(int index) const {
	return checkIndex(index) && lua_isboolean(&_luaState, index);
}

bool Stack::isNumberAt(int index) const {
	return checkIndex(index) && lua_isnumber(&_luaState, index) != 0;
}

bool Stack::isStringAt(int index) const {
	return checkIndex(index) && lua_isstring(&_luaState, index) != 0;
}

bool Stack::isTableAt(int index) const {
	return checkIndex(index) && lua_istable(&_luaState, index);
}

bool Stack::isFunctionAt(int index) const {
	return checkIndex(index) && lua_isfunction(&_luaState, index);
}

bool Stack::isUserTypeAt(int index, const Common::UString &type) const {
	if (type.empty()) {
		return getTypeAt(index) == kTypeUserType;
	}

	tolua_Error error;
	return checkIndex(index) && tolua_isusertype(&_luaState, index, type.c_str(), 0, &error) != 0;
}

void Stack::registerGCForTopObject() {
	assert(getTypeAt(getSize()) == kTypeUserType);
	tolua_register_gc(&_luaState, getSize());
}

lua_State &Stack::getLuaState() const {
	return _luaState;
}

bool Stack::checkIndex(int index) const {
	index = std::abs(index);
	return index > 0 && index <= getSize();
}

} // End of namespace Lua

} // End of namespace Aurora
