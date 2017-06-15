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
 *  The menu for modifying the portrait of the.
 */

#include "src/engines/kotor/gui/widgets/kotorwidget.h"
#include "src/engines/kotor/gui/widgets/button.h"
#include "src/engines/kotor/gui/widgets/label.h"

#include "src/engines/kotor/gui/chargen/chargenportrait.h"
#include "src/engines/kotor/gui/chargen/chargeninfo.h"

namespace Engines {

namespace KotOR {

CharacterGenerationPortraitMenu::CharacterGenerationPortraitMenu(CharacterGenerationInfo &info,
		Console *console) : CharacterGenerationBaseMenu(info, console) {

	load("portcust");

	addBackground(kBackgroundTypeMenu);

	getLabel("LBL_PORTRAIT")->setFill(_info.getPortrait());
}

void CharacterGenerationPortraitMenu::callbackActive(Widget &widget) {
	if (widget.getTag() == "BTN_BACK") {
		_returnCode = 1;
		return;
	}
	if (widget.getTag() == "BTN_ACCEPT") {
		accept();
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "BTN_ARRL") {
		// Get the current skin and face
		Skin skin = _info.getSkin();
		uint8_t face = _info.getFace();

		// Move them to left and go in another skin type if needed
		if (face == 0) {
			if (skin == kSkinA)
				skin = kSkinC;
			else
				skin = Skin(skin - 1);
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
		Skin skin = _info.getSkin();
		uint8_t face = _info.getFace();

		// Move them to right and go in another skin type if needed
		if (face == 4) {
			if (skin == kSkinC)
				skin = kSkinA;
			else
				skin = Skin(skin + 1);
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
