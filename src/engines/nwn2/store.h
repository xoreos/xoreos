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

#include "src/engines/nwn2/types.h"
#include "src/engines/nwn2/object.h"

namespace Engines {

namespace NWN2 {

class Store : public Object {
public:
	/** Load from a store instance. */
	Store(const Aurora::GFF3Struct &store);
	~Store();

	/** Get the charge for identifying an item. */
	int32 getStoreIdentifyCost() const;
	/** Get the amount of gold the store has. */
	int32 getStoreGold() const;
	/** Get the maximum amount the store will pay for an item. */
	int32 getStoreMaximumBuyPrice() const;

	/** Set the charge for identifying an item. */
	void setStoreIdentifyCost(int32 identify);
	/** Set the amount of gold the store has. */
	void setStoreGold(int32 gold);
	/** Set the maximum amount the store will pay for an item. */
	void setStoreMaximumBuyPrice(int32 max);

private:
	int32  _identifyPrice; ///< Charge for identifying items.
	int32  _storeGold;     ///< How many gp the store has available.
	int32  _maxBuyPrice;   ///< The maximum amount this store will pay for an item.
	uint16 _markUp;        ///< Percentage markup on items for sale from store.
	uint16 _markDown;      ///< Percentage markdown on store purchases of items.
	uint16 _bmMarkDown;    ///< Percentage markdown on store purchases of stolen items.
	bool   _blackMarket;   ///< Whether store will purchase stolen goods.

	/** Load from a store instance. */
	void load(const Aurora::GFF3Struct &store);
	/** Load the store from an instance and its blueprint. */
	void load(const Aurora::GFF3Struct &instance, const Aurora::GFF3Struct *blueprint);

	/** Load general store properties. */
	void loadProperties(const Aurora::GFF3Struct &gff);
};

} // End of namespace NWN2

} // End of namespace Engines

#endif // ENGINES_NWN2_STORE_H
