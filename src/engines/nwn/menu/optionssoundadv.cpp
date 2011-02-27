/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/optionssoundadv.cpp
 *  The advanced sound options menu.
 */

#include "engines/nwn/menu/optionssoundadv.h"

#include "engines/aurora/util.h"

namespace Engines {

namespace NWN {

OptionsSoundAdvancedMenu::OptionsSoundAdvancedMenu(bool isMain) {
	load("options_advsound");

	if (isMain) {
		WidgetPanel *backdrop = new WidgetPanel("PNL_MAINMENU", "pnl_mainmenu");
		backdrop->setPosition(0.0, 0.0, -10.0);
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
