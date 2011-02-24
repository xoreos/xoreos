/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/new.cpp
 *  The new game menu.
 */

#include "graphics/graphics.h"

#include "engines/nwn/menu/new.h"
#include "engines/nwn/menu/newmodule.h"

#include "engines/aurora/util.h"

namespace Engines {

namespace NWN {

NewMenu::NewMenu(bool xp1, bool xp2, bool xp3) : _hasXP(xp1 || xp2) {
	load("pre_newgame");

	// TODO: Game chapters
	getWidget("PreludeButton" , true)->setDisabled(true);
	getWidget("Chapter1Button", true)->setDisabled(true);
	getWidget("Chapter2Button", true)->setDisabled(true);
	getWidget("Chapter3Button", true)->setDisabled(true);
	getWidget("Chapter4Button", true)->setDisabled(true);

	if (_hasXP)
		// If we have an expansion, hide the "Other modules" button, it's
		// already in the campaign menu
		getWidget("OtherButton", true)->setInvisible(true);

	_module = 0;
	if (!_hasXP)
		_module = new NewModuleMenu;
}

NewMenu::~NewMenu() {
	delete _module;
}

void NewMenu::callbackActive(Widget &widget) {
	if (widget.getTag() == "CancelButton") {
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "OtherButton") {
		sub(*_module);
		return;
	}
}

} // End of namespace NWN

} // End of namespace Engines
