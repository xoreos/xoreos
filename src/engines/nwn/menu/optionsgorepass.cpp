/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/optionsgorepass.cpp
 *  The violence/gore password options menu.
 */

#include "engines/nwn/menu/optionsgorepass.h"

#include "engines/aurora/util.h"

namespace Engines {

namespace NWN {

OptionsGorePasswordMenu::OptionsGorePasswordMenu() {
	load("options_gorepass");

	// TODO: Password
	getWidget("OldPassword"    , true)->setDisabled(true);
	getWidget("NewPassword"    , true)->setDisabled(true);
	getWidget("ConfirmPassword", true)->setDisabled(true);
}

OptionsGorePasswordMenu::~OptionsGorePasswordMenu() {
}

void OptionsGorePasswordMenu::callbackActive(Widget &widget) {
	if ((widget.getTag() == "CancelButton") ||
	    (widget.getTag() == "CTL_BTN_X")) {
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "OkButton") {
		_returnCode = 2;
		return;
	}
}

} // End of namespace NWN

} // End of namespace Engines
