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
 *  An NWScript object.
 */

#ifndef AURORA_NWSCRIPT_OBJECT_H
#define AURORA_NWSCRIPT_OBJECT_H

#include "src/common/types.h"
#include "src/common/ustring.h"

#include "src/aurora/nwscript/variablecontainer.h"

namespace Aurora {

namespace NWScript {

class Object : public VariableContainer {
public:
	Object() : _id(kObjectIDInvalid) {
	}

	virtual ~Object() {
	}

	uint32_t getID() const {
		return _id;
	}

	const Common::UString &getTag() const {
		return _tag;
	}

protected:
	uint32_t _id;

	Common::UString _tag;
};

} // End of namespace NWScript

} // End of namespace Aurora

#endif // AURORA_NWSCRIPT_OBJECT_H
