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

#include "events/events.h"

#include "graphics/aurora/model.h"

namespace Engines {

namespace NWN {

MainMenu::MainMenu(bool xp1, bool xp2) : _background(0), _xp1(0), _xp2(0) {
	if (xp1) {
		_xp1 = loadModel("ctl_xp1_text", Graphics::Aurora::kModelTypeGUIFront);
		_xp1->setPosition(6.0, 1.38, 0.5);
	}

	if (xp2) {
		_xp2 = loadModel("ctl_xp2_text", Graphics::Aurora::kModelTypeGUIFront);
		_xp2->setPosition(6.0, -0.10, 0.5);
	}

	// => pre_main.gui
	_background = loadModel("pnl_mainmenu", Graphics::Aurora::kModelTypeGUIFront);
	_background->setPosition(4.760000, 1.370000, 0.000000);
}

MainMenu::~MainMenu() {
	freeModel(_xp1);
	freeModel(_xp2);
	freeModel(_background);
}

void MainMenu::show() {
	_background->show();

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
