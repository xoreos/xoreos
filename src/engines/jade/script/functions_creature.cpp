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
 *  Jade Empire engine functions messing with creatures.
 */

#include "src/common/util.h"

#include "src/aurora/nwscript/functioncontext.h"

#include "src/engines/jade/types.h"
#include "src/engines/jade/objectcontainer.h"
#include "src/engines/jade/creature.h"

#include "src/engines/jade/script/functions.h"

namespace Engines {

namespace Jade {

void Functions::getAutoBalance(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = Jade::ObjectContainer::toCreature(getParamObject(ctx, 0));

	ctx.getReturn() = creature ? (int32_t) creature->getAutoBalance() : 0;
}

} // End of namespace Jade

} // End of namespace Engines
