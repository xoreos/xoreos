/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/chartype.cpp
 *  The new/premade character selection menu.
 */

#include "engines/nwn/menu/chartype.h"
#include "engines/nwn/menu/newgamefog.h"
#include "engines/nwn/menu/charnew.h"
#include "engines/nwn/menu/charpremade.h"

namespace Engines {

namespace NWN {

CharTypeMenu::CharTypeMenu() {
	load("pre_chartype");

	_fogs = new NewGameFogs(4);

	_charNew     = new CharNewMenu;
	_charPremade = new CharPremadeMenu;
}

CharTypeMenu::~CharTypeMenu() {
	delete _charPremade;
	delete _charNew;

	delete _fogs;
}

void CharTypeMenu::show() {
	_fogs->show();

	GUI::show();
}

void CharTypeMenu::callbackActive(Widget &widget) {
	if (widget.getTag() == "CancelButton") {
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "CreateNewButton") {
		sub(*_charNew);
		return;
	}

	if (widget.getTag() == "UsePremadeButton") {
		if (sub(*_charPremade) == 2)
			_returnCode = 2;

		return;
	}

}

} // End of namespace NWN

} // End of namespace Engines
