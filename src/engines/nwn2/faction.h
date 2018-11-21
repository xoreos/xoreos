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
 *  Factions in a Neverwinter Nights 2 module.
 */

#ifndef ENGINES_NWN2_FACTION_H
#define ENGINES_NWN2_FACTION_H

#include <vector>

#include "src/engines/nwn2/object.h"

namespace Engines {

namespace NWN2 {

// Track reputation of other creatures
class PersonalReputation {
public:
	PersonalReputation();
	~PersonalReputation();

	void clear();

	bool getPersonalRep(Object *source, uint8 *reputation);
	void setPersonalRep(Object *source, uint8 amount, bool decays = false, uint16 duration = 180);
	void clearPersonalRep(Object *source);

private:
	struct PersonalRep {
		uint8 amount;    ///< Revised reputation.
		uint32 day;      ///< Game day when this reputation was created.
		uint32 time;     ///< Game time when this reputation was created.
		bool decays;     ///< True if reputation adjustment decays at a set time.
		uint16 duration; ///< Duration in seconds of the reputation adjustment.
		uint32 objectId; ///< Object ID of the other creature for which rep applies.
	};

	std::vector<PersonalRep> _reputation;

	void decayPersonalRep();
};

// Factions in module
class Factions {
public:
	Factions(bool use2da = false);
	~Factions();

	/** Get source's reputation with faction */
	uint8 getReputation(Object *source, uint32 faction);
	/** Get the faction name */
	Common::UString getFactionName(uint32 faction);

protected: // Allow testing access
	struct Faction {
		Common::UString name; ///< Name of the faction.
		bool global;          ///< Global effect flag.
	};
	struct Reputation {
		uint32 factionId1;    ///< Index into _factionList.
		uint32 factionId2;    ///< Index into _factionList.
		uint32 factionRep;    ///< How faction Id1 perceives faction Id2.
	};

	size_t                  _count;       ///< Number of factions.
	std::vector<Faction>    _factionList; ///< List of factions.
	std::vector<Reputation> _repList;     ///< List of reputations.

	void loadFac();
	void load2da();
};

} // End of namespace NWN2

} // End of namespace Engines

#endif // ENGINES_NWN2_FACTION_H
