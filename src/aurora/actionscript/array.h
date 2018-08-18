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
 *  Implementation for an actionscript Array class.
 */

#ifndef AURORA_ACTIONSCRIPT_ARRAY_H
#define AURORA_ACTIONSCRIPT_ARRAY_H

#include <list>

#include "src/aurora/actionscript/variable.h"
#include "src/aurora/actionscript/function.h"

namespace Aurora {

namespace ActionScript {

class Array;

typedef boost::shared_ptr<Array> ArrayPtr;

class Array : public Object {
public:
	Array(const std::list<Variable> &values = std::list<Variable>());

	size_t length() const;

	void push(const Variable &v);
	Variable pop();

	Variable getMember(const Variable &id);
	void setMember(const Variable &id, const Variable &value);

private:
	std::list<Variable> _values;
};

} // End of namespace ActionScript

} // End of namespace Aurora

#endif // AURORA_ACTIONSCRIPT_ARRAY_H
