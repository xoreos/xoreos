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
 *  An item in a Neverwinter Nights 2 area.
 */

#ifndef ENGINES_NWN2_ITEM_H
#define ENGINES_NWN2_ITEM_H

#include <vector>

#include "src/engines/nwn2/types.h"
#include "src/engines/nwn2/object.h"
#include "src/engines/nwn2/inventory.h"

namespace Engines {

namespace NWN2 {

class ItemProperty;

class Item : public Object, public Inventory {
public:
	Item(const Aurora::GFF3Struct &item);
	Item(const Common::UString &blueprint, uint16 stackSize, const Common::UString &tag);
	~Item();

	bool getDroppableFlag() const;
	bool getIdentified() const;
	bool getItemCursedFlag() const;
	bool getPickpocketableFlag() const;
	bool getPlotFlag() const;
	bool getStolenFlag() const;

	void setDroppableFlag(bool droppable);
	void setIdentified(bool identified);
	void setItemCursedFlag(bool cursed);
	void setPickpocketableFlag(bool pickpocketable);
	void setPlotFlag(bool plotFlag);
	void setStolenFlag(bool stolen);

	ItemType getBaseItemType() const;
	uint32 getItemIcon() const;
	uint16 getItemStackSize() const;
	uint16 getMaxStackSize() const;
	uint8 getItemCharges() const;

	void setItemIcon(uint32 icon);
	void setItemStackSize(uint16 stackSize);
	void setItemCharges(uint8 charges);

	/** Create an item in the creature's inventory. */
	Item *createItemOnObject(const Common::UString &blueprint, uint16 stackSize, const Common::UString &tag);

private:
	typedef std::vector<ItemProperty> ItemProperties;

	uint32 _icon;       ///< Icon number for inventory UI.
	uint32 _cost;       ///< Base price in gp.
	int32 _modifyCost;  ///< Adjustment to price in gp.
	uint16 _stackSize;  ///< Stack size.
	uint8 _charges;     ///< Number of charges.
	ItemType _baseItem; ///< Base item type.

	bool _plot;           ///< Is this a plot item?
	bool _cursed;         ///< Is the item cursed?
	bool _stolen;         ///< Was the item stolen?
	bool _droppable;      ///< Is the item dropped as loot?
	bool _identified;     ///< Have the item's properties been identified?
	bool _pickpocketable; ///< Can the item be pick-pocketed?

	ItemProperties _itemProperties; ///< The item's properties.

	/* Load from an item instance. */
	void load(const Aurora::GFF3Struct &item);
	void load(const Common::UString &blueprint, uint16 stackSize, const Common::UString &tag);
	void load(const Aurora::GFF3Struct &instance, const Aurora::GFF3Struct *blueprint);
	void loadProperties(const Aurora::GFF3Struct &gff);
	void loadItemProperties(const Aurora::GFF3Struct &gff);
};

} // End of namespace NWN2

} // End of namespace Engines

#endif // ENGINES_NWN2_ITEM_H
