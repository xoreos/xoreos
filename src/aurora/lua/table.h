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

#ifndef AURORA_LUA_TABLE_H
#define AURORA_LUA_TABLE_H

#include "src/common/ustring.h"

#include "src/aurora/lua/types.h"

namespace Aurora {

namespace Lua {

/** A reference to a Lua table. */
class TableRef {
public:
	TableRef();
	TableRef(const Stack &stack, int index);
	TableRef(lua_State &state, int index);
	TableRef(const TableRef &table);
	~TableRef();

	const TableRef &operator=(const TableRef &table);

	/** Return the number of elements in the table. */
	int getSize() const;

	Common::UString getExactType() const;
	TableRef getMetaTable() const;

	/** Enable/Disable access to the table elements using metamethods. */
	void setMetaAccessEnabled(bool enabled);
	bool isMetaAccessEnabled() const;

	void removeAt(int index);
	void removeAt(const Common::UString &key);

	void setBooleanAt(int index, bool value);
	void setBooleanAt(const Common::UString &key, bool value);
	void setFloatAt(int index, float value);
	void setFloatAt(const Common::UString &key, float value);
	void setIntAt(int index, int value);
	void setIntAt(const Common::UString &key, int value);
	void setStringAt(int index, const char *value);
	void setStringAt(const Common::UString &key, const char *value);
	void setStringAt(int index, const Common::UString &value);
	void setStringAt(const Common::UString &key, const Common::UString &value);
	void setTableAt(int index, const TableRef &value);
	void setTableAt(const Common::UString &key, const TableRef &value);
	void setFunctionAt(int index, const FunctionRef &value);
	void setFunctionAt(const Common::UString &key, const FunctionRef &value);

	/** Push a usertype value onto the stack.
	 *  Expect that @a type is a name of the registered type in the script subsystem.
	 */
	void setUserTypeAt(int index, void *value, const Common::UString &type);
	void setUserTypeAt(const Common::UString &key, void *value, const Common::UString &type);

	void setVariableAt(int index, const Variable &value);
	void setVariableAt(const Common::UString &key, const Variable &value);

	bool getBooleanAt(int index) const;
	bool getBooleanAt(const Common::UString &key) const;
	float getFloatAt(int index) const;
	float getFloatAt(const Common::UString &key) const;
	int getIntAt(int index) const;
	int getIntAt(const Common::UString &key) const;
	Common::UString getStringAt(int index) const;
	Common::UString getStringAt(const Common::UString &key) const;
	TableRef getTableAt(int index) const;
	TableRef getTableAt(const Common::UString &key) const;
	FunctionRef getFunctionAt(int index) const;
	FunctionRef getFunctionAt(const Common::UString &key) const;

	/** Return a usertype value at the given @a index in the stack.
	 *  If @a type is not empty, perform a type check.
	 */
	void *getRawUserTypeAt(int index, const Common::UString &type = "") const;
	void *getRawUserTypeAt(const Common::UString &key, const Common::UString &type = "") const;

	template<typename T>
	T *getUserTypeAt(int index, const Common::UString &type = "") const;
	template<typename T>
	T *getUserTypeAt(const Common::UString &key, const Common::UString &type = "") const;

	Variable getVariableAt(int index) const;
	Variable getVariableAt(const Common::UString &key) const;

	Common::UString getExactTypeAt(int index) const;
	Common::UString getExactTypeAt(const Common::UString &key) const;
	Type getTypeAt(int index) const;
	Type getTypeAt(const Common::UString &key) const;

	Variables getVariables() const;

	bool isNilAt(int index) const;
	bool isNilAt(const Common::UString &key) const;
	bool isBooleanAt(int index) const;
	bool isBooleanAt(const Common::UString &key) const;
	bool isNumberAt(int index) const;
	bool isNumberAt(const Common::UString &key) const;
	bool isStringAt(int index) const;
	bool isStringAt(const Common::UString &key) const;
	bool isTableAt(int index) const;
	bool isTableAt(const Common::UString &key) const;
	bool isFunctionAt(int index) const;
	bool isFunctionAt(const Common::UString &key) const;

	/** Check whether the value at the given @a index is a usertype value.
	 *  If @a type is not empty, perform a type check.
	 */
	bool isUserTypeAt(int index, const Common::UString &type = "") const;
	bool isUserTypeAt(const Common::UString &key, const Common::UString &type = "") const;

	/** Return the underlying Lua state. */
	lua_State &getLuaState() const;

	int getRef() const;

private:
	/** The Lua state. */
	lua_State *_luaState;
	int _ref;
	bool _metaAccessEnabled;

	void pushSelf() const;
};

template<typename T>
inline T *TableRef::getUserTypeAt(int index, const Common::UString &type) const {
	return reinterpret_cast<T *>(getRawUserTypeAt(index, type));
}

template<typename T>
inline T *TableRef::getUserTypeAt(const Common::UString &key, const Common::UString &type) const {
	return reinterpret_cast<T *>(getRawUserTypeAt(key, type));
}

} // End of namespace Lua

} // End of namespace Aurora

#endif // AURORA_LUA_TABLE_H
