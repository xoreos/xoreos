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
 *  The mouse settings menu.
 */

#include "src/common/configman.h"

#include "src/engines/aurora/widget.h"

#include "src/engines/kotor/gui/options/mousesettings.h"
#include "src/engines/kotor/gui/widgets/checkbox.h"

namespace Engines {

namespace KotOR {

OptionsMouseSettingsMenu::OptionsMouseSettingsMenu(::Engines::Console *console) : GUI(console) {
	load("optmouse");

	getCheckBox("CB_REVBUTTONS", true)->setColor(0.0f, 0.658824f, 0.980392f, 1.0f);
}

OptionsMouseSettingsMenu::~OptionsMouseSettingsMenu() {

}

void OptionsMouseSettingsMenu::show() {
	GUI::show();

	_reverseMouseButtons = ConfigMan.getBool("reversemousebuttons", false);
	setCheckBoxState("CB_REVBUTTONS", _reverseMouseButtons);
}

void OptionsMouseSettingsMenu::callbackActive(Widget &widget) {

	if (widget.getTag() == "BTN_DEFAULT") {
		_reverseMouseButtons = false;
		setCheckBoxState("CB_REVBUTTONS", _reverseMouseButtons);
	}

	if (widget.getTag() == "BTN_BACK") {
		adoptChanges();
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "CB_REVBUTTONS") {
		_reverseMouseButtons = getCheckBoxState("CB_REVBUTTONS");
		return;
	}
}

void OptionsMouseSettingsMenu::adoptChanges() {
	ConfigMan.setBool("reversemousebuttons", _reverseMouseButtons, true);
}

} // End of namespace KotOR

} // End of namespace Engines

