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
 *  Lua types.
 */

#ifndef AURORA_LUA_TYPES_H
#define AURORA_LUA_TYPES_H

#include "lua/lua.h"

#include <vector>

namespace Aurora {

namespace Lua {

enum Type {
	kTypeNone            = 0,
	kTypeNil                ,
	kTypeBoolean            ,
	kTypeNumber             ,
	kTypeString             ,
	kTypeTable              ,
	kTypeFunction           ,
	kTypeUserType
};

class Stack;
class Variable;
class TableRef;
class FunctionRef;

typedef std::vector<Variable> Variables;

} // End of namespace Lua

} // End of namespace Aurora

#endif // AURORA_LUA_TYPES_H
