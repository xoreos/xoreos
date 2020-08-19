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
 *  A journal in a Neverwinter Nights 2 module or campaign.
 */

#ifndef ENGINES_NWN2_JOURNAL_H
#define ENGINES_NWN2_JOURNAL_H

#include <vector>

#include "src/common/ustring.h"

namespace Engines {

namespace NWN2 {

class Journal {
public:
	Journal(const Aurora::GFF3Struct &journal);
	~Journal() = default;

	/** Get the XP awarded for completing the quest with the matching tag. */
	uint32_t getJournalQuestExperience(const Common::UString &plotID) const;

private:

	struct Entry {
		Common::UString text;
		Common::UString comment;
		uint32_t id;
		bool end;
	};

	struct Category {
		Common::UString identifier;
		Common::UString name;
		Common::UString comment;
		uint32_t priority;
		uint32_t XP;
		std::vector<Entry> entries;
	};

	std::vector<Category> _categories;

	/** Load from a journal instance. */
	void load(const Aurora::GFF3Struct &journal);
};

} // End of namespace NWN2

} // End of namespace Engines

#endif // ENGINES_NWN2_JOURNAL_H
