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
 *  Collection of inventory items in KotOR games.
 */

#ifndef ENGINES_KOTORBASE_INVENTORY_H
#define ENGINES_KOTORBASE_INVENTORY_H

#include <map>

#include "src/common/ustring.h"

namespace Engines {

namespace KotORBase {

class Inventory {
public:
	struct ItemGroup {
		Common::UString tag;
		int count;
	};

	const std::map<Common::UString, ItemGroup> &getItems() const;

	bool hasItem(const Common::UString &tag) const;

	void addItem(const Common::UString &tag, int count = 1);
	void removeItem(const Common::UString &tag, int count = 1);
	void removeAllItems();

private:
	typedef std::map<Common::UString, ItemGroup> ItemMap;

	ItemMap _items;
};

} // End of namespace KotORBase

} // End of namespace Engines

#endif // ENGINES_KOTORBASE_INVENTORY_H
