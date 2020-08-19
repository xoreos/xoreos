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

#ifndef ENGINES_NWN2_STORE_H
#define ENGINES_NWN2_STORE_H

#include <memory>

#include "src/engines/nwn2/types.h"
#include "src/engines/nwn2/object.h"

namespace Engines {

namespace NWN2 {

class Inventory;
class Item;

class Store : public Object {
public:
	/** Load from a store instance. */
	Store(const Aurora::GFF3Struct &store);
	~Store();

	/** Get the charge for identifying an item. */
	int32_t getStoreIdentifyCost() const;
	/** Get the amount of gold the store has. */
	int32_t getStoreGold() const;
	/** Get the maximum amount the store will pay for an item. */
	int32_t getStoreMaximumBuyPrice() const;

	/** Set the charge for identifying an item. */
	void setStoreIdentifyCost(int32_t identify);
	/** Set the amount of gold the store has. */
	void setStoreGold(int32_t gold);
	/** Set the maximum amount the store will pay for an item. */
	void setStoreMaximumBuyPrice(int32_t max);

	/** Get first item in inventory array. */
	Item *getFirstItemInInventory();
	/** Get next item in inventory array. */
	Item *getNextItemInInventory();

private:
	typedef std::vector<uint8_t> BaseItemTypes;

	typedef uint32_t InventoryType;

	static const InventoryType kInventoryTypes = 5; ///< Number of store tabs.

	std::unique_ptr<Inventory> _inventory[kInventoryTypes]; ///< Items for sale.

	/** Index of last inventory array member queried to retrieve an item. */
	InventoryType _lastQueried { kInventoryTypes };

	// The _willOnlyBuy list is ignored unless _willNotBuy is empty
	BaseItemTypes _willNotBuy;  ///< Base item types the store will not buy.
	BaseItemTypes _willOnlyBuy; ///< Base item types the store will only buy.

	int32_t  _identifyPrice; ///< Charge for identifying items.
	int32_t  _storeGold;     ///< How many gp the store has available.
	int32_t  _maxBuyPrice;   ///< The maximum amount this store will pay for an item.
	uint16_t _markUp;        ///< Percentage markup on items for sale from store.
	uint16_t _markDown;      ///< Percentage markdown on store purchases of items.
	uint16_t _bmMarkDown;    ///< Percentage markdown on store purchases of stolen items.
	bool   _blackMarket;     ///< Whether store will purchase stolen goods.

	/** Load from a store instance. */
	void load(const Aurora::GFF3Struct &store);
	/** Load the store from an instance and its blueprint. */
	void load(const Aurora::GFF3Struct &instance, const Aurora::GFF3Struct *blueprint);

	/** Load general store properties. */
	void loadProperties(const Aurora::GFF3Struct &gff);
	/** Load BaseItemTypes list. */
	void loadBaseItemTypes(const Aurora::GFF3Struct &gff, const Common::UString field, BaseItemTypes &types);
};

} // End of namespace NWN2

} // End of namespace Engines

#endif // ENGINES_NWN2_STORE_H
