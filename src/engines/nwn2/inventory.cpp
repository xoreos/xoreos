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
	uint16 equipSize = (uint16) kInventorySlotMax;

	// Cycle through equipped items
	_lastRetrieved = 0;
	for (std::vector<Item *>::const_iterator item = equippedItems.begin(); item != equippedItems.end(); ++item) {
		if (*item != nullptr)
			return *item;

		// Stay in bounds
		if (++_lastRetrieved >= equipSize)
			break;
	}

	// Cycle through the inventory
	_lastRetrieved = equipSize;
	for (std::vector<Item *>::const_iterator item = inventoryItems.begin(); item != inventoryItems.end(); ++item) {
		if (*item != nullptr)
			return *item;

		_lastRetrieved++;
	}

	return nullptr;
}

/** Return the next item in the inventory. */
Item *Inventory::getNextItemInInventory() {
	// Check for the end of inventory
	if (_lastRetrieved == UINT16_MAX)
		return nullptr;

	// Increment the counter
	_lastRetrieved++;

	// Cycle through the remaining equipped items
	const uint16 equipSize = (uint16) kInventorySlotMax;
	if (_lastRetrieved < equipSize) {
		std::vector<Item *>::const_iterator item = equippedItems.begin();
		std::advance(item, _lastRetrieved);
		for (; item != equippedItems.end(); item++) {
			if (*item != nullptr)
				return *item;

			// Stay in bounds
			if(++_lastRetrieved >= equipSize)
				break;
		}
	}

	// Cycle through the remaining inventory
	const size_t inventorySize = inventoryItems.size();
	if (_lastRetrieved < equipSize + inventorySize) {
		// TODO: Check for an item with nested inventory (Ex.: magic bag)
		std::vector<Item *>::const_iterator item = inventoryItems.begin();
		std::advance(item, _lastRetrieved - equipSize);
		for (; item != inventoryItems.end(); item++)
			if (*item != nullptr) {
				return *item;
			_lastRetrieved++;
		}
	}

	// End of the inventory
	_lastRetrieved = UINT16_MAX;
	return nullptr;
}

/** Return the item in a slot. */
Item *Inventory::getItemInSlot(InventorySlot slot) const {
	return (slot < kInventorySlotMax) ? equippedItems[(uint16) slot] : nullptr;
}

Item *Inventory::createItem(const Common::UString &blueprint, uint16 stackSize, const Common::UString &tag) {
	if (blueprint.empty() || stackSize == 0)
		return nullptr;

	// Create and insert the item
	Item *item = new Item(blueprint, stackSize, tag);
	insertItem(item);
	return item;
}

void Inventory::insertItem(Item *item, uint16 slot) {
	if (item == nullptr)
		return;

	// Determine the maximum slot to be occupied
	size_t max = inventoryItems.size();
	if (slot > max)
		max = slot;

	// Make sure there's enough space allocated
	if (max + 2 > inventoryItems.capacity())
		inventoryItems.resize(max + 8, nullptr); // Add a full row

	// Find an empty inventory slot
	if (inventoryItems[slot] != nullptr)
		slot = getFirstEmptySlot();

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

uint16 Inventory::getFirstEmptySlot() const {
	// Look for the first open slot
	uint16 slot = 0;

	for (std::vector<Item *>::const_iterator item = inventoryItems.begin(); item != inventoryItems.end(); ++item) {
		if (*item == nullptr)
			return slot;
		slot++;
	}

	// Failsafe
	return slot;
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
			uint16 slot = getSlotFromBitFlag(item.getID());

			// Check for an open equipment slot
			if (slot < (uint16) kInventorySlotMax && equippedItems[slot] == nullptr) {
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
