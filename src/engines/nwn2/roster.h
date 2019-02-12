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
 *  Roster in a Neverwinter Nights 2 module.
 */

#ifndef ENGINES_NWN2_ROSTER_H
#define ENGINES_NWN2_ROSTER_H

#include <list>

#include "src/common/ustring.h"

#include "src/aurora/types.h"

namespace Engines {

namespace NWN2 {

/** Roster for module. */
class Roster {

public:
	Roster();
	~Roster();

	/** Add a creature to the roster. */
	bool addRosterMemberByTemplate(Common::UString name, Common::UString cTemplate);

	/** Set to the first member in the list. */
	Common::UString getFirstRosterMember();
	/** Increment to the next member in the list. */
	Common::UString getNextRosterMember();

	/** Fetch roster member information. */
	bool getIsRosterMemberAvailable(const Common::UString &name) const;
	bool getIsRosterMemberCampaignNPC(const Common::UString &name) const;
	bool getIsRosterMemberSelectable(const Common::UString &name) const;

	/** Set roster member information. */
	bool setIsRosterMemberAvailable(const Common::UString &name, bool available);
	bool setIsRosterMemberCampaignNPC(const Common::UString &name, bool campaignNPC);
	bool setIsRosterMemberSelectable(const Common::UString &name, bool selectable);

protected:
	void loadMember(const Aurora::GFF3Struct &gff);

private:
	struct Member {
		Member() : rosterName(""),      cTemplate(""),
		           isAvailable(false),  isCampaignNPC(false),
		           isSelectable(false), isLoadBefore(false) {}

		Common::UString rosterName; ///< Unique roster name.
		Common::UString cTemplate;  ///< Creature template name.
		bool isAvailable;           ///< Not currently in a party?
		bool isCampaignNPC;         ///< Is it set as a Campaign NPC?
		bool isSelectable;          ///< Can it be added to a party?
		bool isLoadBefore;          ///< Has it been loaded before?
	};

	std::list<Member> _members; ///< List of roster member instances.

	uint32 _lastRetrieved; ///< Index of last retrieved member.

	/** Load members from 'ROSTER.rst' file. */
	void load();
};

} // End of namespace NWN2

} // End of namespace Engines

#endif // ENGINES_NWN2_FACTION_H
