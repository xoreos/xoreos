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
 *  The character choices in the character generator.
 */

#include "src/common/util.h"

#include "src/aurora/2dareg.h"
#include "src/aurora/2dafile.h"

#include "src/engines/nwn/creature.h"

#include "src/engines/nwn/gui/chargen/chargenchoices.h"

namespace Engines {

namespace NWN {

CharGenChoices::CharGenChoices() {
	_creature = new Creature();
}

CharGenChoices::~CharGenChoices() {
	delete _creature;
}

void CharGenChoices::reset() {
	delete _creature;
	_creature = new Creature();

	_racialFeats.clear();
	_normalFeats.clear();
	_classFeats.clear();
}

const Creature& CharGenChoices::getCharacter() {
	return *_creature;
}

void CharGenChoices::setCharGender(uint32 gender) {
	_creature->setGender(gender);
}

void CharGenChoices::setCharRace(uint32 race) {
	if (race == kRaceInvalid) {
		warning("Setting invalid race: %d", race);
		return;
	}

	_creature->setRace(race);

	_racialFeats.clear();

	const Aurora::TwoDAFile &twodaRace = TwoDAReg.get("racialtypes");
	const Aurora::TwoDAFile &twodaFeatRace = TwoDAReg.get(
		twodaRace.getRow(race).getString("FeatsTable"));

	for (uint8 it = 0; it < twodaFeatRace.getRowCount(); ++it) {
		const Aurora::TwoDARow &rowFeatRace = twodaFeatRace.getRow(it);
		_racialFeats.push_back(rowFeatRace.getInt("FeatIndex"));
	}
}

void CharGenChoices::setCharPortrait(const Common::UString &portrait) {
	_creature->setPortrait(portrait);
}

} // End of namespace NWN

} // End of namespace Engines
