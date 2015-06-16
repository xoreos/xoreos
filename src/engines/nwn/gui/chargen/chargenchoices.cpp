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

	_classId = 0;

	_goodness = 101;
	_loyalty = 101;

	_abilities.assign(6, 8);
	_racialAbilities.assign(6, 0);

	_package = 10000;
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

	const Aurora::TwoDAFile &twodaRace = TwoDAReg.get2DA("racialtypes");
	const Aurora::TwoDAFile &twodaFeatRace = TwoDAReg.get2DA(
		twodaRace.getRow(race).getString("FeatsTable"));

	for (size_t it = 0; it < twodaFeatRace.getRowCount(); ++it) {
		const Aurora::TwoDARow &rowFeatRace = twodaFeatRace.getRow(it);
		_racialFeats.push_back(rowFeatRace.getInt("FeatIndex"));
	}
}

void CharGenChoices::setCharPortrait(const Common::UString &portrait) {
	_creature->setPortrait(portrait);
}

void CharGenChoices::setCharClass(uint32 classId) {
	_classId = classId;

	// Add granted class feats.
	_classFeats.clear();
	const Aurora::TwoDAFile &twodaClasses = TwoDAReg.get2DA("classes");
	const Aurora::TwoDAFile &twodaClsFeat = TwoDAReg.get2DA(twodaClasses.getRow(classId).getString("FeatsTable"));
	for (size_t it = 0; it < twodaClsFeat.getRowCount(); ++it) {
		const Aurora::TwoDARow &rowFeat = twodaClsFeat.getRow(it);
		if (rowFeat.getInt("List") != 3)
			continue;

		if (rowFeat.getInt("GrantedOnLevel") != _creature->getHitDice() + 1)
			continue;

		if (!hasFeat(rowFeat.getInt("FeatIndex")))
			_classFeats.push_back(rowFeat.getInt("FeatIndex"));
	}

	//TODO Init spell slots.
}

void CharGenChoices::setCharAlign(uint32 goodness, uint32 loyalty) {
	_goodness = goodness;
	_loyalty  = loyalty;
}

void CharGenChoices::setCharAbilities(std::vector<uint32> abilities,
                                      std::vector<uint32> racialAbilities) {
	_abilities = abilities;
	_racialAbilities = racialAbilities;
}

void CharGenChoices::setCharPackage(uint32 package) {
	_package = package;
}

bool CharGenChoices::hasFeat(uint32 featId) const {
	for (std::vector<uint32>::const_iterator f = _normalFeats.begin(); f != _normalFeats.end(); ++f)
		if (*f == featId)
			return true;

	for (std::vector<uint32>::const_iterator f = _racialFeats.begin(); f != _racialFeats.end(); ++f)
		if (*f == featId)
			return true;

	for (std::vector<uint32>::const_iterator f = _classFeats.begin(); f != _classFeats.end(); ++f)
		if (*f == featId)
			return true;

	return _creature->hasFeat(featId);
}

uint32 CharGenChoices::getClass() const {
	return _classId;
}

uint32 CharGenChoices::getRace() const {
	return _creature->getRace();
}

bool CharGenChoices::getAlign(uint32 &goodness, uint32 &loyalty) const {
	// Check if alignment has been previously set.
	if (_goodness > 100)
		return false;

	goodness = _goodness;
	loyalty  = _loyalty;
	return true;
}

uint32 CharGenChoices::getAbility(uint32 ability) const {
	return _abilities[ability];
}

uint32 CharGenChoices::getPackage() const {
	return _package;
}

} // End of namespace NWN

} // End of namespace Engines
