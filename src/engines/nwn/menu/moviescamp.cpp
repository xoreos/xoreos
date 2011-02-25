/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/moviescamp.cpp
 *  The campaign movies menu.
 */

#include "common/configman.h"

#include "engines/nwn/menu/moviescamp.h"
#include "engines/nwn/menu/moviesbase.h"

#include "engines/aurora/util.h"

namespace Engines {

namespace NWN {

MoviesCampMenu::MoviesCampMenu() {
	load("pre_camp_movies");

	Widget *button = 0;

	button = getWidget("NWNXP1Button");
	if (button)
		button->setDisabled(!ConfigMan.getBool("NWN_hasXP1"));
	button = getWidget("NWNXP2Button");
	if (button)
		button->setDisabled(!ConfigMan.getBool("NWN_hasXP2"));
	button = getWidget("NWNXP3Button");
	if (button)
		button->setDisabled(true);

	_base = new MoviesBaseMenu();
}

MoviesCampMenu::~MoviesCampMenu() {
	delete _base;
}

void MoviesCampMenu::callbackActive(Widget &widget) {
	if (widget.getTag() == "CancelButton") {
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "NWNButton") {
		sub(*_base);
		return;
	}

	if        (widget.getTag() == "NWNXP1Button") {
		// No GUI file? Harcoded?

		// Just play them one after another for now...
		playVideo("xp1_intro");
		playVideo("xp1_chap1_chap2");
		playVideo("xp1_chap2_chap3");
		playVideo("xp1_closing");
	} else if (widget.getTag() == "NWNXP2Button") {
		// No GUI file? Harcoded?

		// Just play the intro for now...
		// (Which is the only actual video anyway, the rest is in-game cinematics)
		playVideo("xp2_intro");
	}

	updateMouse();
}

} // End of namespace NWN

} // End of namespace Engines
