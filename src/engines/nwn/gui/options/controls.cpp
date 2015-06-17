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
 *  The NWN controls options menu.
 */

#include "src/engines/nwn/gui/widgets/panel.h"

#include "src/engines/nwn/gui/options/controls.h"

namespace Engines {

namespace NWN {

OptionsControlsMenu::OptionsControlsMenu(bool isMain, ::Engines::Console *console) : GUI(console) {
	load("options_controls");

	if (isMain) {
		WidgetPanel *backdrop = new WidgetPanel(*this, "PNL_MAINMENU", "pnl_mainmenu");
		backdrop->setPosition(0.0f, 0.0f, 100.0f);
		addWidget(backdrop);
	}

	std::list<Widget *> camGroup;
	camGroup.push_back(getWidget("ChaseCheckBox"));
	camGroup.push_back(getWidget("StiffCheckBox"));
	camGroup.push_back(getWidget("BGCheckBox"));
	declareGroup(camGroup);

	// TODO: Key configuration
	getWidget("KeyButton", true)->setDisabled(true);

	// TODO: Camera mode
	getWidget("ChaseCheckBox", true)->setDisabled(true);
	getWidget("StiffCheckBox", true)->setDisabled(true);
	getWidget("BGCheckBox", true)->setDisabled(true);

	// TODO: Dialog zoom
	getWidget("DialogZoomBox", true)->setDisabled(true);

	// TODO: Screen edge camera turning
	getWidget("ScreenEdgeCam", true)->setDisabled(true);

	// TODO: Mouse wheel sensitivity
	getWidget("ZoomSpeed", true)->setDisabled(true);

	// TODO: Camera turn speed
	getWidget("TurnSpeed", true)->setDisabled(true);
}

OptionsControlsMenu::~OptionsControlsMenu() {
}

void OptionsControlsMenu::callbackActive(Widget &widget) {
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
