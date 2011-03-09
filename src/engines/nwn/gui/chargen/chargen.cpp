/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/chargen/chargen.cpp
 *  The new character generator.
 */

#include "engines/nwn/module.h"

#include "engines/nwn/gui/chargen/chargen.h"

namespace Engines {

namespace NWN {

CharGenMenu::CharGenMenu(Module &module) : _module(&module) {
	load("cg_main");

	// TODO: "TitleLabel" misplaced!

	// TODO: Character trait buttons
	getWidget("GenderButton"   , true)->setDisabled(true);
	getWidget("RaceButton"     , true)->setDisabled(true);
	getWidget("PortraitButton" , true)->setDisabled(true);
	getWidget("ClassButton"    , true)->setDisabled(true);
	getWidget("AlignButton"    , true)->setDisabled(true);
	getWidget("AbilitiesButton", true)->setDisabled(true);
	getWidget("PackagesButton" , true)->setDisabled(true);
	getWidget("CustomizeButton", true)->setDisabled(true);

	// TODO: Reset
	getWidget("ResetButton", true)->setDisabled(true);

	// TODO: Play
	getWidget("PlayButton" , true)->setDisabled(true);
}

CharGenMenu::~CharGenMenu() {
}

void CharGenMenu::callbackActive(Widget &widget) {
	if (widget.getTag() == "CancelButton") {
		_returnCode = 1;
		return;
	}

}

} // End of namespace NWN

} // End of namespace Engines
