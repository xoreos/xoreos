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
 *  Handles the party management in KotOR games.
 */

#ifndef ENGINES_KOTORBASE_PARTYCONTROLLER_H
#define ENGINES_KOTORBASE_PARTYCONTROLLER_H

#include <vector>
#include <map>

#include "src/common/ustring.h"

namespace Engines {

namespace KotORBase {

class Module;
class Creature;

class PartyController {
public:
	PartyController(Module *module);

	// Current party

	/** Get count of members in the party including the party leader. */
	size_t getPartyMemberCount() const;
	/** Get the party leader. */
	Creature *getPartyLeader() const;
	/** Get the party member at a given index (0 is always the party leader). */
	const std::pair<int, Creature *> &getPartyMemberByIndex(int index) const;
	/** Get a list of the party members. */
	std::vector<int> getPartyMembers() const;

	/** Is a specified creature a party member? */
	bool isObjectPartyMember(Creature *creature) const;

	/** Clear the current party. */
	void clearCurrentParty();
	/** Add a creature to the party. */
	void addPartyMember(int npc, Creature *creature);
	/** Set which party member should be the controlled character. */
	void setPartyLeader(int npc);
	/** Set which party member should be the controlled character. */
	void setPartyLeaderByIndex(int index);

	// Available party

	/** Get a NPC template from the list of available party members. */
	const Common::UString &getAvailableNPCTemplate(int npc) const;

	/** Is a NPC in the list of available party members? */
	bool isAvailableCreature(int npc) const;

	/** Clear the list of available party members. */
	void clearAvailableParty();
	/** Add a NPC to the list of available party members using a template. */
	void addAvailableNPCByTemplate(int npc, const Common::UString &templ);

private:
	Module *_module;
	std::vector<std::pair<int, Creature *>> _party;
	std::map<int, Common::UString> _availableParty;
};

} // End of namespace KotORBase

} // End of namespace Engines

#endif // ENGINES_KOTORBASE_PARTYCONTROLLER_H
