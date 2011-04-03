/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/jade/gui/main/main.cpp
 *  The Jade Empire main menu.
 */

#include "common/util.h"

#include "engines/jade/gui/main/main.h"

namespace Engines {

namespace Jade {

MainMenu::MainMenu() {
	load("lti_maingame");
}

MainMenu::~MainMenu() {
}

void MainMenu::callbackActive(Widget &widget) {
}

} // End of namespace Jade

} // End of namespace Engines
