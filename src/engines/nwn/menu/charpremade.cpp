/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/charpremade.cpp
 *  The premade character chooser.
 */

#include "engines/nwn/menu/charpremade.h"
#include "engines/nwn/menu/charnew.h"

namespace Engines {

namespace NWN {

CharPremadeMenu::CharPremadeMenu() {
	load("pre_playmod");

	// TODO: "Title" misplaced!
	// TODO: Title bar misplaced!
	// TODO: "SaveLabel" should wrap!

	// TODO: Customize character
	getWidget("CustomCharButton", true)->setDisabled(true);

	// TODO: Show pregenerated characters
	getWidget("SaveGameBox"     , true)->setDisabled(true);

	// TODO: Delete character
	getWidget("DeleteCharButton", true)->setDisabled(true);

	// TODO: Play game
	getWidget("PlayButton"      , true)->setDisabled(true);

	// TODO: Characters
	getWidget("ButtonList"      , true)->setDisabled(true);

	_charNew = new CharNewMenu;
}

CharPremadeMenu::~CharPremadeMenu() {
	delete _charNew;
}

void CharPremadeMenu::callbackActive(Widget &widget) {
	if (widget.getTag() == "CancelButton") {
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "NewCharButton") {
		sub(*_charNew);
		return;
	}

}

} // End of namespace NWN

} // End of namespace Engines
