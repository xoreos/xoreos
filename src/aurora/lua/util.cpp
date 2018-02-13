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
 *  Lua helpers.
 */

#include "src/aurora/lua/util.h"
#include "src/aurora/lua/stack.h"
#include "src/aurora/lua/variable.h"
#include "src/aurora/lua/table.h"

namespace Aurora {

namespace Lua {

void *getRawCppObjectFromStack(const Stack &stack, int index) {
	switch (stack.getTypeAt(index)) {
		case Aurora::Lua::kTypeTable: {
			TableRef instance = stack.getTableAt(index);
			instance.setMetaAccessEnabled(false);
			if (instance.isUserTypeAt("CPP_instance")) {
				return instance.getRawUserTypeAt("CPP_instance");
			}
		}
		// Fallthrough
		case Aurora::Lua::kTypeUserType:
			return stack.getRawUserTypeAt(index);
		default:
			break;
	}
	return 0;
}

void *getRawCppObjectFromVariable(const Variable &var) {
	switch (var.getType()) {
		case Aurora::Lua::kTypeTable: {
			TableRef instance = var.getTable();
			instance.setMetaAccessEnabled(false);
			if (instance.isUserTypeAt("CPP_instance")) {
				return instance.getRawUserTypeAt("CPP_instance");
			}
		}
		// Fallthrough
		case Aurora::Lua::kTypeUserType:
			return var.getRawUserType();
		default:
			break;
	}
	return 0;
}

} // End of namespace Lua

} // End of namespace Aurora
