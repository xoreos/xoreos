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
 *  The auto pause menu.
 */

#include "engines/aurora/widget.h"

#include "engines/kotor/gui/options/autopause.h"

#include "engines/kotor/gui/widgets/checkbox.h"

namespace Engines {

namespace KotOR {

OptionsAutoPauseMenu::OptionsAutoPauseMenu() {
	load("optautopause");
	//Hardcoded, the gui file returns 1.0, 1.0, 1.0, 1.0
	getCheckBox("CB_ENDROUND", true)->setColor(0, 0.658824, 0.980392, 1);
	getCheckBox("CB_ENEMYSIGHTED", true)->setColor(0, 0.658824, 0.980392, 1);
	getCheckBox("CB_MINESIGHTED", true)->setColor(0, 0.658824, 0.980392, 1);
	getCheckBox("CB_PARTYKILLED", true)->setColor(0, 0.658824, 0.980392, 1);
	getCheckBox("CB_ACTIONMENU", true)->setColor(0, 0.658824, 0.980392, 1);
	getCheckBox("CB_TRIGGERS", true)->setColor(0, 0.658824, 0.980392, 1);
}

OptionsAutoPauseMenu::~OptionsAutoPauseMenu() {

}

void OptionsAutoPauseMenu::callbackActive(Widget &widget) {

	if (widget.getTag() == "BTN_DEFAULT") {

	}

	if (widget.getTag() == "BTN_BACK") {
		_returnCode = 1;
		return;
	}
}


} // End of namespace KotOR

} // End of namespace Engines

