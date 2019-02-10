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
 *  A roster array in a Neverwinter Nights 2 module.
 */

#include "src/aurora/types.h"
#include "src/aurora/resman.h"
#include "src/aurora/gff3file.h"

#include "src/engines/aurora/util.h"

#include "src/engines/nwn2/roster.h"

namespace Engines {

namespace NWN2 {

Roster::Roster() : _lastRetrieved(UINT32_MAX) {
	load();
}

Roster::~Roster() {
}

/** Return the roster name of the first member. */
Common::UString Roster::getFirstRosterMember() {
	_lastRetrieved = 0;

	if (_members.empty())
		return "";

	// Return the first member's name
	return _members.front().rosterName;
}

/** Return the roster name of the next member. */
Common::UString Roster::getNextRosterMember() {
	if (_members.empty() || _lastRetrieved >= _members.size() - 1) {
		_lastRetrieved = UINT32_MAX;
		return "";
	}

	// Find the matching member
	std::list<Member>::const_iterator it = _members.begin();
	std::advance(it, ++_lastRetrieved);
	return (it != _members.end()) ? it->rosterName : "";
}

bool Roster::getIsRosterMemberAvailable(const Common::UString &name) const {
	std::list<Member>::const_iterator it = getRosterMember(name);
	return (it != _members.end()) ? it->isAvailable : false;
}

bool Roster::getIsRosterMemberCampaignNPC(const Common::UString &name) const {
	std::list<Member>::const_iterator it = getRosterMember(name);
	return (it != _members.end()) ? it->isCampaignNPC : false;
}

bool Roster::getIsRosterMemberSelectable(const Common::UString &name) const {
	std::list<Member>::const_iterator it = getRosterMember(name);
	return (it != _members.end()) ? it->isSelectable : false;
}

std::list<Roster::Member>::const_iterator Roster::getRosterMember(const Common::UString &name) const {
	for (std::list<Member>::const_iterator it = _members.begin(); it != _members.end(); ++it)
		if (it->rosterName == name)
			return it;

	return _members.end();
}

/** Load roster from the saved 'ROSTER.rst' file, if any. */
void Roster::load() {
	Common::ScopedPtr<Aurora::GFF3File> gff(loadOptionalGFF3("ROSTER", Aurora::kFileTypeRST, MKTAG('R', 'S', 'T', ' ')));
	if (!gff)
		return;

	const Aurora::GFF3Struct &top = gff->getTopLevel();

	// Insert the roster members
	const Aurora::GFF3List &rList = top.getList("RosMembers");
	for (Aurora::GFF3List::const_iterator it = rList.begin(); it != rList.end(); ++it)
		loadMember(**it);
}

void Roster::loadMember(const Aurora::GFF3Struct &gff) {
	Member member;

	// Add a roster member to the list
	if (gff.hasField("RosName")) {
		member.rosterName = gff.getString("RosName", member.rosterName);
		member.isAvailable = gff.getBool("RosAvailable", member.isAvailable);
		member.isCampaignNPC = gff.getBool("RosCampaignNPC", member.isCampaignNPC);
		member.isSelectable = gff.getBool("RosSelectable", member.isSelectable);
		member.isLoadBefore = gff.getBool("RosLoadBefore", member.isLoadBefore);

		if (getRosterMember(member.rosterName) == _members.end()) {
			_members.push_back(member);
		} else {
			throw Common::Exception("Attempt to load duplicate roster member name: \"%s\"", member.rosterName.c_str());
		}
	} else {
		throw Common::Exception("Missing RosName field.");
	}
}

} // End of namespace NWN2

} // End of namespace Engines
