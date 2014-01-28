/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file engines/nwn/gui/main/new.cpp
 *  The new game menu.
 */

#include "common/configman.h"

#include "engines/nwn/module.h"

#include "engines/nwn/gui/widgets/nwnwidget.h"

#include "engines/nwn/gui/main/new.h"
#include "engines/nwn/gui/main/newmodule.h"

namespace Engines {

namespace NWN {

NewMenu::NewMenu(Module &module, GUI &charType) : _module(&module), _charType(&charType), _modules(0) {
	setPosition(0.0, 0.0, -300.0);
	load("pre_newgame");

	_hasXP = ConfigMan.getBool("NWN_hasXP1") || ConfigMan.getBool("NWN_hasXP2");

	if (_hasXP)
		// If we have an expansion, hide the "Other modules" button, it's
		// already in the campaign menu
		getWidget("OtherButton", true)->setInvisible(true);
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
		if (!_modules)
			_modules = new NewModuleMenu(*_module, *_charType);

		if (sub(*_modules, 0, false) == 2) {
			_returnCode = 2;
			return;
		}

		setVisible(true);
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

		setVisible(true);
	}
}

} // End of namespace NWN

} // End of namespace Engines
