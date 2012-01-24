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
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

	/** @file engines/nwn/gui/main/newcamp.cpp
	 *  The new campaign menu.
	 */

#include "common/configman.h"

#include "engines/nwn/gui/main/newcamp.h"
#include "engines/nwn/gui/main/new.h"
#include "engines/nwn/gui/main/newxp1.h"
#include "engines/nwn/gui/main/newxp2.h"
#include "engines/nwn/gui/main/newmodule.h"

namespace Engines {

namespace NWN {

NewCampMenu::NewCampMenu(Module &module, GUI &charType) :
	_module(&module), _charType(&charType) {

	load("pre_campaign");

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

	_base    = new NewMenu(*_module, *_charType);
	_xp1     = new NewXP1Menu(*_module, *_charType);
	_xp2     = new NewXP2Menu(*_module, *_charType);
	_modules = new NewModuleMenu(*_module, *_charType);
}

NewCampMenu::~NewCampMenu() {
	delete _modules;
	delete _xp2;
	delete _xp1;
	delete _base;
}

void NewCampMenu::callbackActive(Widget &widget) {
	if (widget.getTag() == "CancelButton") {
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "NWNButton") {
		if (sub(*_base, 0, false) == 2) {
			_returnCode = 2;
			return;
		}

		show();
		return;
	}

	if (widget.getTag() == "NWNXP1Button") {
		if (sub(*_xp1, 0, false) == 2) {
			_returnCode = 2;
			return;
		}

		show();
		return;
	}

	if (widget.getTag() == "NWNXP2Button") {
		if (sub(*_xp2, 0, false) == 2) {
			_returnCode = 2;
			return;
		}

		show();
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

}

} // End of namespace NWN

} // End of namespace Engines
