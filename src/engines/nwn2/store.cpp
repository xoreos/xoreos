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
 *  A store in a Neverwinter Nights 2 area.
 */

#include "src/common/endianness.h"
#include "src/common/maths.h"
#include "src/common/error.h"

#include "src/aurora/gff3file.h"

#include "src/engines/aurora/util.h"

#include "src/engines/nwn2/inventory.h"
#include "src/engines/nwn2/store.h"

namespace Engines {

namespace NWN2 {

Store::Store(const Aurora::GFF3Struct &store) : Object(kObjectTypeStore) {

	load(store);
}

Store::~Store() {
}

int32_t Store::getStoreIdentifyCost() const {
	return _identifyPrice;
}

int32_t Store::getStoreGold() const {
	return _storeGold;
}

int32_t Store::getStoreMaximumBuyPrice() const {
	return _maxBuyPrice;
}

void Store::setStoreIdentifyCost(int32_t identify) {
	// -1 indicates store will not identify items
	_identifyPrice = (identify < 0) ? -1 : identify;
}

void Store::setStoreGold(int32_t gold) {
	// -1 indicates it is not using gold
	_storeGold = (gold < 0) ? -1 : gold;
}

void Store::setStoreMaximumBuyPrice(int32_t max) {
	// -1 indicates price unlimited
	_maxBuyPrice  = (max < 0) ? -1 : max;
}

Item *Store::getFirstItemInInventory() {
	// Cycle through the inventory types
	for (InventoryType it = 0; it < kInventoryTypes; it++)
		if (_inventory[it] != nullptr) {
			// Retrieve the first item in this inventory
			Item *item = _inventory[it]->getFirstItemInInventory();
			if (item != nullptr) {
				_lastQueried = it;
				return item;
			}
		}

	// Reached end of inventory
	_lastQueried = kInventoryTypes;
	return nullptr;
}

Item *Store::getNextItemInInventory() {
	if (_lastQueried >= kInventoryTypes)
		return nullptr;

	// Check the last queried inventory for another item
	if (_inventory[_lastQueried] != nullptr) {
		Item *nextItem = _inventory[_lastQueried]->getNextItemInInventory();
		if (nextItem != nullptr)
			return nextItem;
	}

	// Cycle through the remaining inventory types
	for (InventoryType it = _lastQueried + 1; it < kInventoryTypes; it++)
		if (_inventory[it] != nullptr) {
			// Retrieve the first item in this inventory
			Item *item = _inventory[it]->getFirstItemInInventory();
			if (item != nullptr) {
				_lastQueried = it;
				return item;
			}
		}

	// Reached end of inventory
	_lastQueried = kInventoryTypes;
	return nullptr;
}

void Store::load(const Aurora::GFF3Struct &store) {
	Common::UString temp = store.getString("TemplateResRef");

	std::unique_ptr<Aurora::GFF3File> utm;
	if (!temp.empty())
		utm.reset(loadOptionalGFF3(temp, Aurora::kFileTypeUTM, MKTAG('U', 'T', 'M', ' ')));

	load(store, utm ? &utm->getTopLevel() : 0);
}

void Store::load(const Aurora::GFF3Struct &instance, const Aurora::GFF3Struct *blueprint) {
	// General properties

	if (blueprint)
		loadProperties(*blueprint); // Blueprint
	loadProperties(instance);    // Instance

	// Position

	setPosition(instance.getDouble("XPosition"),
	            instance.getDouble("YPosition"),
	            instance.getDouble("ZPosition"));

	// Orientation

	float bearingX = instance.getDouble("XOrientation");
	float bearingY = instance.getDouble("YOrientation");

	setOrientation(0.0f, 0.0f, 1.0f, -Common::rad2deg(atan2(bearingX, bearingY)));
}

void Store::loadProperties(const Aurora::GFF3Struct &gff) {
	// Tag
	_tag = gff.getString("Tag", _tag);

	// Store parameters
	_identifyPrice = gff.getSint("IdentifyPrice", _identifyPrice);
	_storeGold = gff.getSint("StoreGold", _storeGold);
	_maxBuyPrice = gff.getSint("MaxBuyPrice", _maxBuyPrice);
	_markUp = gff.getUint("MarkUp", _markUp);
	_markDown = gff.getUint("MarkDown", _markDown);
	_bmMarkDown = gff.getUint("BM_MarkDown", _bmMarkDown);
	_blackMarket = gff.getBool("BlackMarket", _blackMarket);

	// Inventory
	const Aurora::GFF3List &sLists = gff.getList("StoreList");
	for (Aurora::GFF3List::const_iterator s = sLists.begin(); s != sLists.end(); ++s) {
		InventoryType id = (*s)->getID();
		if (id < kInventoryTypes)
			_inventory[id] = std::make_unique<Inventory>(**s);
		else
			throw Common::Exception("Invalid store inventory type '%d' for tag \"%s\"", id, _tag.c_str());
	}

	// Load base item types
	loadBaseItemTypes(gff, "WillNotBuy", _willNotBuy);
	loadBaseItemTypes(gff, "WillOnlyBuy", _willOnlyBuy);

	// Scripts and variables
	readScripts(gff);
	readVarTable(gff);
}

void Store::loadBaseItemTypes(const Aurora::GFF3Struct &gff, const Common::UString field, BaseItemTypes &types) {
	if (!gff.hasField(field))
		return;

	const Aurora::GFF3List &list = gff.getList(field);
	for (Aurora::GFF3List::const_iterator b = list.begin(); b != list.end(); ++b) {
		// Make the default value be an error condition
		int32_t baseItem = (*b)->getSint("BaseItem", -1);
		if (baseItem >= 0 && baseItem <= UINT8_MAX)
			types.push_back(static_cast<uint8_t>(baseItem));
		else
			throw Common::Exception("Invalid BaseItem type '%d'", baseItem);
	}
}

} // End of namespace NWN2

} // End of namespace Engines
