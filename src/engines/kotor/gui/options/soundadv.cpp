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
 *  The advanced sound menu.
 */

#include "src/engines/aurora/widget.h"

#include "src/engines/kotor/gui/options/soundadv.h"

#include "src/engines/kotor/gui/widgets/checkbox.h"

namespace Engines {

namespace KotOR {

OptionsSoundAdvancedMenu::OptionsSoundAdvancedMenu(::Engines::Console *console) : GUI(console) {
	load("optsoundadv");

	//Hardcoded, the gui file returns incorrect values
	getCheckBox("CB_FORCESOFTWARE", true)->setColor(0.0f, 0.658824f, 0.980392f, 1.0f);
}

OptionsSoundAdvancedMenu::~OptionsSoundAdvancedMenu() {

}

void OptionsSoundAdvancedMenu::callbackActive(Widget &widget) {

	if (widget.getTag() == "BTN_DEFAULT") {

	}

	if (widget.getTag() == "BTN_CANCEL") {
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "BTN_BACK") {
		_returnCode = 1;
		return;
	}
}

} // End of namespace KotOR

} // End of namespace Engines

