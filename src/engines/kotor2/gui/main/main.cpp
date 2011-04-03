/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/kotor2/gui/main/main.cpp
 *  The KotOR 2 main menu.
 */

#include "common/util.h"

#include "engines/kotor/gui/widgets/kotorwidget.h"

#include "engines/kotor2/gui/main/main.h"

namespace Engines {

namespace KotOR2 {

MainMenu::MainMenu() {
	load("mainmenu16x12_p");
}

MainMenu::~MainMenu() {
}

void MainMenu::initWidget(Widget &widget) {
	// ...BioWare...
	if (widget.getTag() == "LBL_GAMELOGO") {
		dynamic_cast< ::Engines::KotOR::KotORWidget & >(widget).setFill("kotor2logo");
		return;
	}

	// Warp button? O_o
	if (widget.getTag() == "BTN_WARP") {
		widget.setInvisible(true);
		return;
	}

	// New downloadable content is available, bluhbluh.
	if (widget.getTag() == "LBL_NEWCONTENT") {
		widget.setInvisible(true);
		return;
	}
}

void MainMenu::callbackActive(Widget &widget) {
}

} // End of namespace KotOR2

} // End of namespace Engines
