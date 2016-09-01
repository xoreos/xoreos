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
 *  Lua helpers.
 */

#ifndef AURORA_LUA_UTIL_H
#define AURORA_LUA_UTIL_H

#include <cassert>

#include "src/aurora/lua/types.h"
#include "src/aurora/lua/stack.h"

namespace Aurora {

namespace Lua {

void *getRawCppObjectFromStack(const Stack& stack, int index);
void *getRawCppObjectFromVariable(const Variable& var);

template<typename T>
T *getCppObjectFromStack(const Stack& stack, int index) {
	return reinterpret_cast<T *>(getRawCppObjectFromStack(stack, index));
}

template<typename T>
T *getCppObjectFromVariable(const Variable& var) {
	return reinterpret_cast<T *>(getRawCppObjectFromVariable(var));
}

template<typename T>
class DefaultDeleter {
public:
	static int function(lua_State *state) {
		assert(state);

		Stack stack(*state);
		assert(stack.getSize() == 1);

		T* object = getCppObjectFromStack<T>(stack, 1);
		delete object;

		return 0;
	}
};

} // End of namespace Lua

} // End of namespace Aurora

#define LUA_DEFAULT_DELETER(Type) ::Aurora::Lua::DefaultDeleter<Type>::function

#endif // AURORA_LUA_UTIL_H
