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

#include "common/configman.h"

#include "graphics/graphics.h"

#include "engines/nwn/types.h"

#include "engines/nwn/menu/new.h"
#include "engines/nwn/menu/newmodule.h"

#include "engines/aurora/util.h"

namespace Engines {

namespace NWN {

NewMenu::NewMenu(ModuleContext &moduleContext) : _moduleContext(&moduleContext) {
	load("pre_newgame");

	_hasXP = ConfigMan.getBool("NWN_hasXP1") || ConfigMan.getBool("NWN_hasXP2");

	if (_hasXP)
		// If we have an expansion, hide the "Other modules" button, it's
		// already in the campaign menu
		getWidget("OtherButton", true)->setInvisible(true);

	_module = 0;
	if (!_hasXP)
		_module = new NewModuleMenu(*_moduleContext);
}

NewMenu::~NewMenu() {
	delete _module;
}

void NewMenu::callbackRun() {
	int startCode = _startCode;
	_startCode = 0;

	if ((startCode == 3) && _module)
		if (sub(*_module, 0) == 3)
			_returnCode = 3;
}

void NewMenu::callbackActive(Widget &widget) {
	if (widget.getTag() == "CancelButton") {
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "OtherButton") {
		if (sub(*_module) == 3)
			_returnCode = 3;
		return;
	}

	if (widget.getTag() == "PreludeButton") {
		_moduleContext->module = "prelude.nwm";
		_returnCode = 2;
		return;
	}

	if (widget.getTag() == "Chapter1Button") {
		_moduleContext->module = "chapter1.nwm";
		_returnCode = 2;
		return;
	}

	if (widget.getTag() == "Chapter2Button") {
		_moduleContext->module = "chapter2.nwm";
		_returnCode = 2;
		return;
	}

	if (widget.getTag() == "Chapter3Button") {
		_moduleContext->module = "chapter3.nwm";
		_returnCode = 2;
		return;
	}

	if (widget.getTag() == "Chapter4Button") {
		_moduleContext->module = "chapter4.nwm";
		_returnCode = 2;
		return;
	}
}

} // End of namespace NWN

} // End of namespace Engines
