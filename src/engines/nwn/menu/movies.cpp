/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/movies.cpp
 *  The movies menu.
 */

#include "engines/nwn/menu/movies.h"
#include "engines/nwn/menu/basemovies.h"

#include "events/events.h"

namespace Engines {

namespace NWN {

static const uint kButtonNWN    = 1;
static const uint kButtonNWNXP1 = 2;
static const uint kButtonNWNXP2 = 3;
static const uint kButtonNWNXP3 = 4;
static const uint kButtonCancel = 5;
static const uint kButtonMAX    = 6;

static const char *kButtonTags[] = {
	"NONE"        , "NWNButton"   , "NWNXP1Button",
	"NWNXP2Button", "NWNXP3Button", "CancelButton",
};

MoviesMenu::MoviesMenu(const ModelLoader &modelLoader, bool xp1, bool xp2, bool xp3) :
	Menu(modelLoader, "pre_camp_movies") {

	addButton(kButtonTags[kButtonNWN], kButtonNWN);
	_baseMovies = new BaseMoviesMenu(modelLoader);

	if (xp1) {
		// TODO: No GUI file? Hardcoded?
		addButton(kButtonTags[kButtonNWNXP1], kButtonNWNXP1);
		disableButton(kButtonNWNXP1);
	}

	if (xp2) {
		// TODO: No GUI file? Hardcoded?
		addButton(kButtonTags[kButtonNWNXP2], kButtonNWNXP2);
		disableButton(kButtonNWNXP2);
	}

	if (xp3) {
		// TODO: No GUI file? Hardcoded?
		addButton(kButtonTags[kButtonNWNXP3], kButtonNWNXP3);
		disableButton(kButtonNWNXP3);
	}

	addButton(kButtonTags[kButtonCancel], kButtonCancel);
}

MoviesMenu::~MoviesMenu() {
	delete _baseMovies;
}

void MoviesMenu::show() {
	Menu::show();

	_close = false;
}

void MoviesMenu::mouseUp(Events::Event &event) {
	Menu::mouseUp(event);

	if        (_currentButton == kButtonNWN   ) {
		subMenu(*_baseMovies);
	} else if (_currentButton == kButtonNWNXP1) {
	} else if (_currentButton == kButtonNWNXP2) {
	} else if (_currentButton == kButtonNWNXP3) {
	} else if (_currentButton == kButtonCancel) {
		_close = true;
	}

	updateMouse();
}

bool MoviesMenu::handleCallBack() {
	return _close;
}

} // End of namespace NWN

} // End of namespace Engines
