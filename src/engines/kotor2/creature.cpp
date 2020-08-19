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
 *  Creature within an area in KotOR II.
 */

#include "src/common/error.h"
#include "src/common/strutil.h"

#include "src/engines/kotor2/creature.h"

namespace Engines {

namespace KotOR2 {

Creature::Creature() : KotORBase::Creature() {
}

Creature::Creature(const Aurora::GFF3Struct &creature) : KotORBase::Creature(creature) {
}

Creature::Creature(const Common::UString &resRef) : KotORBase::Creature(resRef) {
}

Common::UString Creature::getBodyMeshString(KotORBase::Gender gender, KotORBase::Class UNUSED(charClass), char UNUSED(state)) {
	Common::UString body("p");

	switch (gender) {
		case KotORBase::kGenderMale:
			body += "m";
			break;
		case KotORBase::kGenderFemale:
			body += "f";
			break;
		default:
			throw Common::Exception("Invalid gender");
	}

	body += "bam";

	return body;
}

Common::UString Creature::getBodyTextureString(KotORBase::Gender gender, KotORBase::Skin skin, KotORBase::Class UNUSED(charClass), char UNUSED(state)) {
	Common::UString body("p");

	switch (gender) {
		case KotORBase::kGenderMale:
			body += "m";
			break;
		case KotORBase::kGenderFemale:
			body += "f";
			break;
		default:
			throw Common::Exception("Invalid gender");
	}

	body += "bam";

	switch (skin) {
		case KotORBase::kSkinA:
		case KotORBase::kSkinH:
			body += "a";
			break;
		case KotORBase::kSkinB:
			body += "b";
			break;
		case KotORBase::kSkinC:
			body += "c";
			break;
		default:
			throw Common::Exception("Invalid skin");
	}

	return body;
}

Common::UString Creature::getHeadMeshString(KotORBase::Gender gender, KotORBase::Skin skin, uint32_t faceId) {
	Common::UString head("p");

	switch (gender) {
		case KotORBase::kGenderMale:
			head += "m";
			break;
		case KotORBase::kGenderFemale:
			head += "f";
			break;
		default:
			throw Common::Exception("Invalid gender");
	}

	head += "h";

	switch (skin) {
		case KotORBase::kSkinA:
			head += "a";
			break;
		case KotORBase::kSkinB:
			head += "b";
			break;
		case KotORBase::kSkinC:
			head += "c";
			break;
		case KotORBase::kSkinH:
			head += "h";
			break;
		default:
			throw Common::Exception("Invalid skin");
	}

	uint32_t transformedFaceId = transformFaceId(gender, skin, faceId);

	if (transformedFaceId >= 10)
		head += Common::composeString(transformedFaceId);
	else
		head += "0" + Common::composeString(transformedFaceId);

	return head;
}

void Creature::getPartModelsPC(PartModels &parts, uint32_t state, uint8_t textureVariation) {
	KotORBase::Class charClass = getClassByPosition(0);
	parts.body = getBodyMeshString(_gender, charClass, state);
	parts.bodyTexture = getBodyTextureString(_gender, _skin, charClass, state);
	parts.head = getHeadMeshString(_gender, _skin, _face);

	parts.portrait = "po_" + parts.head;
	parts.portrait.replaceAll("0", "");
	parts.bodyTexture += Common::UString::format("%02u", textureVariation);

	loadMovementRate("PLAYER");
}

uint32_t Creature::transformFaceId(KotORBase::Gender gender, KotORBase::Skin skin, uint32_t faceId) {
	switch (skin) {
		case KotORBase::kSkinA:
			switch (faceId) {
				case 0: return 1;
				case 1: return 3;
				case 2:
					if (gender == KotORBase::kGenderFemale) return 4;
					else if (gender == KotORBase::kGenderMale) return 5;
					else throw Common::Exception("invalid gender");
				case 3:
					if (gender == KotORBase::kGenderFemale) return 5;
					else if (gender == KotORBase::kGenderMale) return 6;
					else throw Common::Exception("invalid gender");
				case 4:
					if (gender == KotORBase::kGenderFemale) return 6;
					else if (gender == KotORBase::kGenderMale) return 7;
					else throw Common::Exception("invalid gender");
				default:
					throw Common::Exception("invalid face id");
			}
		case KotORBase::kSkinB:
			if (gender == KotORBase::kGenderFemale) return faceId + 1;
			else return faceId + 6;
		case KotORBase::kSkinC:
			switch (faceId) {
				case 0: return 1;
				case 1:
					if (gender == KotORBase::kGenderFemale) return 2;
					else if (gender == KotORBase::kGenderMale) return 3;
					else throw Common::Exception("invalid gender");
				case 2:
					if (gender == KotORBase::kGenderFemale) return 5;
					else if (gender == KotORBase::kGenderMale) return 4;
					else throw Common::Exception("invalid gender");
				case 3: return 6;
				case 4: return 7;
				default:
					throw Common::Exception("invalid face id");
			}
		case KotORBase::kSkinH: return faceId + 1;
		default:
			throw Common::Exception("invalid skin id");
	}
}

} // End of namespace KotOR2

} // End of namespace Engines
