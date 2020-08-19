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
 *  An inventory in a Neverwinter Nights 2 object.
 */

#ifndef ENGINES_NWN2_INVENTORY_H
#define ENGINES_NWN2_INVENTORY_H

#include <vector>

#include "src/engines/nwn2/types.h"

namespace Engines {

namespace NWN2 {

class Item;

class Inventory {
public:
	Inventory();
	Inventory(const Aurora::GFF3Struct &inventory);
	~Inventory();

	Item *getFirstItemInInventory();
	Item *getNextItemInInventory();
	Item *getItemInSlot(InventorySlot slot) const;

	/** Add a new item to the inventory using the blueprint template. */
	Item *createItem(const Common::UString &blueprint, uint16_t stackSize, const Common::UString &tag);

private:

	typedef std::vector<Item *> ItemSlots;

	ItemSlots equippedItems;
	ItemSlots inventoryItems;

	size_t _lastRetrieved { SIZE_MAX }; ///< Index of last retrieved item.

	/** Insert item in the slot. */
	void insertItem(Item *item, size_t slot = 0);

	/** Find the next inventory item starting at the last retrieved. */
	Item *getNextRetrieved();

	/** Convert a bit flag to an equipment slot. */
	InventorySlot getSlotFromBitFlag(uint32_t bitFlag) const;

	/** Delete all items in inventory then clear the maps. */
	void clear();

	/** Load from an item list. */
	void load(const Aurora::GFF3Struct &inventory);
};

} // End of namespace NWN2

} // End of namespace Engines

#endif // ENGINES_NWN2_INVENTORY_H
