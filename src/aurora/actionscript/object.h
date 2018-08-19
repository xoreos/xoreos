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
 *  Abstract object which is inherited by every other class.
 */

#ifndef AURORA_ACTIONSCRIPT_OBJECT_H
#define AURORA_ACTIONSCRIPT_OBJECT_H

#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "src/common/ustring.h"

#include "src/aurora/actionscript/variable.h"
#include "src/aurora/actionscript/avm.h"

namespace Aurora {

namespace ActionScript {

class AVM;
class Object;

typedef boost::shared_ptr<Object> ObjectPtr;

class Object : public boost::enable_shared_from_this<Object> {
public:
	Object();
	Object(Object *object);
	virtual ~Object();

	bool hasMember(const Common::UString &id);

	Variable getMember(Common::UString);
	void setMember(Common::UString, Variable);

	Variable call(Common::UString, AVM &avm);

private:
	std::map<Common::UString, Variable> _members;
};

} // End of namespace ActionScript

} // End of namespace Aurora

#endif // AURORA_ACTIONSCRIPT_OBJECT_H
