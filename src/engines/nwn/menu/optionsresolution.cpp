/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/optionsresolution.cpp
 *  The resolution options menu.
 */

#include "engines/nwn/menu/optionsresolution.h"

#include "engines/aurora/util.h"

namespace Engines {

namespace NWN {

OptionsResolutionMenu::OptionsResolutionMenu() {
	load("options_vidmodes");
}

OptionsResolutionMenu::~OptionsResolutionMenu() {
}

void OptionsResolutionMenu::callbackActive(Widget &widget) {
	if ((widget.getTag() == "CancelButton") ||
	    (widget.getTag() == "XButton")) {
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
