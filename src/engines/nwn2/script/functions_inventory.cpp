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

void Functions::getHasInventory(Aurora::NWScript::FunctionContext &ctx) {
	Object *object = NWN2::ObjectContainer::toObject(getParamObject(ctx, 0));
	ctx.getReturn() = object && object->getHasInventory();
}

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

void Functions::setDroppableFlag(Aurora::NWScript::FunctionContext &ctx) {
	Item *item = NWN2::ObjectContainer::toItem(getParamObject(ctx, 0));
	if (item) {
		const bool droppable = ctx.getParams()[1].getInt() != 0;
		item->setDroppableFlag(droppable);
	}
}

void Functions::setIdentified(Aurora::NWScript::FunctionContext &ctx) {
	Item *item = NWN2::ObjectContainer::toItem(getParamObject(ctx, 0));
	if (item) {
		const bool identified = ctx.getParams()[1].getInt() != 0;
		item->setIdentified(identified);
	}
}

void Functions::setItemCursedFlag(Aurora::NWScript::FunctionContext &ctx) {
	Item *item = NWN2::ObjectContainer::toItem(getParamObject(ctx, 0));
	if (item) {
		const bool cursed = ctx.getParams()[1].getInt() != 0;
		item->setItemCursedFlag(cursed);
	}
}

void Functions::setPickpocketableFlag(Aurora::NWScript::FunctionContext &ctx) {
	Item *item = NWN2::ObjectContainer::toItem(getParamObject(ctx, 0));
	if (item) {
		const bool pickpocketable = ctx.getParams()[1].getInt() != 0;
		item->setPickpocketableFlag(pickpocketable);
	}
}

void Functions::setStolenFlag(Aurora::NWScript::FunctionContext &ctx) {
	Item *item = NWN2::ObjectContainer::toItem(getParamObject(ctx, 0));
	if (item) {
		const bool stolen = ctx.getParams()[1].getInt() != 0;
		item->setStolenFlag(stolen);
	}
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

void Functions::setItemIcon(Aurora::NWScript::FunctionContext &ctx) {
	Item *item = NWN2::ObjectContainer::toItem(getParamObject(ctx, 0));
	if (item) {
		const uint32 icon = (uint32)(ctx.getParams()[1].getInt());
		item->setItemIcon(icon);
	}
}

void Functions::setItemStackSize(Aurora::NWScript::FunctionContext &ctx) {
	Item *item = NWN2::ObjectContainer::toItem(getParamObject(ctx, 0));
	if (item) {
		const uint16 stackSize = (uint16)(ctx.getParams()[1].getInt());
		item->setItemStackSize(stackSize);
	}
}

void Functions::getFirstItemInInventory(Aurora::NWScript::FunctionContext &ctx) {
	Item *item = NWN2::ObjectContainer::toItem(getParamObject(ctx, 0));
	if (item && item->getHasInventory()) {
		ctx.getReturn() = item->getFirstItemInInventory();
	}
}

void Functions::getNextItemInInventory(Aurora::NWScript::FunctionContext &ctx) {
	Item *item = NWN2::ObjectContainer::toItem(getParamObject(ctx, 0));
	if (item && item->getHasInventory()) {
		ctx.getReturn() = item->getNextItemInInventory();
	}
}

void Functions::createItemOnObject(Aurora::NWScript::FunctionContext &ctx) {
	Item *item = NWN2::ObjectContainer::toItem(getParamObject(ctx, 1));
	if (item && item->getHasInventory()) {
		const Common::UString &blueprint = ctx.getParams()[0].getString();
		if (blueprint.empty())
			return;

		int stackSize = ctx.getParams()[2].getInt();
		const Common::UString &tag = ctx.getParams()[3].getString();
		ctx.getReturn() = item->createItemOnObject(blueprint, stackSize, tag);
	}
}

} // End of namespace NWN2

} // End of namespace Engines
