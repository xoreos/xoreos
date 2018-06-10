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

#include "src/engines/kotor/inventory.h"

namespace Engines {

namespace KotOR {

void Inventory::addItem(const Common::UString &tag, int count) {
	assert(count > 0);

	ItemMap::iterator i = _items.find(tag);
	if (i == _items.end()) {
		InventoryItem item;
		item.tag = tag;
		item.count = count;
		_items.insert(std::pair<Common::UString, InventoryItem>(tag, item));
	} else
		i->second.count += count;
}

void Inventory::removeItem(const Common::UString &tag, int count) {
	assert(count > 0);

	ItemMap::iterator i = _items.find(tag);
	if (i == _items.end())
		return;

	if (i->second.count > count)
		i->second.count -= count;
	else
		_items.erase(i);
}

const std::map<Common::UString, InventoryItem> &Inventory::getItems() const {
	return _items;
}

bool Inventory::hasItem(const Common::UString &tag) const {
	return _items.find(tag) != _items.end();
}

} // End of namespace KotOR

} // End of namespace Engines
