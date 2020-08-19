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
 *  A journal in a Neverwinter Nights 2 module.
 */

#include <algorithm>

#include "src/aurora/gff3file.h"

#include "src/engines/nwn2/journal.h"

namespace Engines {

namespace NWN2 {

Journal::Journal(const Aurora::GFF3Struct &journal) {

	load(journal);
}

uint32_t Journal::getJournalQuestExperience(const Common::UString &plotID) const {
	auto category = std::find_if(_categories.begin(), _categories.end(), [&](const Category &c) {
		return c.name == plotID;
	});

	return (category != _categories.end()) ? category->XP : 0;
}

void Journal::load(const Aurora::GFF3Struct &journal) {
	if (!journal.hasField("Categories"))
		return;

	const Aurora::GFF3List &categories = journal.getList("Categories");
	for (const Aurora::GFF3Struct *c : categories) {
		Category category;

		category.name       = c->getString("Name");
		category.identifier = c->getString("Tag");
		category.comment    = c->getString("Comment");
		category.priority   = c->getUint("Priority");
		category.XP         = c->getUint("XP");

		category.entries.clear();
		if (c->hasField("EntryList")) {
			const Aurora::GFF3List &entries = c->getList("EntryList");
			for (const Aurora::GFF3Struct *e : entries) {
				Entry entry;

				entry.text    = e->getString("Text");
				entry.comment = e->getString("Comment");
				entry.id      = e->getUint("ID");
				entry.end     = (e->getUint("End") != 0);

				category.entries.push_back(entry);
			}
		}

		_categories.push_back(category);
	}
}

} // End of namespace NWN2

} // End of namespace Engines
