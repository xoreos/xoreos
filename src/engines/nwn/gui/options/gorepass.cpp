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
 *  The NWN violence/gore password options menu.
 */

#include "src/engines/nwn/gui/widgets/panel.h"

#include "src/engines/nwn/gui/options/gorepass.h"

namespace Engines {

namespace NWN {

OptionsGorePasswordMenu::OptionsGorePasswordMenu(bool isMain, ::Engines::Console *console) : GUI(console) {
	load("options_gorepass");

	if (isMain) {
		WidgetPanel *backdrop = new WidgetPanel(*this, "PNL_MAINMENU", "pnl_mainmenu");
		backdrop->setPosition(0.0f, 0.0f, 100.0f);
		addWidget(backdrop);
	}

	// TODO: Password
	getWidget("OldPassword"    , true)->setDisabled(true);
	getWidget("NewPassword"    , true)->setDisabled(true);
	getWidget("ConfirmPassword", true)->setDisabled(true);
}

OptionsGorePasswordMenu::~OptionsGorePasswordMenu() {
}

void OptionsGorePasswordMenu::callbackActive(Widget &widget) {
	if ((widget.getTag() == "CancelButton") ||
	    (widget.getTag() == "CTL_BTN_X")) {
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "OkButton") {
		_returnCode = 2;
		return;
	}
}

} // End of namespace NWN

} // End of namespace Engines
