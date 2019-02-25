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
 *  Neverwinter Nights 2 engine functions messing with items and inventory.
 */

#include "src/common/util.h"

#include "src/aurora/nwscript/functioncontext.h"

#include "src/engines/nwn2/types.h"
#include "src/engines/nwn2/objectcontainer.h"
#include "src/engines/nwn2/item.h"

#include "src/engines/nwn2/script/functions.h"

namespace Engines {

namespace NWN2 {

void Functions::getDroppableFlag(Aurora::NWScript::FunctionContext &ctx) {
	Item *item = NWN2::ObjectContainer::toItem(getParamObject(ctx, 0));
	ctx.getReturn() = item && item->getDroppableFlag();
}

void Functions::getIdentified(Aurora::NWScript::FunctionContext &ctx) {
	Item *item = NWN2::ObjectContainer::toItem(getParamObject(ctx, 0));
	ctx.getReturn() = item && item->getIdentified();
}

void Functions::getItemCursedFlag(Aurora::NWScript::FunctionContext &ctx) {
	Item *item = NWN2::ObjectContainer::toItem(getParamObject(ctx, 0));
	ctx.getReturn() = item && item->getItemCursedFlag();
}

void Functions::getPickpocketableFlag(Aurora::NWScript::FunctionContext &ctx) {
	Item *item = NWN2::ObjectContainer::toItem(getParamObject(ctx, 0));
	ctx.getReturn() = item && item->getPickpocketableFlag();
}

void Functions::getStolenFlag(Aurora::NWScript::FunctionContext &ctx) {
	Item *item = NWN2::ObjectContainer::toItem(getParamObject(ctx, 0));
	ctx.getReturn() = item && item->getStolenFlag();
}

void Functions::getBaseItemType(Aurora::NWScript::FunctionContext &ctx) {
	Item *item = NWN2::ObjectContainer::toItem(getParamObject(ctx, 0));
	if (item)
		ctx.getReturn() = (int) item->getBaseItemType();
}

void Functions::getItemIcon(Aurora::NWScript::FunctionContext &ctx) {
	Item *item = NWN2::ObjectContainer::toItem(getParamObject(ctx, 0));
	if (item)
		ctx.getReturn() = (int) item->getItemIcon();
}

void Functions::getItemStackSize(Aurora::NWScript::FunctionContext &ctx) {
	Item *item = NWN2::ObjectContainer::toItem(getParamObject(ctx, 0));
	if (item)
		ctx.getReturn() = (int) item->getItemStackSize();
}

void Functions::getNumStackedItems(Aurora::NWScript::FunctionContext &ctx) {
	// Duplicate functionality of getItemStackSize()
	Item *item = NWN2::ObjectContainer::toItem(getParamObject(ctx, 0));
	if (item)
		ctx.getReturn() = (int) item->getItemStackSize();
}

} // End of namespace NWN2

} // End of namespace Engines
