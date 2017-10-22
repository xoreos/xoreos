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
 *  The KotOR 2 character generation info.
 */

#include "src/common/strutil.h"
#include "src/common/error.h"

#include "chargeninfo.h"

namespace Engines {

namespace KotOR2 {

CharacterGenerationInfo *CharacterGenerationInfo::createRandomMaleConsular() {
	CharacterGenerationInfo *info = new CharacterGenerationInfo();
	info->_gender = kGenderMale;
	info->_class = kClassJediConsular;
	info->_skin = Skin(std::rand() % kSkinMAX);
	switch (info->_skin) {
		case kSkinH:
			info->_face = std::rand() % 2;
			break;
		default:
			info->_face = std::rand() % 5;
			break;
	}
	return info;
}

CharacterGenerationInfo *CharacterGenerationInfo::createRandomFemaleConsular() {
	CharacterGenerationInfo *info = new CharacterGenerationInfo();
	info->_gender = kGenderFemale;
	info->_class = kClassJediConsular;
	info->_skin = Skin(std::rand() % kSkinMAX);
	switch (info->_skin) {
		case kSkinH:
			info->_face = std::rand() % 2;
			break;
		default:
			info->_face = std::rand() % 5;
			break;
	}
	return info;
}

CharacterGenerationInfo *CharacterGenerationInfo::createRandomMaleGuardian() {
	CharacterGenerationInfo *info = new CharacterGenerationInfo();
	info->_gender = kGenderMale;
	info->_class = kClassJediGuardian;
	info->_skin = Skin(std::rand() % kSkinMAX);
	switch (info->_skin) {
		case kSkinH:
			info->_face = std::rand() % 2;
			break;
		default:
			info->_face = std::rand() % 5;
			break;
	}
	return info;
}

CharacterGenerationInfo *CharacterGenerationInfo::createRandomFemaleGuardian() {
	CharacterGenerationInfo *info = new CharacterGenerationInfo();
	info->_gender = kGenderFemale;
	info->_class = kClassJediGuardian;
	info->_skin = Skin(std::rand() % kSkinMAX);
	switch (info->_skin) {
		case kSkinH:
			info->_face = std::rand() % 2;
			break;
		default:
			info->_face = std::rand() % 5;
			break;
	}
	return info;
}

CharacterGenerationInfo *CharacterGenerationInfo::createRandomMaleSentinel() {
	CharacterGenerationInfo *info = new CharacterGenerationInfo();
	info->_gender = kGenderMale;
	info->_class = kClassJediSentinel;
	info->_skin = Skin(std::rand() % kSkinMAX);
	switch (info->_skin) {
		case kSkinH:
			info->_face = std::rand() % 2;
			break;
		default:
			info->_face = std::rand() % 5;
			break;
	}
	return info;
}

CharacterGenerationInfo *CharacterGenerationInfo::createRandomFemaleSentinel() {
	CharacterGenerationInfo *info = new CharacterGenerationInfo();
	info->_gender = kGenderFemale;
	info->_class = kClassJediSentinel;
	info->_skin = Skin(std::rand() % kSkinMAX);
	switch (info->_skin) {
		case kSkinH:
			info->_face = std::rand() % 2;
			break;
		default:
			info->_face = std::rand() % 5;
			break;
	}
	return info;
}

CharacterGenerationInfo::CharacterGenerationInfo() {
}

Gender CharacterGenerationInfo::getGender() const {
	return _gender;
}

Class CharacterGenerationInfo::getClass() const {
	return _class;
}

} // End of namespace KotOR2

} // End of namespace Engines
