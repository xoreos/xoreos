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
 *  String implementation for actionscript.
 */

#ifndef AURORA_ACTIONSCRIPT_STRING_H
#define AURORA_ACTIONSCRIPT_STRING_H

#include "src/common/ustring.h"

#include "src/aurora/actionscript/object.h"

namespace Aurora {

namespace ActionScript {

class String;

typedef boost::shared_ptr<String> StringPtr;

class String : public Common::UString, public Object {
public:
	String();
};

} // End of namespace ActionScript

} // End of namespace Aurora

#endif // AURORA_ACTIONSCRIPT_STRING_H
