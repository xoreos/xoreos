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

#ifndef AURORA_LUA_FUNCTION_H
#define AURORA_LUA_FUNCTION_H

#include "src/aurora/lua/types.h"

namespace Aurora {

namespace Lua {

/** A reference to a Lua function. */
class FunctionRef {
public:
	FunctionRef();
	FunctionRef(const Stack &stack, int index);
	FunctionRef(lua_State &state, int index);
	FunctionRef(const FunctionRef &fn);
	~FunctionRef();

	const FunctionRef &operator=(const FunctionRef &fn);

	Variables call(const Variables &params) const;

	Variables call() const;
	Variables call(const Variable &v) const;
	Variables call(const Variable &v1, const Variable &v2) const;
	Variables call(const Variable &v1, const Variable &v2, const Variable &v3) const;
	Variables call(const Variable &v1, const Variable &v2, const Variable &v3, const Variable &v4) const;

	lua_State &getLuaState() const;
	int getRef() const;

private:
	lua_State *_luaState;
	int _ref;

	void pushSelf() const;
};

} // End of namespace Lua

} // End of namespace Aurora

#endif // AURORA_LUA_FUNCTION_H
