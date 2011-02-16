/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/basemovies.cpp
 *  The base game movies menu.
 */

#include "engines/nwn/menu/basemovies.h"

#include "events/events.h"

#include "engines/aurora/util.h"

namespace Engines {

namespace NWN {

static const uint kButtonPrelude = 1;
static const uint kButtonChpt1   = 2;
static const uint kButtonChpt2   = 3;
static const uint kButtonChpt3   = 4;
static const uint kButtonChpt4   = 5;
static const uint kButtonEnd     = 6;
static const uint kButtonCredits = 7;
static const uint kButtonClose   = 8;
static const uint kButtonMAX     = 9;

static const char *kButtonTags[] = {
	"NONE"       , "PreludeButton", "Chpt1Button"  , "Chpt2Button", "Chpt3Button",
	"Chpt4Button", "EndButton"    , "CreditsButton", "CloseButton",
};

BaseMoviesMenu::BaseMoviesMenu() : Menu("pre_movies") {

	for (uint i = 1; i < kButtonMAX; i++)
		addButton(kButtonTags[i], i);
}

BaseMoviesMenu::~BaseMoviesMenu() {
}

void BaseMoviesMenu::show() {
	Menu::show();

	_close = false;
}

void BaseMoviesMenu::mouseUp(Events::Event &event) {
	Menu::mouseUp(event);

	if      (_currentButton == kButtonPrelude)
		playVideo("prelude");
	else if (_currentButton == kButtonChpt1  )
		playVideo("prelude_chap1");
	else if (_currentButton == kButtonChpt2  )
		playVideo("chap1_chap2");
	else if (_currentButton == kButtonChpt3  )
		playVideo("chap2_chap3");
	else if (_currentButton == kButtonChpt4  )
		playVideo("chap3_chap4");
	else if (_currentButton == kButtonEnd    )
		playVideo("ending");
	else if (_currentButton == kButtonCredits)
		playVideo("credits");
	else if (_currentButton == kButtonClose  )
		_close = true;

	updateMouse();
}

bool BaseMoviesMenu::handleCallBack() {
	return _close;
}

} // End of namespace NWN

} // End of namespace Engines
