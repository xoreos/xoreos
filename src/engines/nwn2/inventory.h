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

private:

	typedef std::map<uint16, Item *> ItemSlotMap;

	ItemSlotMap equippedItems;
	ItemSlotMap inventoryItems;

	uint32 _lastRetrieved; //Index of last retrieved item.

	/** Convert a bit flag to an equipment slot. */
	InventorySlot getSlotFromBitFlag(uint32 bitFlag) const;

	/** Delete all items in inventory then clear the map. */
	void clear();

	/** Load from an item list. */
	void load(const Aurora::GFF3Struct &inventory);
};

} // End of namespace NWN2

} // End of namespace Engines

#endif // ENGINES_NWN2_INVENTORY_H
