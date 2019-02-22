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
 *  Character generation information for KotOR II.
 */

#include "src/common/strutil.h"

#include "src/aurora/ltrfile.h"

#include "src/engines/kotor2/creature.h"

#include "src/engines/kotor2/gui/chargen/chargeninfo.h"

namespace Engines {

namespace KotOR2 {

CharacterGenerationInfo *CharacterGenerationInfo::createRandomMaleConsular() {
	CharacterGenerationInfo *info = new CharacterGenerationInfo();
	info->_gender = KotORBase::kGenderMale;
	info->_class = KotORBase::kClassJediConsular;
	info->_skin = KotORBase::Skin(std::rand() % KotORBase::kSkinMAX);
	switch (info->_skin) {
		case KotORBase::kSkinH:
			info->_face = std::rand() % 2;
			break;
		default:
			info->_face = std::rand() % 5;
			break;
	}

	Aurora::LTRFile humanMale("humanm");
	Aurora::LTRFile humanLast("humanl");

	info->_name = humanMale.generateRandomName(8) + " " + humanLast.generateRandomName(8);

	return info;
}

CharacterGenerationInfo *CharacterGenerationInfo::createRandomFemaleConsular() {
	CharacterGenerationInfo *info = new CharacterGenerationInfo();
	info->_gender = KotORBase::kGenderFemale;
	info->_class = KotORBase::kClassJediConsular;
	info->_skin = KotORBase::Skin(std::rand() % KotORBase::kSkinMAX);
	switch (info->_skin) {
		case KotORBase::kSkinH:
			info->_face = std::rand() % 2;
			break;
		default:
			info->_face = std::rand() % 5;
			break;
	}

	Aurora::LTRFile humanFemale("humanf");
	Aurora::LTRFile humanLast("humanl");

	info->_name = humanFemale.generateRandomName(8) + " " + humanLast.generateRandomName(8);

	return info;
}

CharacterGenerationInfo *CharacterGenerationInfo::createRandomMaleGuardian() {
	CharacterGenerationInfo *info = new CharacterGenerationInfo();
	info->_gender = KotORBase::kGenderMale;
	info->_class = KotORBase::kClassJediGuardian;
	info->_skin = KotORBase::Skin(std::rand() % KotORBase::kSkinMAX);
	switch (info->_skin) {
		case KotORBase::kSkinH:
			info->_face = std::rand() % 2;
			break;
		default:
			info->_face = std::rand() % 5;
			break;
	}

	Aurora::LTRFile humanMale("humanm");
	Aurora::LTRFile humanLast("humanl");

	info->_name = humanMale.generateRandomName(8) + " " + humanLast.generateRandomName(8);

	return info;
}

CharacterGenerationInfo *CharacterGenerationInfo::createRandomFemaleGuardian() {
	CharacterGenerationInfo *info = new CharacterGenerationInfo();
	info->_gender = KotORBase::kGenderFemale;
	info->_class = KotORBase::kClassJediGuardian;
	info->_skin = KotORBase::Skin(std::rand() % KotORBase::kSkinMAX);
	switch (info->_skin) {
		case KotORBase::kSkinH:
			info->_face = std::rand() % 2;
			break;
		default:
			info->_face = std::rand() % 5;
			break;
	}

	Aurora::LTRFile humanFemale("humanf");
	Aurora::LTRFile humanLast("humanl");

	info->_name = humanFemale.generateRandomName(8) + " " + humanLast.generateRandomName(8);

	return info;
}

CharacterGenerationInfo *CharacterGenerationInfo::createRandomMaleSentinel() {
	CharacterGenerationInfo *info = new CharacterGenerationInfo();
	info->_gender = KotORBase::kGenderMale;
	info->_class = KotORBase::kClassJediSentinel;
	info->_skin = KotORBase::Skin(std::rand() % KotORBase::kSkinMAX);
	switch (info->_skin) {
		case KotORBase::kSkinH:
			info->_face = std::rand() % 2;
			break;
		default:
			info->_face = std::rand() % 5;
			break;
	}

	Aurora::LTRFile humanMale("humanm");
	Aurora::LTRFile humanLast("humanl");

	info->_name = humanMale.generateRandomName(8) + " " + humanLast.generateRandomName(8);

	return info;
}

CharacterGenerationInfo *CharacterGenerationInfo::createRandomFemaleSentinel() {
	CharacterGenerationInfo *info = new CharacterGenerationInfo();
	info->_gender = KotORBase::kGenderFemale;
	info->_class = KotORBase::kClassJediSentinel;
	info->_skin = KotORBase::Skin(std::rand() % KotORBase::kSkinMAX);
	switch (info->_skin) {
		case KotORBase::kSkinH:
			info->_face = std::rand() % 2;
			break;
		default:
			info->_face = std::rand() % 5;
			break;
	}

	Aurora::LTRFile humanFemale("humanf");
	Aurora::LTRFile humanLast("humanl");

	info->_name = humanFemale.generateRandomName(8) + " " + humanLast.generateRandomName(8);

	return info;
}

Common::UString CharacterGenerationInfo::getPortrait() const {
	Common::UString portrait;
	portrait += "po_p";

	switch (_gender) {
		case KotORBase::kGenderMale:
			portrait += "m";
			break;
		case KotORBase::kGenderFemale:
			portrait += "f";
			break;
		default:
			throw Common::Exception("Gender unknown for creating portrait string");
	}

	portrait += "h";

	switch (_skin) {
		case KotORBase::kSkinA:
			switch (_face) {
				case 0:
					portrait += "a01";
					break;
				case 1:
					portrait += "a03";
					break;
				case 2:
					if (_gender == KotORBase::kGenderFemale)
						portrait += "a04";
					else if (_gender == KotORBase::kGenderMale)
						portrait += "a05";
					break;
				case 3:
					if (_gender == KotORBase::kGenderFemale)
						portrait += "a05";
					else if (_gender == KotORBase::kGenderMale)
						portrait += "a06";
					break;
				case 4:
					if (_gender == KotORBase::kGenderFemale)
						portrait += "a06";
					else if (_gender == KotORBase::kGenderMale)
						portrait += "a07";
					break;
				default:
					throw Common::Exception("invalid face id");
			}
			break;
		case KotORBase::kSkinB:
			portrait += "b";
			if (_gender == KotORBase::kGenderFemale)
				portrait += ("0" + Common::composeString(_face + 1));
			else
				if (_face + 6 >= 10)
					portrait += Common::composeString(_face + 6);
				else
					portrait += ("0" + Common::composeString(_face + 6));
			break;
		case KotORBase::kSkinC:
			switch (_face) {
				case 0:
					portrait += "c01";
					break;
				case 1:
					if (_gender == KotORBase::kGenderFemale)
						portrait += "c02";
					else if (_gender == KotORBase::kGenderMale)
						portrait += "c03";
					break;
				case 2:
					if (_gender == KotORBase::kGenderFemale)
						portrait += "c05";
					else if (_gender == KotORBase::kGenderMale)
						portrait += "c04";
					break;
				case 3:
					portrait += "c06";
					break;
				case 4:
					portrait += "c07";
					break;
				default:
					throw Common::Exception("invalid face id");
			}
			break;
		case KotORBase::kSkinH:
			portrait += ("h0" + Common::composeString(_face + 1));
			break;
		default:
			throw Common::Exception("invalid skin id");
	}

	return portrait;
}

KotORBase::Creature *CharacterGenerationInfo::createCharacter() const {
	Common::ScopedPtr<KotORBase::Creature> creature(new Creature());
	creature->createPC(*this);
	return creature.release();
}

} // End of namespace KotOR2

} // End of namespace Engines
