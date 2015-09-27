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
 *  NWScript variable.
 */

#ifndef AURORA_NWSCRIPT_VARIABLE_H
#define AURORA_NWSCRIPT_VARIABLE_H

#include <vector>

#include <boost/shared_ptr.hpp>

#include "src/common/types.h"

#include "src/aurora/types.h"

#include "src/aurora/nwscript/types.h"

namespace Common {
	class UString;
}

namespace Aurora {

namespace NWScript {

class Object;
class EngineType;

struct ScriptState {
	uint32 offset;
	std::vector<class Variable> globals;
	std::vector<class Variable> locals;
};

class Variable {
public:
	typedef std::vector< boost::shared_ptr<Variable> > Array;

	Variable(Type type = kTypeVoid);
	Variable(int32 value);
	Variable(float value);
	Variable(const Common::UString &value);
	Variable(Object *value);
	Variable(const EngineType *value);
	Variable(const EngineType &value);
	Variable(float x, float y, float z);
	Variable(const Variable &var);
	~Variable();

	void setType(Type type);

	Variable &operator=(const Variable &var);

	Variable &operator=(int32 value);
	Variable &operator=(float value);
	Variable &operator=(const Common::UString &value);
	Variable &operator=(Object *value);
	Variable &operator=(const EngineType *value);
	Variable &operator=(const EngineType &value);

	bool operator==(const Variable &var) const;
	bool operator!=(const Variable &var) const;

	Type getType() const;

	int32 getInt() const;
	float getFloat() const;
	Common::UString &getString();
	const Common::UString &getString() const;
	Object *getObject() const;
	EngineType *getEngineType() const;

	void setVector(float  x, float  y, float  z);
	void getVector(float &x, float &y, float &z) const;

	const Array &getArray() const;
	Array &getArray();

	size_t getArraySize() const;

	void growArray(Type type, size_t size);

	ScriptState &getScriptState();
	const ScriptState &getScriptState() const;

	Variable *getReference() const;
	void setReference(Variable *reference);

private:
	Type _type;

	union {
		int32 _int;
		float _float;
		Common::UString *_string;
		Object *_object;
		float _vector[3];
		ScriptState *_scriptState;
		EngineType *_engineType;
		Variable *_reference;
	} _value;

	boost::shared_ptr<Array> _array;
};

} // End of namespace NWScript

} // End of namespace Aurora

#endif // AURORA_NWSCRIPT_VARIABLE_H
