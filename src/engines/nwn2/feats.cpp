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
 *  Feats for a Neverwinter Nights 2 creature.
 */

#include "src/common/debug.h"

#include "src/engines/nwn2/types.h"
#include "src/engines/nwn2/feats.h"

namespace Engines {

namespace NWN2 {

Feats::Feats() {
	clear();
}

Feats::~Feats() {
}

/** Clear all feat information and reset the modifiers */
void Feats::clear() {
	_feats.clear();
	initParameters();
}

/** Add the feat to the list */
void Feats::featAdd(const uint32 id) {
	_feats.push_back(id);
	applyFeat(id);
}

/** Remove the feat from the list */
void Feats::featRemove(const uint32 id) {
	// Look for a matching feat
	for (std::vector<uint32>::const_iterator it = _feats.begin(); it != _feats.end(); ++it) {
		if (*it == id) {
			// Found a match, so remove it
			_feats.erase(it);

			// Rebuild the data
			resetFeats();
			break;
		}
	}
}

/** Return true if the feat is in the list */
bool Feats::getHasFeat(uint32 id) const {
	// Look for a matching feat
	for (std::vector<uint32>::const_iterator it = _feats.begin(); it != _feats.end(); ++it)
		if (*it == id)
			return true;

	return false;
}

/** Return the cumulative modifier for this skill */
int Feats::getFeatsSkillBonus(Skill skill) const {
	assert((skill > -1) && (skill < kSkillMAX));
	return _skillBonus[skill];
}

/** Initialize the data */
void Feats::initParameters() {
	uint i;

	// Skill modifiers
	for (i = 0; i < kSkillMAX; i++)
		_skillBonus[i] = 0;
}

/** Apply all feats */
void Feats::resetFeats() {
	// Reset the parameters
	initParameters();

	// Apply modifiers for each feat
	for (std::vector<uint32>::const_iterator it = _feats.begin(); it != _feats.end(); ++it)
		applyFeat(*it);
}

/**
 * Update the instance data for the feat with row
 * number 'id' in the feats.2da table.
 *
 * If 'isAdd' is true, this change is a feat addition.
 * Otherwise, it is for a feat removal.
 *
 * This function uses a single large switch statement
 * so the compiler can build an indexed branch table.
 */
void Feats::applyFeat(const uint32 id) {
	switch(id) {

		/* ---- PC History feats ---- */

		case kFeatHistorySurvivor:
			// +1 Search, +1 Spot, +1 Survival, -2 Fortitude Saving Throws
			_skillBonus[kSkillSearch]   += +1;
			_skillBonus[kSkillSpot]     += +1;
			_skillBonus[kSkillSurvival] += +1;
			warning("Survivor feat is not fully implemented"); // -2 Fort
			break;

		/* ---- Racial feats ---- */

		case kFeatSkillAffinitySearch:
			// +2 to Search
			_skillBonus[kSkillSearch] += +2;
			break;

		case kFeatPSASearch:
			// +1 to Search
			_skillBonus[kSkillSearch] += +1;
			break;

		/* ---- Skill feats ---- */

		case kFeatNimbleFingers:
			// +2 to Open Locks and Disable Device
			_skillBonus[kSkillOpenLock]     += +2;
			_skillBonus[kSkillDisableDevice] += +2;
			break;

		case kFeatSkillFocusDisableTrap:
			// +3 to Disable Device
			_skillBonus[kSkillDisableDevice] += +3;
			break;

		case kFeatSkillFocusSearch:
			// +3 to Search
			_skillBonus[kSkillSearch] += +3;
			break;

		/* ---- Class feats ---- */

		case kFeatNatureSense:
			// +2 to Survival in all environments and a +2
			// to Search and Spot while in wilderness areas
			_skillBonus[kSkillSurvival] += +3;
			warning("Nature Sense feat is not fully implemented");
			break;

		default:
			warning("Feat %d is not implemented", id);
			break;
	}
}

} // End of namespace NWN2

} // End of namespace Engines
