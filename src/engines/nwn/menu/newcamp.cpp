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

NewCampMenu::NewCampMenu() {
	load("pre_campaign");

	Widget *button = 0;

	// No GUI files? Harcoded?
	button = getWidget("NWNXP1Button");
	if (button)
		button->setDisabled(/*!ConfigMan.getBool("NWN_hasXP1")*/ true);
	button = getWidget("NWNXP2Button");
	if (button)
		button->setDisabled(/*!ConfigMan.getBool("NWN_hasXP2")*/ true);
	button = getWidget("NWNXP3Button");
	if (button)
		button->setDisabled(true);

	_base   = new NewMenu;
	_module = new NewModuleMenu;
}

NewCampMenu::~NewCampMenu() {
	delete _module;
	delete _base;
}

void NewCampMenu::callbackRun() {
	int startCode = _startCode;
	_startCode = 0;

	if (startCode == 2)
		if (sub(*_base, startCode) == 2)
			_returnCode = 2;

	if (startCode == 3)
		if (sub(*_module, startCode) == 3)
			_returnCode = 3;
}

void NewCampMenu::callbackActive(Widget &widget) {
	if (widget.getTag() == "CancelButton") {
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "NWNButton") {
		if (sub(*_base) == 2)
			_returnCode = 2;
		return;
	}

	if (widget.getTag() == "OtherButton") {
		if (sub(*_module) == 3)
			_returnCode = 3;
		return;
	}

}

} // End of namespace NWN

} // End of namespace Engines
