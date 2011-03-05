/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/ingamemain.cpp
 *  The ingame main menu.
 */

#include "aurora/talkman.h"

#include "engines/nwn/menu/ingamemain.h"
#include "engines/nwn/menu/yesnocancel.h"

namespace Engines {

namespace NWN {

InGameMainMenu::InGameMainMenu() {
	load("options_main");

	_quitPrompt = new YesNoCancelDialog(TalkMan.getString(10308), false);
}

InGameMainMenu::~InGameMainMenu() {
	delete _quitPrompt;
}

void InGameMainMenu::initWidget(Widget &widget) {
	if (widget.getTag() == "BuildNumber") {
		dynamic_cast<WidgetLabel &>(widget).setText(PACKAGE_STRING);
		dynamic_cast<WidgetLabel &>(widget).setColor(0.6, 0.6, 0.6, 1.0);
		return;
	}

	if (widget.getTag() == "SaveCharButton#Caption") {
		dynamic_cast<WidgetLabel &>(widget).setText(TalkMan.getString(6560));
		return;
	}
}

void InGameMainMenu::callbackActive(Widget &widget) {
	if ((widget.getTag() == "ResumeGameButton") ||
	    (widget.getTag() == "XButton")) {
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "ExitButton") {
		if (sub(*_quitPrompt, 0, false) == 1) {
			_returnCode = 2;
			return;
		}

		show();
		return;
	}
}

} // End of namespace NWN

} // End of namespace Engines
