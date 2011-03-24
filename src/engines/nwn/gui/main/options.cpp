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
	backdrop->setPosition(0.0, 0.0, 100.0);
	addWidget(backdrop);

	_game     = 0;
	_video    = 0;
	_sound    = 0;
	_controls = 0;
}

OptionsMenu::~OptionsMenu() {
	delete _controls;
	delete _sound;
	delete _video;
	delete _game;
}

void OptionsMenu::createGame() {
	if (_game)
		return;

	_game = new OptionsGameMenu(true);
}

void OptionsMenu::createVideo() {
	if (_video)
		return;

	_video = new OptionsVideoMenu(true);
}

void OptionsMenu::createSound() {
	if (_sound)
		return;

	_sound = new OptionsSoundMenu(true);
}

void OptionsMenu::createControls() {
	if (_controls)
		return;

	_controls = new OptionsControlsMenu(true);
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
		createGame();

		sub(*_game);
		return;
	}

	if (widget.getTag() == "VideoOptsButton") {
		createVideo();

		sub(*_video);
		return;
	}

	if (widget.getTag() == "SoundButton") {
		createSound();

		sub(*_sound);
		return;
	}

	if (widget.getTag() == "ControlButton") {
		createControls();

		sub(*_controls);
		return;
	}
}

} // End of namespace NWN

} // End of namespace Engines
