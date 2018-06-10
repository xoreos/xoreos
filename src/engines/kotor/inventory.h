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
 *  Collection of items.
 */

#ifndef ENGINES_KOTOR_INVENTORY_H
#define ENGINES_KOTOR_INVENTORY_H

#include <map>

#include <src/common/ustring.h>

namespace Engines {

namespace KotOR {

struct InventoryItem {
	Common::UString tag;
	int count;
};

class Inventory {
public:
	void addItem(const Common::UString &tag, int count = 1);
	void removeItem(const Common::UString &tag, int count = 1);

	const std::map<Common::UString, InventoryItem> &getItems() const;
	bool hasItem(const Common::UString &tag) const;
private:
	typedef std::map<Common::UString, InventoryItem> ItemMap;

	ItemMap _items;
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_INVENTORY_H
