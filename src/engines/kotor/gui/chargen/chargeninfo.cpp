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
 *  The class for storing character information for generation.
 */

#include "src/common/strutil.h"
#include "src/common/error.h"
#include "chargeninfo.h"

namespace Engines {

namespace KotOR {

CharacterGenerationInfo *CharacterGenerationInfo::createRandomMaleSoldier() {
	CharacterGenerationInfo *info = new CharacterGenerationInfo();
	info->_gender = kGenderMale;
	info->_class = kClassSoldier;
	info->_face = std::rand() % 5;
	info->_skin = Skin(std::rand() % kSkinMAX);
	info->_name = "";

	return info;
}

CharacterGenerationInfo *CharacterGenerationInfo::createRandomMaleScout() {
	CharacterGenerationInfo *info = new CharacterGenerationInfo();
	info->_gender = kGenderMale;
	info->_class = kClassScout;
	info->_face = std::rand() % 5;
	info->_skin = Skin(std::rand() % kSkinMAX);
	info->_name = "";

	return info;
}

CharacterGenerationInfo *CharacterGenerationInfo::createRandomMaleScoundrel() {
	CharacterGenerationInfo *info = new CharacterGenerationInfo();
	info->_gender = kGenderMale;
	info->_class = kClassScoundrel;
	info->_face = std::rand() % 5;
	info->_skin = Skin(std::rand() % kSkinMAX);
	info->_name = "";

	return info;
}

CharacterGenerationInfo *CharacterGenerationInfo::createRandomFemaleSoldier() {
	CharacterGenerationInfo *info = new CharacterGenerationInfo();
	info->_gender = kGenderFemale;
	info->_class = kClassSoldier;
	info->_face = std::rand() % 5;
	info->_skin = Skin(std::rand() % kSkinMAX);
	info->_name = "";

	return info;
}

CharacterGenerationInfo *CharacterGenerationInfo::createRandomFemaleScout() {
	CharacterGenerationInfo *info = new CharacterGenerationInfo();
	info->_gender = kGenderFemale;
	info->_class = kClassScout;
	info->_face = std::rand() % 5;
	info->_skin = Skin(std::rand() % kSkinMAX);
	info->_name = "";

	return info;
}

CharacterGenerationInfo *CharacterGenerationInfo::createRandomFemaleScoundrel() {
	CharacterGenerationInfo *info = new CharacterGenerationInfo();
	info->_gender = kGenderFemale;
	info->_class = kClassScoundrel;
	info->_face = std::rand() % 5;
	info->_skin = Skin(std::rand() % kSkinMAX);
	info->_name = "";

	return info;
}

Common::UString CharacterGenerationInfo::getName() {
	return _name;
}

Common::UString CharacterGenerationInfo::getPortrait() {
	Common::UString portrait;
	portrait = "po_p";

	switch (_gender) {
		case kGenderMale:
			portrait += "mh";
			break;
		case kGenderFemale:
			portrait += "fh";
			break;
		default:
			throw Common::Exception("Gender unknown for creating portrait string");
	}

	switch (_skin) {
		case kSkinA:
			portrait += "a";
			break;
		case kSkinB:
			portrait += "b";
			break;
		case kSkinC:
			portrait += "c";
			break;
		default:
			throw Common::Exception("Skin unknown for creating portrait string");
	}

	portrait += Common::composeString(_face + 1);

	return portrait;
}

Skin CharacterGenerationInfo::getSkin() {
	return _skin;
}

uint8_t CharacterGenerationInfo::getFace() {
	return _face;
}

Class CharacterGenerationInfo::getClass() {
	return _class;
}

Gender CharacterGenerationInfo::getGender() {
	return _gender;
}

void CharacterGenerationInfo::setName(Common::UString name) {
	_name = name;
}

void CharacterGenerationInfo::setSkin(Skin skin) {
	_skin = skin;
}

void CharacterGenerationInfo::setFace(uint8 face) {
	_face = face;
}

Creature *CharacterGenerationInfo::getCharacter() {
	Creature* creature = new Creature();
	creature->createPC(this);
	return creature;
}

CharacterGenerationInfo::CharacterGenerationInfo() {

}

} // End of namespace KotOR

} // End of namespace Engines
