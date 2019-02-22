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
 *  The KotOR 2 character generation portrait menu.
 */

#include "src/engines/odyssey/label.h"

#include "src/engines/kotor2/gui/chargen/chargenportrait.h"

namespace Engines {

namespace KotOR2 {

CharacterGenerationPortraitMenu::CharacterGenerationPortraitMenu(KotORBase::CharacterGenerationInfo &info,
                                                                 Engines::Console *console) :
		CharacterGenerationBaseMenu(info, console) {

	load("portcust_p");

	getLabel("LBL_PORTRAIT")->setFill(_info.getPortrait());
}

void CharacterGenerationPortraitMenu::callbackActive(Widget &widget) {
	if (widget.getTag() == "BTN_ACCEPT") {
		accept();
		_returnCode = kReturnCodeAbort;
		return;
	}
	if (widget.getTag() == "BTN_CANCEL") {
		_returnCode = kReturnCodeAbort;
		return;
	}

	if (widget.getTag() == "BTN_ARRL") {
		// Get the current skin and face
		KotORBase::Skin skin = _info.getSkin();
		unsigned int face = _info.getFace();

		// Move them to left and go in another skin type if needed
		if (face == 0) {
			if (skin == KotORBase::kSkinA)
				skin = KotORBase::kSkinH;
			else
				skin = KotORBase::Skin(skin - 1);

			if (skin == KotORBase::kSkinH)
				face = 1;
			else
				face = 4;
		} else {
			face -= 1;
		}

		// Set the new skin and face values
		_info.setSkin(skin);
		_info.setFace(face);

		// And then reset the portrait
		getLabel("LBL_PORTRAIT")->setFill(_info.getPortrait());

		return;
	}

	if (widget.getTag() == "BTN_ARRR") {
		// Get the current skin and face
		KotORBase::Skin skin = _info.getSkin();
		unsigned int face = _info.getFace();

		// Move them to right and go in another skin type if needed
		if (face == ((skin == KotORBase::kSkinH) ? 1 : 4)) {
			if (skin == KotORBase::kSkinH)
				skin = KotORBase::kSkinA;
			else
				skin = KotORBase::Skin(skin + 1);
			face = 0;
		} else {
			face += 1;
		}

		// Set the new skin and face values
		_info.setSkin(skin);
		_info.setFace(face);

		// And then reset the portrait
		getLabel("LBL_PORTRAIT")->setFill(_info.getPortrait());

		return;
	}
}

} // End of namespace KotOR

} // End of namespace Engines
