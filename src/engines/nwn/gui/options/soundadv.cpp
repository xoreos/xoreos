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
 *  The NWN advanced sound options menu.
 */

#include "src/engines/nwn/gui/widgets/panel.h"

#include "src/engines/nwn/gui/options/soundadv.h"

namespace Engines {

namespace NWN {

OptionsSoundAdvancedMenu::OptionsSoundAdvancedMenu(bool isMain, ::Engines::Console *console) : GUI(console) {
	load("options_advsound");

	if (isMain) {
		WidgetPanel *backdrop = new WidgetPanel(*this, "PNL_MAINMENU", "pnl_mainmenu");
		backdrop->setPosition(0.0f, 0.0f, 100.0f);
		addWidget(backdrop);
	}

	// TODO: Sound providers
	getWidget("ProviderList", true)->setDisabled(true);

	// TODO: 2D/3D bias
	getWidget("2D3DBiasSlider", true)->setDisabled(true);

	// TODO: EAX level
	getWidget("EAXSlider", true)->setDisabled(true);

	// TODO: Test the sound settings
	getWidget("TestButton", true)->setDisabled(true);
}

OptionsSoundAdvancedMenu::~OptionsSoundAdvancedMenu() {
}

void OptionsSoundAdvancedMenu::fixWidgetType(const Common::UString &tag, WidgetType &type) {
	if (tag == "ProviderList")
		type = kWidgetTypeListBox;
}

void OptionsSoundAdvancedMenu::callbackActive(Widget &widget) {
	if ((widget.getTag() == "CancelButton") ||
	    (widget.getTag() == "XButton")) {
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
