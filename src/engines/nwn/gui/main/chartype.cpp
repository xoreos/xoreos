/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/main/chartype.cpp
 *  The new/premade character selection menu.
 */

#include "engines/nwn/gui/chargen/chargen.h"

#include "engines/nwn/gui/main/chartype.h"
#include "engines/nwn/gui/main/charpremade.h"

namespace Engines {

namespace NWN {

CharTypeMenu::CharTypeMenu(Module &module) : _module(&module) {
	load("pre_chartype");

	_charGen     = new CharGenMenu(*_module);
	_charPremade = new CharPremadeMenu(*_module);
}

CharTypeMenu::~CharTypeMenu() {
	delete _charPremade;
	delete _charGen;
}

void CharTypeMenu::callbackActive(Widget &widget) {
	if (widget.getTag() == "CancelButton") {
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "CreateNewButton") {
		sub(*_charGen);
		return;
	}

	if (widget.getTag() == "UsePremadeButton") {
		if (sub(*_charPremade, 0, false) == 2) {
			_returnCode = 2;
			return;
		}

		show();
		return;
	}

}

} // End of namespace NWN

} // End of namespace Engines
