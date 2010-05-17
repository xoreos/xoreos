/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
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

#include "engines/nwn/menu/main.h"
#include "engines/nwn/menu/gui.h"

#include "events/events.h"

#include "graphics/aurora/model.h"

namespace Engines {

namespace NWN {

MainMenu::MainMenu(bool xp1, bool xp2) : _gui(0), _xp1(0), _xp2(0) {
	_gui = loadGUI("pre_main");

	if (xp1) {
		_xp1 = loadModel("ctl_xp1_text", Graphics::Aurora::kModelTypeGUIFront);
		_xp1->setPosition(1.24, 0.00, 0.50);
	}

	if (xp2) {
		_xp2 = loadModel("ctl_xp2_text", Graphics::Aurora::kModelTypeGUIFront);
		_xp2->setPosition(1.24, -1.47, 0.50);
	}
}

MainMenu::~MainMenu() {
	delete _gui;

	freeModel(_xp1);
	freeModel(_xp2);
}

void MainMenu::show() {
	_gui->show();

	if (_xp1)
		_xp1->show();
	if (_xp2)
		_xp2->show();
}

void MainMenu::handle() {
	while (!EventMan.quitRequested())
		EventMan.delay(10);
}

} // End of namespace NWN

} // End of namespace Engines
