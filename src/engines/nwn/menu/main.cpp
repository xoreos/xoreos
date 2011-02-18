/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/main.cpp
 *  The main menu.
 */

#include "events/events.h"

#include "graphics/aurora/model.h"

#include "engines/nwn/menu/main.h"

#include "engines/aurora/model.h"

namespace Engines {

namespace NWN {

MainMenu::MainMenu(bool xp1, bool xp2, bool xp3) : GUI("pre_main"), _xp1(0), _xp2(0) {
	if (xp1) {
		_xp1 = loadModelGUI("ctl_xp1_text");
		_xp1->setPosition(1.24, 0.00, 0.50);
	}

	if (xp2) {
		_xp2 = loadModelGUI("ctl_xp2_text");
		_xp2->setPosition(1.24, -1.47, 0.50);
	}

	getWidget("NewButton"    , true)->setDisabled(true);
	getWidget("LoadButton"   , true)->setDisabled(true);
	getWidget("MultiButton"  , true)->setDisabled(true);
	getWidget("MoviesButton" , true)->setDisabled(true);
	getWidget("OptionsButton", true)->setDisabled(true);
}

MainMenu::~MainMenu() {
	delete _xp2;
	delete _xp1;
}

void MainMenu::show() {
	GUI::show();

	if (_xp1)
		_xp1->show();
	if (_xp2)
		_xp2->show();
}

void MainMenu::hide() {
	GUI::hide();

	if (_xp1)
		_xp1->hide();
	if (_xp2)
		_xp2->hide();
}

void MainMenu::callbackActive(Widget &widget) {
	if (widget.getTag() == "ExitButton") {
		EventMan.requestQuit();
		return;
	}
}

} // End of namespace NWN

} // End of namespace Engines
