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
 *  A variable used in the execution context.
 */

#ifndef AURORA_ACTIONSCRIPT_VARIABLE_H
#define AURORA_ACTIONSCRIPT_VARIABLE_H

#include <boost/shared_ptr.hpp>

#include "src/common/ustring.h"

#include "src/aurora/actionscript/types.h"

namespace Aurora {

namespace ActionScript {

class Object;
class Function;

typedef boost::shared_ptr<Object> ObjectPtr;

/** An action script variable. */
class Variable {
public:
	Variable();

	Variable(const Common::UString &value);
	Variable(double value);
	Variable(unsigned int value);
	Variable(bool value);
	Variable(ObjectPtr value);

	Variable(const Variable &variable);

	~Variable();

	bool isUndefined() const;
	bool isObject() const;
	bool isString() const;
	bool isNumber() const;
	bool isFunction() const;

	double asNumber() const;
	ObjectPtr asObject();
	const Common::UString &asString() const;
	bool asBoolean() const;

	void operator=(Variable v);

	bool operator!();
	Variable operator&&(Variable v);
	Variable operator||(Variable v);

	Variable operator==(Variable v);
	Variable operator<(Variable v);

	Variable operator-(Variable v);
	Variable operator+(Variable v);
	Variable operator*(Variable v);
	Variable operator/(Variable v);

	Variable operator++();

private:
	Type _type;

	struct {
		ObjectPtr object;

		double number;
		bool boolean;

		Common::UString string;
	} _value;
};

} // End of namespace ActionScript

} // End of namespace Aurora

#endif // AURORA_ACTIONSCRIPT_VARIABLE_H
