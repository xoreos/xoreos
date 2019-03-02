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

#include <iterator>

#include "src/common/error.h"

#include "src/aurora/gff3file.h"

#include "src/engines/nwn2/inventory.h"
#include "src/engines/nwn2/item.h"

namespace Engines {

namespace NWN2 {

Inventory::Inventory() {
}

Inventory::Inventory(const Aurora::GFF3Struct &inventory) {

	load(inventory);
}

Inventory::~Inventory() {
	clear();
}

/** Return the first item in the inventory. */
Item *Inventory::getFirstItemInInventory() {
	_lastRetrieved = 0;

	if (!equippedItems.empty()) {
		return equippedItems.begin()->second;
	} else if (!inventoryItems.empty()) {
		return inventoryItems.begin()->second;
	}
	return 0;
}

/** Return the next item in the inventory. */
Item *Inventory::getNextItemInInventory() {
	// Find the matching item
	const size_t equipSize = equippedItems.size();
	_lastRetrieved++;
	if (_lastRetrieved < equipSize) {
		std::map<uint16, Item *>::const_iterator item = equippedItems.begin();
		std::advance(item, _lastRetrieved);
		return item->second;
	} else {
		const size_t inventorySize = inventoryItems.size();
		if (_lastRetrieved < equipSize + inventorySize) {
			// TODO: Check for an item with nested inventory (Ex.: magic bag)
			std::map<uint16, Item *>::const_iterator item = inventoryItems.begin();
			std::advance(item, _lastRetrieved - equipSize);
			return item->second;
		}
	}

	// End of the inventory
	_lastRetrieved = UINT32_MAX;
	return 0;
}

void Inventory::clear() {
	std::map<uint16, Item *>::iterator item;

	// Purge all equipped item instances
	for (item = equippedItems.begin(); item != equippedItems.end(); ++item)
		if (item->second)
			delete(item->second);

	// Purge all inventory item instances
	for (item = inventoryItems.begin(); item != inventoryItems.end(); ++item)
		if (item->second)
			delete(item->second);

	// Clear the maps
	equippedItems.clear();
	inventoryItems.clear();
}

InventorySlot Inventory::getSlotFromBitFlag(uint32 bitFlag) const {
	// Bit flags used for the 'EquipableSlots' column of 'baseitem.2da'
	static const uint32 kSlotBitFlagHead      = (uint32)(0x1 << (int) kInventorySlotHead);
	static const uint32 kSlotBitFlagChest     = (uint32)(0x1 << (int) kInventorySlotChest);
	static const uint32 kSlotBitFlagBoots     = (uint32)(0x1 << (int) kInventorySlotBoots);
	static const uint32 kSlotBitFlagArms      = (uint32)(0x1 << (int) kInventorySlotArms);
	static const uint32 kSlotBitFlagRightHand = (uint32)(0x1 << (int) kInventorySlotRightHand);
	static const uint32 kSlotBitFlagLeftHand  = (uint32)(0x1 << (int) kInventorySlotLeftHand);
	static const uint32 kSlotBitFlagCloak     = (uint32)(0x1 << (int) kInventorySlotCloak);
	static const uint32 kSlotBitFlagLeftRing  = (uint32)(0x1 << (int) kInventorySlotLeftRing);
	static const uint32 kSlotBitFlagRightRing = (uint32)(0x1 << (int) kInventorySlotRightRing);
	static const uint32 kSlotBitFlagNeck      = (uint32)(0x1 << (int) kInventorySlotNeck);
	static const uint32 kSlotBitFlagBelt      = (uint32)(0x1 << (int) kInventorySlotBelt);
	static const uint32 kSlotBitFlagArrows    = (uint32)(0x1 << (int) kInventorySlotArrows);
	static const uint32 kSlotBitFlagBullets   = (uint32)(0x1 << (int) kInventorySlotBullets);
	static const uint32 kSlotBitFlagBolts     = (uint32)(0x1 << (int) kInventorySlotBolts);
	static const uint32 kSlotBitFlagCWeaponL  = (uint32)(0x1 << (int) kInventorySlotCWeaponL);
	static const uint32 kSlotBitFlagCWeaponR  = (uint32)(0x1 << (int) kInventorySlotCWeaponR);
	static const uint32 kSlotBitFlagCWeaponB  = (uint32)(0x1 << (int) kInventorySlotCWeaponB);
	static const uint32 kSlotBitFlagCArmour   = (uint32)(0x1 << (int) kInventorySlotCArmour);

	switch (bitFlag) {
		case kSlotBitFlagHead:
			return kInventorySlotHead;
		case kSlotBitFlagChest:
			return kInventorySlotChest;
		case kSlotBitFlagBoots:
			return kInventorySlotBoots;
		case kSlotBitFlagArms:
			return kInventorySlotArms;
		case kSlotBitFlagRightHand:
			return kInventorySlotRightHand;
		case kSlotBitFlagLeftHand:
			return kInventorySlotLeftHand;
		case kSlotBitFlagCloak:
			return kInventorySlotCloak;
		case kSlotBitFlagLeftRing:
			return kInventorySlotLeftRing;
		case kSlotBitFlagRightRing:
			return kInventorySlotRightRing;
		case kSlotBitFlagNeck:
			return kInventorySlotNeck;
		case kSlotBitFlagBelt:
			return kInventorySlotBelt;
		case kSlotBitFlagArrows:
			return kInventorySlotArrows;
		case kSlotBitFlagBullets:
			return kInventorySlotBullets;
		case kSlotBitFlagBolts:
			return kInventorySlotBolts;
		case kSlotBitFlagCWeaponL:
			return kInventorySlotCWeaponL;
		case kSlotBitFlagCWeaponR:
			return kInventorySlotCWeaponR;
		case kSlotBitFlagCWeaponB:
			return kInventorySlotCWeaponB;
		case kSlotBitFlagCArmour:
			return kInventorySlotCArmour;
	}

	throw Common::Exception("Invalid inventory slot flag: %x", bitFlag);
}

void Inventory::load(const Aurora::GFF3Struct &inventory) {
	// Load items in general inventory
	if (inventory.hasField("ItemList")) {
		const Aurora::GFF3List &itemList = inventory.getList("ItemList");
		for (Aurora::GFF3List::const_iterator it = itemList.begin(); it != itemList.end(); ++it) {
			const Aurora::GFF3Struct &item = **it;
			uint16 slot = item.getUint("Repos_Index");

			// TODO: Find an empty inventory slot instead
			if (inventoryItems.find(slot) != inventoryItems.end())
				throw Common::Exception("Inventory slot already occupied: %d", slot);

			inventoryItems[slot] = new Item(item);
		}
	}

	// Load equipped items (creature only)
	if (inventory.hasField("Equip_ItemList")) {
		const Aurora::GFF3List &itemList = inventory.getList("Equip_ItemList");
		for (Aurora::GFF3List::const_iterator it = itemList.begin(); it != itemList.end(); ++it) {
			const Aurora::GFF3Struct &item = **it;

			// Slot number is given by the struct id bit flag
			uint16 slot = getSlotFromBitFlag(item.getID());

			// TODO: Move item to general inventory instead
			if (equippedItems.find(slot) != equippedItems.end())
				throw Common::Exception("Equip slot already occupied: %d", slot);

			equippedItems[slot] = new Item(item);
		}
	}
}

} // End of namespace NWN2

} // End of namespace Engines
