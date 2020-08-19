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

#include <cassert>

#include "src/common/endianness.h"

#include "src/aurora/types.h"
#include "src/aurora/resman.h"
#include "src/aurora/gff3file.h"
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
bool PersonalReputation::getPersonalRep(Object *source, uint8_t *reputation) {
	// Quick bypass for most objects
	if (_reputation.size() == 0 || source == 0)
		return false;

	// Clean out decayed reputations
	decayPersonalRep();

	// Cycle through the stored reputations, looking for a matching ID
	uint32_t id = source->getID();
	for (std::vector<PersonalRep>::const_iterator it = _reputation.begin(); it != _reputation.end(); ++it) {
		if (id == it->objectId) {
			// Found a matching rep
			*reputation = it->amount;
			return true;
		}
	}

	return false;
}

/**  Set a personal reputation of the creature with the subject. */
void PersonalReputation::setPersonalRep(Object *source, uint8_t reputation, bool decays, uint16_t duration) {
	assert(source);

	// Clean out decayed reputations
	if (_reputation.size() > 0)
		decayPersonalRep();

	// Remove an existing reputation
	clearPersonalRep(source);

	// Initialize the new reputation
	PersonalRep rep;
	rep.amount = reputation;
	rep.decays = decays;
	rep.duration = duration;
	// TODO
	rep.day = 0;
	rep.time = 0;

	// Add to the list
	_reputation.push_back(rep);
}

/** Clear out any decayed reputations */
void PersonalReputation::decayPersonalRep() {
	// Work through the list
	for (std::vector<PersonalRep>::iterator it = _reputation.begin(); it != _reputation.end(); ++it) {
		if (it->decays) {
		/**
		 * TODO:
		 * Compute the decay time then compare to the
		 * game clock. If the time has expired, remove
		 * this reputation then reset the loop.
		 */
		}
	}
}

/** Clear a personal reputation the target has for the creature. */
void PersonalReputation::clearPersonalRep(Object *subject) {
	// Quick bypass for most objects
	if (_reputation.size() == 0 || subject == 0)
		return;

	// Clean out decayed reputations
	decayPersonalRep();

	// Cycle through the stored reputations, looking for a matching ID
	uint32_t id = subject->getID();
	for (std::vector<PersonalRep>::iterator it = _reputation.begin(); it != _reputation.end(); ++it) {
		if (id == it->objectId) {
			// Erase the matching rep
			_reputation.erase(it);

			// Reset the iterator
			it = _reputation.begin();
		}
	}
}

/* ---- FactionList class ---- */

Factions::Factions(bool use2da) {
	if (use2da)
		load2da();
	else
		loadFac();
}

Factions::~Factions() {
}

/** Get the source's reputation with the faction. */
uint8_t Factions::getReputation(Object *source, uint32_t faction) {
	assert(source);
	assert(faction < _factionList.size());

	/*
	 * The _repList data is listed by consecutive rows of
	 * factionId2 with column factionId1. To find how the
	 * 'faction' feels about 'source', look at the row
	 * matching the 'source's faction and the 'faction'
	 * column, then return the cell's factionRep value.
	 */
	uint32_t sFaction = source->getFaction();
	size_t index = (_count * sFaction) + faction;

	assert(index < _repList.size());
	assert(_repList[index].factionId1 == faction);
	assert(_repList[index].factionId2 == sFaction);
	return _repList[index].factionRep;
}

/** Get the faction name. */
Common::UString Factions::getFactionName(uint32_t faction) {
	assert(faction < _factionList.size());
	return _factionList[faction].name;
}

/** Load factions from the 'repute.FAC' file. */
void Factions::loadFac() {
	std::unique_ptr<Common::SeekableReadStream> stream(ResMan.getResource("repute", Aurora::kFileTypeFAC));
	if (!stream)
		throw Common::Exception("No repute.FAC available");

	std::unique_ptr<Aurora::GFF3File> gff = std::make_unique<Aurora::GFF3File>(stream.release(), MKTAG('F', 'A', 'C', ' '));
	const Aurora::GFF3Struct &top = gff->getTopLevel();

	// Insert the factions
	const Aurora::GFF3List &fList = top.getList("FactionList");
	_count = fList.size();
	for (Aurora::GFF3List::const_iterator it = fList.begin(); it != fList.end(); ++it) {
		Faction fac;

		// Add a faction entry to the array
		fac.name = (*it)->getString("FactionName");
		fac.global = (*it)->getUint("FactionGlobal");
		_factionList.push_back(fac);
	}

	// Pre-populate the list with a neutral '50' reputations
	for (size_t row = 0; row < _count; row++)
		for (size_t col = 0; col < _count; col++) {
			Reputation rep;
			rep.factionId1 = col;
			rep.factionId2 = row;
			rep.factionRep = 50;
			_repList.push_back(rep);
		}

	// Insert the reputation list values
	const Aurora::GFF3List &rList = top.getList("RepList");
	for (Aurora::GFF3List::const_iterator it = rList.begin(); it != rList.end(); ++it) {
		Reputation rep;

		// Initialize the reputation
		rep.factionId1 = (*it)->getUint("FactionID1");
		rep.factionId2 = (*it)->getUint("FactionID2");
		rep.factionRep = (*it)->getUint("FactionRep");

		// Bounds checking
		if ((rep.factionId1 < _count) && (rep.factionId2 < _count)) {
			// Insert this reputation entry into the array
			size_t index = (rep.factionId2 * _count) + rep.factionId1;
			_repList[index] = rep;
		}
	}
}

/** Load standard factions from the 'repute.2da' file. */
void Factions::load2da() {
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
