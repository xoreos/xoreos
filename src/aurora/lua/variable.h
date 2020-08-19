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

#ifndef AURORA_LUA_VARIABLE_H
#define AURORA_LUA_VARIABLE_H

#include "src/common/ustring.h"

#include "src/aurora/lua/types.h"

namespace Aurora {

namespace Lua {

class Variable {
public:
	Variable(Type type, const Common::UString &exactType = "");
	Variable(bool value);
	Variable(int32_t value);
	Variable(float value);
	Variable(const char *value);
	Variable(const Common::UString &value);
	Variable(const TableRef &value);
	Variable(const FunctionRef &value);
	Variable(void *value, const Common::UString &exactType);
	Variable(const Variable &var);
	~Variable();

	void setType(Type type, const Common::UString &exactType = "");

	Variable &operator=(const Variable &var);

	Variable &operator=(bool value);
	Variable &operator=(int32_t value);
	Variable &operator=(float value);
	Variable &operator=(const char *value);
	Variable &operator=(const Common::UString &value);
	Variable &operator=(const TableRef &value);
	Variable &operator=(const FunctionRef &value);
	Variable &operator=(void *value);

	bool operator==(const Variable &var) const;
	bool operator!=(const Variable &var) const;

	Type getType() const;
	const Common::UString &getExactType() const;

	bool getBool() const;
	int32_t getInt() const;
	float getFloat() const;
	Common::UString &getString();
	const Common::UString &getString() const;
	TableRef &getTable();
	const TableRef &getTable() const;
	const FunctionRef &getFunction() const;
	void *getRawUserType() const;

	template<typename T>
	T *getUserType() const;

private:
	Type _type;
	Common::UString _exactType;

	union {
		bool _bool;
		float _float;
		Common::UString *_string;
		TableRef *_table;
		FunctionRef *_function;
		void *_data;
	} _value;
};

template<typename T>
inline T *Variable::getUserType() const {
	return reinterpret_cast<T *>(getRawUserType());
}

} // End of namespace Lua

} // End of namespace Aurora

#endif // AURORA_LUA_VARIABLE_H
