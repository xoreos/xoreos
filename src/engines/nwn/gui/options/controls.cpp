/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/options/controls.cpp
 *  The NWN controls options menu.
 */

#include "engines/nwn/gui/widgets/panel.h"

#include "engines/nwn/gui/options/controls.h"

namespace Engines {

namespace NWN {

OptionsControlsMenu::OptionsControlsMenu(bool isMain) {
	load("options_controls");

	if (isMain) {
		WidgetPanel *backdrop = new WidgetPanel(*this, "PNL_MAINMENU", "pnl_mainmenu");
		backdrop->setPosition(0.0, 0.0, -10.0);
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
