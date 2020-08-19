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
#include "src/engines/nwn2/creature.h"
#include "src/engines/nwn2/store.h"
#include "src/engines/nwn2/item.h"
#include "src/engines/nwn2/itemproperty.h"

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

void Functions::getInfiniteFlag(Aurora::NWScript::FunctionContext &ctx) {
	Item *item = NWN2::ObjectContainer::toItem(getParamObject(ctx, 0));
	ctx.getReturn() = item && item->getInfinite();
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

void Functions::setInfiniteFlag(Aurora::NWScript::FunctionContext &ctx) {
	Item *item = NWN2::ObjectContainer::toItem(getParamObject(ctx, 0));
	if (item) {
		const bool infinite = ctx.getParams()[1].getInt() != 0;
		item->setInfinite(infinite);
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

void Functions::getItemCharges(Aurora::NWScript::FunctionContext &ctx) {
	Item *item = NWN2::ObjectContainer::toItem(getParamObject(ctx, 0));
	if (item)
		ctx.getReturn() = (int) item->getItemCharges();
}

void Functions::setItemIcon(Aurora::NWScript::FunctionContext &ctx) {
	Item *item = NWN2::ObjectContainer::toItem(getParamObject(ctx, 0));
	if (item) {
		const uint32_t icon = (uint32_t)(ctx.getParams()[1].getInt());
		item->setItemIcon(icon);
	}
}

void Functions::setItemStackSize(Aurora::NWScript::FunctionContext &ctx) {
	Item *item = NWN2::ObjectContainer::toItem(getParamObject(ctx, 0));
	if (item) {
		const uint16_t stackSize = (uint16_t)(ctx.getParams()[1].getInt());
		item->setItemStackSize(stackSize);
	}
}

void Functions::setItemCharges(Aurora::NWScript::FunctionContext &ctx) {
	Item *item = NWN2::ObjectContainer::toItem(getParamObject(ctx, 0));
	if (item) {
		const uint8_t charges = (uint8_t)(ctx.getParams()[1].getInt());
		item->setItemCharges(charges);
	}
}

void Functions::getStoreIdentifyCost(Aurora::NWScript::FunctionContext &ctx) {
	Store *store = NWN2::ObjectContainer::toStore(getParamObject(ctx, 0));
	ctx.getReturn() = (store) ? store->getStoreIdentifyCost() : -2;
}

void Functions::getStoreGold(Aurora::NWScript::FunctionContext &ctx) {
	Store *store = NWN2::ObjectContainer::toStore(getParamObject(ctx, 0));
	ctx.getReturn() = (store) ? store->getStoreGold() : -2;
}

void Functions::getStoreMaxBuyPrice(Aurora::NWScript::FunctionContext &ctx) {
	Store *store = NWN2::ObjectContainer::toStore(getParamObject(ctx, 0));
	ctx.getReturn() = (store) ? store->getStoreMaximumBuyPrice() : -2;
}

void Functions::setStoreIdentifyCost(Aurora::NWScript::FunctionContext &ctx) {
	Store *store = NWN2::ObjectContainer::toStore(getParamObject(ctx, 0));
	if (store) {
		const int32_t identify = (uint8_t)(ctx.getParams()[1].getInt());
		store->setStoreIdentifyCost(identify);
	}
}

void Functions::setStoreGold(Aurora::NWScript::FunctionContext &ctx) {
	Store *store = NWN2::ObjectContainer::toStore(getParamObject(ctx, 0));
	if (store) {
		const int32_t gold = (uint8_t)(ctx.getParams()[1].getInt());
		store->setStoreGold(gold);
	}
}

void Functions::setStoreMaxBuyPrice(Aurora::NWScript::FunctionContext &ctx) {
	Store *store = NWN2::ObjectContainer::toStore(getParamObject(ctx, 0));
	if (store) {
		const int32_t max = (uint8_t)(ctx.getParams()[1].getInt());
		store->setStoreMaximumBuyPrice(max);
	}
}

void Functions::getFirstItemInInventory(Aurora::NWScript::FunctionContext &ctx) {
	Item *item = NWN2::ObjectContainer::toItem(getParamObject(ctx, 0));
	if (item && item->getHasInventory()) {
		ctx.getReturn() = item->getFirstItemInInventory();
	} else {
		Store *store = NWN2::ObjectContainer::toStore(getParamObject(ctx, 0));
		if (store)
			ctx.getReturn() = store->getFirstItemInInventory();
	}
}

void Functions::getNextItemInInventory(Aurora::NWScript::FunctionContext &ctx) {
	Item *item = NWN2::ObjectContainer::toItem(getParamObject(ctx, 0));
	if (item && item->getHasInventory()) {
		ctx.getReturn() = item->getNextItemInInventory();
	} else {
		Store *store = NWN2::ObjectContainer::toStore(getParamObject(ctx, 0));
		if (store)
			ctx.getReturn() = store->getNextItemInInventory();
	}
}

void Functions::getItemInSlot(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = NWN2::ObjectContainer::toCreature(getParamObject(ctx, 0));
	int slot = ctx.getParams()[1].getInt();
	ctx.getReturn() = (creature) ? creature->getItemInSlot((InventorySlot) slot) : 0;
}

void Functions::getFirstItemProperty(Aurora::NWScript::FunctionContext &ctx) {
	Item *item = NWN2::ObjectContainer::toItem(getParamObject(ctx, 0));
	if (item) {
		ItemProperty *firstIP = item->getFirstItemProperty();
		if (firstIP) {
			ItemProperty prop(*firstIP);
			ctx.getReturn() = prop;
			return;
		}
	}

	ItemProperty iprop(kItemPropertyInvalid, 0);
	ctx.getReturn() = iprop;
}

void Functions::getNextItemProperty(Aurora::NWScript::FunctionContext &ctx) {
	Item *item = NWN2::ObjectContainer::toItem(getParamObject(ctx, 0));
	if (item) {
		ItemProperty *nextIP = item->getNextItemProperty();
		if (nextIP) {
			ItemProperty prop(*nextIP);
			ctx.getReturn() = prop;
			return;
		}
	}

	ItemProperty iprop(kItemPropertyInvalid, 0);
	ctx.getReturn() = iprop;
}

void Functions::getItemHasItemProperty(Aurora::NWScript::FunctionContext &ctx) {
	Item *item = NWN2::ObjectContainer::toItem(getParamObject(ctx, 0));
	if (!item)
		return;

	const ItemPropertyType type = (ItemPropertyType)(ctx.getParams()[1].getInt());
	ctx.getReturn() = (int) item->getItemHasItemProperty(type);
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
