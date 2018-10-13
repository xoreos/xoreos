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
 *  A factions array in a Neverwinter Nights 2 module.
 */

#include "src/aurora/2dafile.h"
#include "src/aurora/2dareg.h"

#include "src/engines/nwn2/faction.h"

namespace Engines {

namespace NWN2 {

/* ---- PersonalReputation class ---- */

PersonalReputation::PersonalReputation() {
}

PersonalReputation::~PersonalReputation() {
}

void PersonalReputation::clear() {
	_reputation.clear();
}

/**
 * If the source has a personal reputation for this
 * creature, set reputation argument to that value and
 * return true. Otherwise return false.
 */
bool PersonalReputation::getPersonalRep(Object *source, uint8 *reputation) {
	// Quick bypass for most objects
	if (_reputation.size() == 0 || source == 0)
		return false;

	return false; // TODO
}

/**  Set a personal reputation of the creature with the subject. */
void PersonalReputation::setPersonalRep(Object *source, uint8 reputation) {
	assert(source);
	// TODO
}

/** Clear a personal reputation the target has for the creature. */
void PersonalReputation::clearPersonalRep(Object *subject) {
	// Quick bypass for most objects
	if (_reputation.size() == 0 || subject == 0)
		return;

	// TODO
}

/* ---- FactionList class ---- */

Factions::Factions() {
	load();
}

Factions::~Factions() {
}

/** Get the source's reputation with the faction. */
uint8 Factions::getReputation(Object *source, uint32 faction) {
	assert(source);
	assert(faction < _factionList.size());
	return 0; // TODO
}

/** Get the faction name. */
Common::UString Factions::getFactionName(uint32 faction) {
	assert(faction < _factionList.size());
	return _factionList[faction].name;
}

/** Load standard factions from the 'repute.2da' file. */
void Factions::load() {
	const Aurora::TwoDAFile &repute = TwoDAReg.get2DA("repute");
	size_t rows = repute.getRowCount();
	Faction faction;
	Reputation rep;

	// Set the number of factions for lookups
	_count = rows + 1;

	// Insert a player faction row for padding
	faction.name = repute.getHeaders().at(1);
	faction.global = true;
	_factionList.push_back(faction);
	for (size_t id1 = 0; id1 < _count; id1++) {
		rep.factionId1 = id1;
		rep.factionId2 = kStandardPC;
		rep.factionRep = 100; // Per Aurora documentation
		_repList.push_back(rep);
	}

	// Add the remaining standard factions
	for (size_t id2 = 0; id2 < rows; id2++) {
		const Aurora::TwoDARow &row = TwoDAReg.get2DA("repute").getRow(id2);

		// Add a faction entry to the array
		faction.name = row.getString(0);
		faction.global = true;
		_factionList.push_back(faction);

		// Populate the reputation array
		for (size_t id1 = 0; id1 < _count; id1++) {
			rep.factionId1 = id1;     // Target faction
			rep.factionId2 = id2 + 1; // Source faction

			// How faction id2 feels about id1
			rep.factionRep = row.getInt(id1);
			_repList.push_back(rep);
		}
	}
}

} // End of namespace NWN2

} // End of namespace Engines
