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

#ifndef ENGINES_AURORA_LUA_STACK_H
#define ENGINES_AURORA_LUA_STACK_H

struct lua_State;

namespace Common {
	class UString;
}

namespace Aurora {

namespace Lua {

/** A Lua stack wrapper. */
class Stack {
public:
	Stack(lua_State &state);
	~Stack();

	/** Return the number of elements in the stack. */
	int getSize() const;

	/** Push a nil value onto the stack. */
	void pushNil();
	/** Push a boolean @a value onto the stack. */
	void pushBoolean(bool value);
	/** Push a float value onto the stack. */
	void pushFloat(float value);
	/** Push an integer value onto the stack. */
	void pushInt(int value);
	/** Push a raw C string onto the stack. */
	void pushString(const char *value);
	/** Push a string onto the stack. */
	void pushString(const Common::UString &value);

	/** Push a usertype value onto the stack.
	 *  Expect that @a type is a name of the registered type in the script subsystem.
	 */
	template<typename T>
	void pushUserType(T &value, const Common::UString &type);

	/** Return a boolean value at the given @a index in the stack. */
	bool getBooleanAt(int index) const;
	/** Return a float value at the given @a index in the stack. */
	float getFloatAt(int index) const;
	/** Return an integer value at the given @a index in the stack. */
	int getIntAt(int index) const;
	/** Return a string at the given @a index in the stack. */
	Common::UString getStringAt(int index) const;

	/** Return a usertype value at the given @a index in the stack.
	 *  Expect that @a type is a name of the registered type in the script subsystem.
	 */
	template<typename T>
	T *getUserTypeAt(int index, const Common::UString &type) const;

	/** Return the type of the value at the given @a index in the stack. */
	Common::UString getTypeNameAt(int index) const;

	/** Check whether the value with the given @a index is a nil. */
	bool isNilAt(int index) const;
	/** Check whether the value at the given @a index is a boolean value. */
	bool isBooleanAt(int index) const;
	/** Check whether the value at the given @a index is a number. */
	bool isNumberAt(int index) const;
	/** Check whether the value at the given @a index is a string. */
	bool isStringAt(int index) const;
	/** Check whether the value at the given @a index is a usertype value with the @a given type. */
	bool isUserTypeAt(int index, const Common::UString &type) const;

	/** Return the underlying Lua state. */
	lua_State &getLuaState() const;

private:
	/** The Lua state. */
	lua_State &_luaState;

	/** Check whether the given @a index is valid. */
	bool checkIndex(int index) const;

	void pushRawUserType(void *value, const Common::UString &type);
	void *getRawUserTypeAt(int index, const Common::UString &type) const;
};

template<typename T>
void Stack::pushUserType(T &value, const Common::UString &type) {
	return pushRawUserType(&value, type);
}

template<typename T>
T *Stack::getUserTypeAt(int index, const Common::UString &type) const {
	return reinterpret_cast<T *>(getRawUserTypeAt(index, type));
}

} // End of namespace Lua

} // End of namespace Aurora

#endif // ENGINES_AURORA_LUA_STACK_H
