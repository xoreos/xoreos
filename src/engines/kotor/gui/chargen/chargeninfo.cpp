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
 *  Character generation information for Star Wars: Knights of the Old Republic.
 */

#include "src/common/strutil.h"
#include "src/common/random.h"

#include "src/aurora/ltrfile.h"
#include "src/aurora/2dareg.h"
#include "src/aurora/2dafile.h"

#include "src/engines/kotorbase/types.h"

#include "src/engines/kotor/gui/chargen/chargeninfo.h"

namespace Engines {

namespace KotOR {

CharacterGenerationInfo::CharacterGenerationInfo() : KotORBase::CharacterGenerationInfo() {
}

CharacterGenerationInfo::CharacterGenerationInfo(const KotORBase::CharacterGenerationInfo &info) :
		KotORBase::CharacterGenerationInfo(info) {
}

CharacterGenerationInfo *CharacterGenerationInfo::createRandomMaleSoldier() {
	CharacterGenerationInfo *info = new CharacterGenerationInfo();
	info->_gender = KotORBase::kGenderMale;
	info->_class = KotORBase::kClassSoldier;
	info->_face = RNG.getNext(0, 5);
	info->_skin = KotORBase::Skin(RNG.getNext(0, 3));

	Aurora::LTRFile humanMale("humanm");
	Aurora::LTRFile humanLast("humanl");

	info->_name = humanMale.generateRandomName(8) + " " + humanLast.generateRandomName(8);

	info->loadAbilities();

	return info;
}

CharacterGenerationInfo *CharacterGenerationInfo::createRandomMaleScout() {
	CharacterGenerationInfo *info = new CharacterGenerationInfo();
	info->_gender = KotORBase::kGenderMale;
	info->_class = KotORBase::kClassScout;
	info->_face = RNG.getNext(0, 5);
	info->_skin = KotORBase::Skin(RNG.getNext(0, 3));

	Aurora::LTRFile humanMale("humanm");
	Aurora::LTRFile humanLast("humanl");

	info->_name = humanMale.generateRandomName(8) + " " + humanLast.generateRandomName(8);

	info->loadAbilities();

	return info;
}

CharacterGenerationInfo *CharacterGenerationInfo::createRandomMaleScoundrel() {
	CharacterGenerationInfo *info = new CharacterGenerationInfo();
	info->_gender = KotORBase::kGenderMale;
	info->_class = KotORBase::kClassScoundrel;
	info->_face = RNG.getNext(0, 5);
	info->_skin = KotORBase::Skin(RNG.getNext(0, 3));

	Aurora::LTRFile humanMale("humanm");
	Aurora::LTRFile humanLast("humanl");

	info->_name = humanMale.generateRandomName(8) + " " + humanLast.generateRandomName(8);

	info->loadAbilities();

	return info;
}

CharacterGenerationInfo *CharacterGenerationInfo::createRandomFemaleSoldier() {
	CharacterGenerationInfo *info = new CharacterGenerationInfo();
	info->_gender = KotORBase::kGenderFemale;
	info->_class = KotORBase::kClassSoldier;
	info->_face = RNG.getNext(0, 5);
	info->_skin = KotORBase::Skin(RNG.getNext(0, 3));

	Aurora::LTRFile humanFemale("humanf");
	Aurora::LTRFile humanLast("humanl");

	info->_name = humanFemale.generateRandomName(8) + " " + humanLast.generateRandomName(8);

	info->loadAbilities();

	return info;
}

CharacterGenerationInfo *CharacterGenerationInfo::createRandomFemaleScout() {
	CharacterGenerationInfo *info = new CharacterGenerationInfo();
	info->_gender = KotORBase::kGenderFemale;
	info->_class = KotORBase::kClassScout;
	info->_face = RNG.getNext(0, 5);
	info->_skin = KotORBase::Skin(RNG.getNext(0, 3));

	Aurora::LTRFile humanFemale("humanf");
	Aurora::LTRFile humanLast("humanl");

	info->_name = humanFemale.generateRandomName(8) + " " + humanLast.generateRandomName(8);

	info->loadAbilities();

	return info;
}

CharacterGenerationInfo *CharacterGenerationInfo::createRandomFemaleScoundrel() {
	CharacterGenerationInfo *info = new CharacterGenerationInfo();
	info->_gender = KotORBase::kGenderFemale;
	info->_class = KotORBase::kClassScoundrel;
	info->_face = RNG.getNext(0, 5);
	info->_skin = KotORBase::Skin(RNG.getNext(0, 3));

	Aurora::LTRFile humanFemale("humanf");
	Aurora::LTRFile humanLast("humanl");

	info->_name = humanFemale.generateRandomName(8) + " " + humanLast.generateRandomName(8);

	info->loadAbilities();

	return info;
}

Common::UString CharacterGenerationInfo::getPortrait() const {
	Common::UString portrait = "po_p";

	switch (_gender) {
		case KotORBase::kGenderMale:
			portrait += "mh";
			break;
		case KotORBase::kGenderFemale:
			portrait += "fh";
			break;
		default:
			throw Common::Exception("Gender unknown for creating portrait string");
	}

	switch (_skin) {
		case KotORBase::kSkinA:
			portrait += "a";
			break;
		case KotORBase::kSkinB:
			portrait += "b";
			break;
		case KotORBase::kSkinC:
			portrait += "c";
			break;
		default:
			throw Common::Exception("Skin unknown for creating portrait string");
	}

	portrait += Common::composeString(_face + 1);

	return portrait;
}

void CharacterGenerationInfo::loadAbilities() {
	const Aurora::TwoDAFile &classes = TwoDAReg.get2DA("classes");
	Common::UString label;

	switch (_class) {
		case KotORBase::kClassSoldier:
			label = "Soldier";
			break;
		case KotORBase::kClassScout:
			label = "Scout";
			break;
		case KotORBase::kClassScoundrel:
			label = "Scoundrel";
			break;
		default:
			return;
	}

	const Aurora::TwoDARow &row = classes.getRow("label", label);

	_abilities.strength = row.getInt("str");
	_abilities.dexterity = row.getInt("dex");
	_abilities.constitution = row.getInt("con");
	_abilities.wisdom = row.getInt("wis");
	_abilities.intelligence = row.getInt("int");
	_abilities.charisma = row.getInt("cha");
}

} // End of namespace KotOR

} // End of namespace Engines
