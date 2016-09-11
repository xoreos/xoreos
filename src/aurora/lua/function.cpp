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
 *  A reference to a Lua function.
 */

#include "lua/lauxlib.h"

#include "src/common/error.h"

#include "src/aurora/lua/function.h"
#include "src/aurora/lua/stack.h"
#include "src/aurora/lua/stackguard.h"
#include "src/aurora/lua/variable.h"

namespace Aurora {

namespace Lua {

FunctionRef::FunctionRef() : _luaState(0), _ref(LUA_REFNIL) {

}

FunctionRef::FunctionRef(const Stack &stack, int index) : _luaState(&stack.getLuaState()), _ref(LUA_REFNIL) {
	lua_pushvalue(_luaState, index);
	_ref = lua_ref(_luaState, true);
}

FunctionRef::FunctionRef(lua_State &state, int index) : _luaState(&state), _ref(LUA_REFNIL) {
	lua_pushvalue(_luaState, index);
	_ref = lua_ref(_luaState, true);
}

FunctionRef::FunctionRef(const FunctionRef &fn) : _luaState(fn._luaState), _ref(LUA_REFNIL) {
	assert(fn._luaState && fn._ref != LUA_REFNIL);

	lua_getref(_luaState, fn._ref);
	_ref = lua_ref(_luaState, true);
}

FunctionRef::~FunctionRef() {
	assert(_ref != LUA_REFNIL);

	lua_unref(_luaState, _ref);
}

const FunctionRef& FunctionRef::operator=(const FunctionRef &fn) {
	if (this == &fn) {
		return *this;
	}

	assert(fn._luaState && fn._ref != LUA_REFNIL);

	lua_State *oldState = _luaState;
	const int oldRef = _ref;

	lua_getref(fn._luaState, fn._ref);
	_luaState = fn._luaState;
	_ref = lua_ref(_luaState, true);

	if (oldState && oldRef != LUA_REFNIL) {
		lua_unref(oldState, oldRef);
	}
	return *this;
}

Variables FunctionRef::call(const Variables &params) const {
	StackGuard guard(*_luaState);

	const int savedTop = lua_gettop(_luaState);
	pushSelf();

	Stack stack(*_luaState);
	stack.pushVariables(params);

	if (lua_pcall(&stack.getLuaState(), params.size(), LUA_MULTRET, 0) != 0) {
		throw Common::Exception("Failed to call Lua function");
	}

	const int retsCount = stack.getSize() - savedTop;
	return stack.getVariablesFromTop(retsCount);
}

Variables FunctionRef::call() const {
	return call(Variables());
}

Variables FunctionRef::call(const Variable &v) const {
	Variables params;
	params.push_back(v);
	return call(params);
}

Variables FunctionRef::call(const Variable &v1, const Variable &v2) const {
	Variables params;
	params.push_back(v1);
	params.push_back(v2);
	return call(params);
}

Variables FunctionRef::call(const Variable &v1, const Variable &v2, const Variable &v3) const {
	Variables params;
	params.push_back(v1);
	params.push_back(v2);
	params.push_back(v3);
	return call(params);
}

Variables FunctionRef::call(const Variable &v1, const Variable &v2, const Variable &v3, const Variable &v4) const {
	Variables params;
	params.push_back(v1);
	params.push_back(v2);
	params.push_back(v3);
	params.push_back(v4);
	return call(params);
}

lua_State &FunctionRef::getLuaState() const {
	assert(_luaState);

	return *_luaState;
}

int FunctionRef::getRef() const {
	return _ref;
}

void FunctionRef::pushSelf() const {
	assert(_luaState && _ref != LUA_REFNIL);

	lua_getref(_luaState, _ref);
}

} // End of namespace Lua

} // End of namespace Aurora
