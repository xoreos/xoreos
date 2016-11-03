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
 *  NWScript utility functions.
 */

#ifndef AURORA_NWSCRIPT_UTIL_H
#define AURORA_NWSCRIPT_UTIL_H

#include "src/common/ustring.h"

namespace Aurora {

namespace NWScript {

class Object;
class FunctionContext;

/** Construct a string with the tag of this object. */
Common::UString formatTag(const Object *object);

/** Construct a string describing parameters of this function. */
Common::UString formatParams(const FunctionContext &ctx);

/** Construct a string describing the return value of this function. */
Common::UString formatReturn(const FunctionContext &ctx);

/** Construct a string describing this variable type. */
Common::UString formatType(Type type);

/** Print a description of this variable into that string. */
void formatVariable(Common::UString &str, const Variable &var);

} // End of namespace NWScript

} // End of namespace Aurora

#endif // AURORA_NWSCRIPT_UTIL_H
