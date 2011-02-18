/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/moviesbase.cpp
 *  The base game movies menu.
 */

#include "engines/nwn/menu/moviesbase.h"

#include "engines/aurora/util.h"

namespace Engines {

namespace NWN {

MoviesBaseMenu::MoviesBaseMenu() : GUI("pre_movies") {
}

MoviesBaseMenu::~MoviesBaseMenu() {
}

void MoviesBaseMenu::callbackActive(Widget &widget) {
	if (widget.getTag() == "CloseButton") {
		_returnCode = 1;
		return;
	}

	if      (widget.getTag() == "PreludeButton")
		playVideo("prelude");
	else if (widget.getTag() == "Chpt1Button")
		playVideo("prelude_chap1");
	else if (widget.getTag() == "Chpt2Button")
		playVideo("chap1_chap2");
	else if (widget.getTag() == "Chpt3Button")
		playVideo("chap2_chap3");
	else if (widget.getTag() == "Chpt4Button")
		playVideo("chap3_chap4");
	else if (widget.getTag() == "EndButton")
		playVideo("ending");
	else if (widget.getTag() == "CreditsButton")
		playVideo("credits");

	updateMouse();
}

} // End of namespace NWN

} // End of namespace Engines
