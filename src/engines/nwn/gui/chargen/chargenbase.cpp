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

/** @file engines/nwn/gui/chargen/chargenbase.cpp
 *  The character generator base class.
 */


#include "aurora/2dareg.h"
#include "aurora/2dafile.h"

#include "engines/nwn/creature.h"

#include "engines/nwn/gui/chargen/chargenbase.h"

namespace Engines {

namespace NWN {

CharGenBase::CharacterAbilities CharGenBase::_charChoices = CharacterAbilities();

CharGenBase::CharacterAbilities::CharacterAbilities() :
	race(kRaceInvalid), character(0) {

}

CharGenBase::CharacterAbilities::CharacterAbilities(Creature *charac) :
	race(kRaceInvalid), character(charac) {

	// TODO: Get race from creature?
}

void CharGenBase::applyAbilities() {
	_charChoices.character->setRace(_charChoices.race);
}

void CharGenBase::setCharRace(uint32 race) {
	_charChoices.race = race;

	_charChoices.racialFeats.clear();

	if (race == kRaceInvalid)
		return;

	const Aurora::TwoDAFile &twodaRace = TwoDAReg.get("racialtypes");
	const Aurora::TwoDAFile &twodaFeatRace = TwoDAReg.get(
		twodaRace.getRow(race).getString("FeatsTable"));

	for (uint8 it = 0; it < twodaFeatRace.getRowCount(); ++it) {
		const Aurora::TwoDARow &rowFeatRace = twodaFeatRace.getRow(it);
		_charChoices.racialFeats.push_back(rowFeatRace.getInt("FeatIndex"));
	}
}

} // End of namespace NWN

} // End of namespace Engines
