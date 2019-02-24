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

#include "src/engines/nwn2/types.h"
#include "src/engines/nwn2/object.h"

namespace Engines {

namespace NWN2 {

class Item : public Object {
public:
	Item(const Aurora::GFF3Struct &item);
	~Item();

private:
	uint32 _icon;       ///< Icon number for inventory UI.
	uint32 _cost;       ///< Base price in gp.
	int32 _modifyCost;  ///< Adjustment to price in gp.
	uint16 _stackSize;  ///< Stack size.
	ItemType _baseItem; ///< Base item type.

	bool _plot;           ///< Is this a plot item?
	bool _cursed;         ///< Is the item cursed?
	bool _stolen;         ///< Was the item stolen?
	bool _droppable;      ///< Is the item dropped as loot?
	bool _identified;     ///< Have the item's properties been identified?
	bool _pickpocketable; ///< Can the item be pick-pocketed?

	/* Load from an item instance. */
	void load(const Aurora::GFF3Struct &item);
	void load(const Aurora::GFF3Struct &instance, const Aurora::GFF3Struct *blueprint);
	void loadProperties(const Aurora::GFF3Struct &gff);
};

} // End of namespace NWN2

} // End of namespace Engines

#endif // ENGINES_NWN2_ITEM_H
