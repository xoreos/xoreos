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
 *  Creature within an area in Star Wars: Knights of the Old Republic.
 */

#include "src/common/strutil.h"

#include "src/graphics/aurora/model.h"

#include "src/engines/aurora/model.h"

#include "src/engines/kotor/creature.h"

#include "src/engines/kotor/gui/chargen/chargeninfo.h"

namespace Engines {

namespace KotOR {

Creature::Creature() : KotORBase::Creature() {
}

Creature::Creature(const Aurora::GFF3Struct &creature) : KotORBase::Creature(creature) {
}

Creature::Creature(const Common::UString &resRef) : KotORBase::Creature(resRef) {
}

Common::UString Creature::getBodyMeshString(KotORBase::Gender gender, KotORBase::Class charClass, char state) {
	Common::UString body, head;

	body = "p";

	switch (gender) {
		case KotORBase::kGenderMale:
			body += "m";
			break;
		case KotORBase::kGenderFemale:
			body += "f";
			break;
		default:
			throw Common::Exception("Unknown gender");
	}

	body += "b";
	body += state;

	switch (charClass) {
		case KotORBase::kClassSoldier:
			body += "l";
			break;
		case KotORBase::kClassScout:
			body += "m";
			break;
		default:
		case KotORBase::kClassScoundrel:
			body += "s";
			break;
	}

	return body;
}

Common::UString Creature::getBodyTextureString(KotORBase::Gender gender, KotORBase::Skin skin, KotORBase::Class charClass, char state) {
	Common::UString bodyTexture("p");

	switch (gender) {
		case KotORBase::kGenderMale:
			bodyTexture += "m";
			break;
		case KotORBase::kGenderFemale:
			bodyTexture += "f";
			break;
		default:
			throw Common::Exception("Unknown gender");
	}

	bodyTexture  += Common::UString("b") + state;

	switch (state) {
		case 'a':
		case 'b':
			switch (charClass) {
				case KotORBase::kClassSoldier:
					bodyTexture += "l";
					break;
				case KotORBase::kClassScout:
					bodyTexture += "m";
					break;
				default:
				case KotORBase::kClassScoundrel:
					bodyTexture += "s";
					break;
			}
			if (state == 'a') {
				switch (skin) {
					case KotORBase::kSkinA:
						bodyTexture += "A";
						break;
					case KotORBase::kSkinB:
						bodyTexture += "B";
						break;
					default:
						break;
				}
			}
			break;
		default:
			break;
	}

	return bodyTexture;
}

Common::UString Creature::getHeadMeshString(KotORBase::Gender gender, KotORBase::Skin skin, uint32_t faceId) {
	Common::UString head;

	head = "p";

	switch (gender) {
		case KotORBase::kGenderMale:
			head += "m";
			break;
		case KotORBase::kGenderFemale:
			head += "f";
			break;
		default:
			throw Common::Exception("Unknown gender");
	}

	head += "h";

	switch (skin) {
		case KotORBase::kSkinA:
			head += "a";
			break;
		case KotORBase::kSkinB:
			head += "b";
			break;
		default:
		case KotORBase::kSkinC:
			head += "c";
			break;
	}

	head += "0";
	head += Common::composeString(faceId + 1);

	return head;
}

Graphics::Aurora::Model *Creature::createModel(const CharacterGenerationInfo *info) {
	Common::UString bodyString = getBodyMeshString(info->getGender(), info->getClass());
	std::unique_ptr<Graphics::Aurora::Model> body(loadModelObject(bodyString, ""));
	if (!body)
		return 0;

	std::unique_ptr<Graphics::Aurora::Model> head(createHeadModel(info));
	if (head)
		body->attachModel("headhook", head.release());

	return body.release();
}

Graphics::Aurora::Model *Creature::createHeadModel(const CharacterGenerationInfo *info) {
	Common::UString headString = getHeadMeshString(info->getGender(), info->getSkin(), info->getFace());
	std::unique_ptr<Graphics::Aurora::Model> head(loadModelObject(headString, ""));
	if (!head)
		return 0;

	return head.release();
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

} // End of namespace KotOR

} // End of namespace Engines
