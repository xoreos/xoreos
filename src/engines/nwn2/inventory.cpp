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
#include <vector>
#include <algorithm>

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
	// Set to start of inventory
	_lastRetrieved = 0;
	return getNextRetrieved();
}

/** Return the next item in the inventory. */
Item *Inventory::getNextItemInInventory() {
	if (_lastRetrieved == SIZE_MAX)
		return nullptr;

	// Set to next slot
	_lastRetrieved++;
	return getNextRetrieved();
}

Item *Inventory::getNextRetrieved() {
	static const size_t kEquipSize = (size_t) kInventorySlotMax;

	// Check if _lastRetrieved is in the equipment range
	if (_lastRetrieved < kEquipSize) {
		// Set the iterator
		std::vector<Item *>::iterator start1 = equippedItems.begin();
		if (_lastRetrieved > 0)
			std::advance(start1, _lastRetrieved);

		// Search for a non-null item beginning at offset
		std::vector<Item *>::iterator equip = std::find_if(start1, equippedItems.end(), [](Item *i1) {
			return i1 != nullptr;
		});

		// Check for success
		if (equip != equippedItems.end()) {
			// Only return if in bounds
			_lastRetrieved = std::distance(equippedItems.begin(), equip);
			if (_lastRetrieved < kEquipSize)
				return *equip;
		}

		// Fall through to the inventory
		_lastRetrieved = kEquipSize;
	}

	// Check if _lastRetrieved is in the inventory range
	if (_lastRetrieved < kEquipSize + inventoryItems.size()) {
		// Set the iterator
		std::vector<Item *>::iterator start2 = inventoryItems.begin();
		if (_lastRetrieved > kEquipSize)
			std::advance(start2, _lastRetrieved - kEquipSize);

		// Search for a non-null item beginning at offset
		std::vector<Item *>::iterator item = std::find_if(start2, inventoryItems.end(), [](Item *i2) {
			return i2 != nullptr;
		});

		// Check for success
		if (item != inventoryItems.end()) {
			_lastRetrieved = std::distance(inventoryItems.begin(), item) + kEquipSize;
			return *item;
		}
	}

	// Reached end of the inventory
	_lastRetrieved = SIZE_MAX;
	return nullptr;
}

/** Return the item in a slot. */
Item *Inventory::getItemInSlot(InventorySlot slot) const {
	return (slot < kInventorySlotMax) ? equippedItems[(size_t) slot] : nullptr;
}

Item *Inventory::createItem(const Common::UString &blueprint, uint16_t stackSize, const Common::UString &tag) {
	if (blueprint.empty() || stackSize == 0)
		return nullptr;

	// Create and insert the item
	Item *item = new Item(blueprint, stackSize, tag);
	insertItem(item);
	return item;
}

void Inventory::insertItem(Item *item, size_t slot) {
	if (item == nullptr)
		return;

	// If slot is in existing range, check if already occupied
	const size_t max = inventoryItems.size();
	if (slot < max && inventoryItems[slot] != nullptr) {
		// Find an empty inventory slot
		std::vector<Item *>::iterator it = std::find(inventoryItems.begin(), inventoryItems.end(), nullptr);
		slot = (it != inventoryItems.end()) ? std::distance(inventoryItems.begin(), it) : max;
	}

	// Make sure there's enough space allocated
	if (slot >= max)
		inventoryItems.resize(slot + 1, nullptr);

	// Insert the item pointer
	inventoryItems[slot] = item;
}

void Inventory::clear() {
	// Purge all equipped item instances
	for (std::vector<Item *>::iterator item = equippedItems.begin(); item != equippedItems.end(); ++item)
		if (*item)
			delete *item;

	// Purge all inventory item instances
	for (std::vector<Item *>::iterator item = inventoryItems.begin(); item != inventoryItems.end(); ++item)
		if (*item)
			delete *item;

	// Clear the maps
	equippedItems.clear();
	inventoryItems.clear();
}

InventorySlot Inventory::getSlotFromBitFlag(uint32_t bitFlag) const {
	// Bit flags used for the 'EquipableSlots' column of 'baseitem.2da'
	static const uint32_t kSlotBitFlagHead      = (uint32_t)(0x1 << (int) kInventorySlotHead);
	static const uint32_t kSlotBitFlagChest     = (uint32_t)(0x1 << (int) kInventorySlotChest);
	static const uint32_t kSlotBitFlagBoots     = (uint32_t)(0x1 << (int) kInventorySlotBoots);
	static const uint32_t kSlotBitFlagArms      = (uint32_t)(0x1 << (int) kInventorySlotArms);
	static const uint32_t kSlotBitFlagRightHand = (uint32_t)(0x1 << (int) kInventorySlotRightHand);
	static const uint32_t kSlotBitFlagLeftHand  = (uint32_t)(0x1 << (int) kInventorySlotLeftHand);
	static const uint32_t kSlotBitFlagCloak     = (uint32_t)(0x1 << (int) kInventorySlotCloak);
	static const uint32_t kSlotBitFlagLeftRing  = (uint32_t)(0x1 << (int) kInventorySlotLeftRing);
	static const uint32_t kSlotBitFlagRightRing = (uint32_t)(0x1 << (int) kInventorySlotRightRing);
	static const uint32_t kSlotBitFlagNeck      = (uint32_t)(0x1 << (int) kInventorySlotNeck);
	static const uint32_t kSlotBitFlagBelt      = (uint32_t)(0x1 << (int) kInventorySlotBelt);
	static const uint32_t kSlotBitFlagArrows    = (uint32_t)(0x1 << (int) kInventorySlotArrows);
	static const uint32_t kSlotBitFlagBullets   = (uint32_t)(0x1 << (int) kInventorySlotBullets);
	static const uint32_t kSlotBitFlagBolts     = (uint32_t)(0x1 << (int) kInventorySlotBolts);
	static const uint32_t kSlotBitFlagCWeaponL  = (uint32_t)(0x1 << (int) kInventorySlotCWeaponL);
	static const uint32_t kSlotBitFlagCWeaponR  = (uint32_t)(0x1 << (int) kInventorySlotCWeaponR);
	static const uint32_t kSlotBitFlagCWeaponB  = (uint32_t)(0x1 << (int) kInventorySlotCWeaponB);
	static const uint32_t kSlotBitFlagCArmour   = (uint32_t)(0x1 << (int) kInventorySlotCArmour);

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
			size_t slot = item.getUint("Repos_Index");
			insertItem(new Item(item), slot);
		}
	}

	// Load equipped items (creature only)
	if (inventory.hasField("Equip_ItemList")) {
		// Allocate space for all slots
		size_t capacity = equippedItems.capacity();
		if (capacity < (size_t) kInventorySlotMax)
			equippedItems.resize((size_t) kInventorySlotMax, nullptr);

		const Aurora::GFF3List &itemList = inventory.getList("Equip_ItemList");
		for (Aurora::GFF3List::const_iterator it = itemList.begin(); it != itemList.end(); ++it) {
			const Aurora::GFF3Struct &item = **it;

			// Slot number is given by the struct id bit flag
			size_t slot = getSlotFromBitFlag(item.getID());

			// Check for an open equipment slot
			if (slot < (size_t) kInventorySlotMax && equippedItems[slot] == nullptr) {
				equippedItems[slot] = new Item(item);
			} else {
				// Move to the general inventory instead
				insertItem(new Item(item));
			}
		}
	}
}

} // End of namespace NWN2

} // End of namespace Engines
