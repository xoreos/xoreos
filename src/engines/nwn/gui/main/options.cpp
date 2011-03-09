/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/main/options.cpp
 *  The options menu.
 */

#include "engines/nwn/gui/widgets/panel.h"
#include "engines/nwn/gui/widgets/label.h"

#include "engines/nwn/gui/options/game.h"
#include "engines/nwn/gui/options/video.h"
#include "engines/nwn/gui/options/sound.h"
#include "engines/nwn/gui/options/controls.h"

#include "engines/nwn/gui/main/options.h"

namespace Engines {

namespace NWN {

OptionsMenu::OptionsMenu() {
	load("options_pregame");

	WidgetPanel *backdrop = new WidgetPanel(*this, "PNL_MAINMENU", "pnl_mainmenu");
	backdrop->setPosition(0.0, 0.0, -10.0);
	addWidget(backdrop);

	_game     = new OptionsGameMenu(true);
	_video    = new OptionsVideoMenu(true);
	_sound    = new OptionsSoundMenu(true);
	_controls = new OptionsControlsMenu(true);
}

OptionsMenu::~OptionsMenu() {
	delete _controls;
	delete _sound;
	delete _video;
	delete _game;
}

void OptionsMenu::initWidget(Widget &widget) {
	if (widget.getTag() == "BuildNumber") {
		dynamic_cast<WidgetLabel &>(widget).setText(PACKAGE_STRING);
		dynamic_cast<WidgetLabel &>(widget).setColor(0.6, 0.6, 0.6, 1.0);
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
