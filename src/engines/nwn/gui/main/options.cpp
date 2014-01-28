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
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file engines/nwn/gui/main/options.cpp
 *  The options menu.
 */

#include "common/version.h"

#include "engines/nwn/gui/widgets/panel.h"
#include "engines/nwn/gui/widgets/label.h"

#include "engines/nwn/gui/options/game.h"
#include "engines/nwn/gui/options/video.h"
#include "engines/nwn/gui/options/sound.h"
#include "engines/nwn/gui/options/controls.h"

#include "engines/nwn/gui/main/options.h"

namespace Engines {

namespace NWN {

OptionsMenu::OptionsMenu() : _game(0), _video(0), _sound(0), _controls(0) {
	setPosition(0.0, 0.0, -290.0);
	load("options_pregame");

	WidgetPanel *backdrop = new WidgetPanel(*this, "PNL_MAINMENU", "pnl_mainmenu");
	backdrop->setPosition(0.0, 0.0, -300.0);
	addWidget(backdrop);
}

OptionsMenu::~OptionsMenu() {
	delete _controls;
	delete _sound;
	delete _video;
	delete _game;
}

void OptionsMenu::initWidget(Widget &widget) {
	if (widget.getTag() == "BuildNumber") {
		dynamic_cast<WidgetLabel &>(widget).setText(XOREOS_NAMEVERSION);
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
		if (!_game)
			_game = new OptionsGameMenu(true);

		sub(*_game);
		return;
	}

	if (widget.getTag() == "VideoOptsButton") {
		if (!_video)
			_video = new OptionsVideoMenu(true);

		sub(*_video);
		return;
	}

	if (widget.getTag() == "SoundButton") {
		if (!_sound)
			_sound = new OptionsSoundMenu(true);

		sub(*_sound);
		return;
	}

	if (widget.getTag() == "ControlButton") {
		if (!_controls)
			_controls = new OptionsControlsMenu(true);

		sub(*_controls);
		return;
	}
}

} // End of namespace NWN

} // End of namespace Engines
