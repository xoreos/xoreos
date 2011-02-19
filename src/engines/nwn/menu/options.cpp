/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/options.cpp
 *  The options menu.
 */

#include "engines/nwn/menu/options.h"
#include "engines/nwn/menu/optionsgame.h"
#include "engines/nwn/menu/optionsvideo.h"
#include "engines/nwn/menu/optionssound.h"
#include "engines/nwn/menu/optionscontrols.h"

#include "engines/aurora/util.h"

namespace Engines {

namespace NWN {

OptionsMenu::OptionsMenu() {
	load("options_pregame");

	_game     = new OptionsGameMenu;
	_video    = new OptionsVideoMenu;
	_sound    = new OptionsSoundMenu;
	_controls = new OptionsControlsMenu;
}

OptionsMenu::~OptionsMenu() {
	delete _controls;
	delete _sound;
	delete _video;
	delete _game;
}

void OptionsMenu::initWidget(WidgetLabel &widget) {
	if (widget.getTag() == "BuildNumber") {
		widget.setText(PACKAGE_STRING);
		widget.setColor(0.6, 0.6, 0.6, 1.0);
		return;
	}
}

void OptionsMenu::callbackActive(Widget &widget) {
	if ((widget.getTag() == "CloseButton") ||
	    (widget.getTag() == "XButton")) {
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "GameButton") {
		sub(*_game);
		return;
	}

	if (widget.getTag() == "VideoOptsButton") {
		sub(*_video);
		return;
	}

	if (widget.getTag() == "SoundButton") {
		sub(*_sound);
		return;
	}

	if (widget.getTag() == "ControlButton") {
		sub(*_controls);
		return;
	}
}

} // End of namespace NWN

} // End of namespace Engines
