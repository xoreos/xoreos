/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/newcamp.cpp
 *  The new campaign menu.
 */

#include "engines/nwn/menu/newcamp.h"
#include "engines/nwn/menu/new.h"
#include "engines/nwn/menu/newmodule.h"

#include "engines/aurora/util.h"

namespace Engines {

namespace NWN {

NewCampMenu::NewCampMenu(bool xp1, bool xp2, bool xp3) {
	load("pre_campaign");

	Widget *button = 0;

	// No GUI files? Harcoded?
	button = getWidget("NWNXP1Button");
	if (button)
		button->setDisabled(/*!xp1*/ true);
	button = getWidget("NWNXP2Button");
	if (button)
		button->setDisabled(/*!xp2*/ true);
	button = getWidget("NWNXP3Button");
	if (button)
		button->setDisabled(true);

	_base   = new NewMenu(xp1, xp2, xp3);
	_module = new NewModuleMenu;
}

NewCampMenu::~NewCampMenu() {
	delete _module;
	delete _base;
}

void NewCampMenu::callbackActive(Widget &widget) {
	if (widget.getTag() == "CancelButton") {
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "NWNButton") {
		sub(*_base);
		return;
	}

	if (widget.getTag() == "OtherButton") {
		sub(*_module);
		return;
	}

}

} // End of namespace NWN

} // End of namespace Engines
