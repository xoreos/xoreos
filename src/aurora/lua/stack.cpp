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

extern "C" {

#include "toluapp/tolua++.h"

}

#include "src/common/error.h"
#include "src/common/ustring.h"

#include "src/aurora/lua/stack.h"

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

bool Stack::getBooleanAt(int index) const {
	if (!isBooleanAt(index)) {
		throw Common::Exception("Failed to get a boolean value from the Lua stack (index: %d)", index);
	}
	return lua_toboolean(&_luaState, index);
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

Common::UString Stack::getTypeNameAt(int index) const {
	if (!checkIndex(index)) {
		throw Common::Exception("Invalid Lua stack index: %d", index);
	}

	const Common::UString type = tolua_typename(&_luaState, index);
	lua_pop(&_luaState, 1);
	return type;
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

bool Stack::isUserTypeAt(int index, const Common::UString &type) const {
	tolua_Error error;
	return checkIndex(index) && tolua_isusertype(&_luaState, index, type.c_str(), 0, &error) != 0;
}

lua_State &Stack::getLuaState() const {
	return _luaState;
}

bool Stack::checkIndex(int index) const {
	index = std::abs(index);
	return index > 0 && index <= getSize();
}

void Stack::pushRawUserType(void *value, const Common::UString &type) {
	tolua_pushusertype(&_luaState, value, type.c_str());
}

void *Stack::getRawUserTypeAt(int index, const Common::UString &type) const {
	if (!isUserTypeAt(index, type)) {
		throw Common::Exception(
		    "Failed to get a usertype value from the Lua stack (type: %s, index: %d)",
		    type.c_str(),
		    index);
	}
	return tolua_tousertype(&_luaState, index, 0);
}

} // End of namespace Lua

} // End of namespace Aurora
