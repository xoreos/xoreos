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

#include "engines/nwn/module.h"

#include "engines/nwn/menu/new.h"
#include "engines/nwn/menu/newmodule.h"

#include "engines/aurora/util.h"

namespace Engines {

namespace NWN {

NewMenu::NewMenu(Module &module, GUI &charType) : _module(&module), _charType(&charType) {
	load("pre_newgame");

	_hasXP = ConfigMan.getBool("NWN_hasXP1") || ConfigMan.getBool("NWN_hasXP2");

	if (_hasXP)
		// If we have an expansion, hide the "Other modules" button, it's
		// already in the campaign menu
		getWidget("OtherButton", true)->setInvisible(true);

	_modules = 0;
	if (!_hasXP)
		_modules = new NewModuleMenu(*_module, *_charType);
}

NewMenu::~NewMenu() {
	delete _modules;
}

void NewMenu::callbackActive(Widget &widget) {
	if (widget.getTag() == "CancelButton") {
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "OtherButton") {
		if (sub(*_modules, 0, false) == 2) {
			_returnCode = 2;
			return;
		}

		show();
		return;
	}

	if (widget.getTag() == "PreludeButton") {
		loadModule("prelude.nwm");
		return;
	}

	if (widget.getTag() == "Chapter1Button") {
		loadModule("chapter1.nwm");
		return;
	}

	if (widget.getTag() == "Chapter2Button") {
		loadModule("chapter2.nwm");
		return;
	}

	if (widget.getTag() == "Chapter3Button") {
		loadModule("chapter3.nwm");
		return;
	}

	if (widget.getTag() == "Chapter4Button") {
		loadModule("chapter4.nwm");
		return;
	}
}

void NewMenu::loadModule(const Common::UString &module) {
	if (_module->loadModule(module)) {
		if (sub(*_charType, 0, false) == 2) {
			_returnCode = 2;
			return;
		}

		show();
	}
}

} // End of namespace NWN

} // End of namespace Engines
