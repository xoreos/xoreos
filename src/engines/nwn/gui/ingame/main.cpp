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

/** @file engines/nwn/gui/ingame/main.cpp
 *  The NWN ingame main menu.
 */

#include "common/version.h"

#include "aurora/talkman.h"

#include "engines/nwn/gui/widgets/label.h"

#include "engines/nwn/gui/dialogs/yesnocancel.h"
#include "engines/nwn/gui/dialogs/okcancel.h"

#include "engines/nwn/gui/options/game.h"
#include "engines/nwn/gui/options/video.h"
#include "engines/nwn/gui/options/sound.h"
#include "engines/nwn/gui/options/controls.h"

#include "engines/nwn/gui/ingame/main.h"

namespace Engines {

namespace NWN {

IngameMainMenu::IngameMainMenu(Module &module) : _module(&module), _game(0), _video(0), _sound(0), _controls(0), _quitPrompt(0) {
	setPosition(0.0, 0.0, -300.0);
	load("options_main");

	// TODO: Load game
	getWidget("LoadButton", true)->setDisabled(true);

	// TODO: Save game
	getWidget("SaveButton", true)->setDisabled(true);

	// TODO: Save character
	getWidget("SaveCharButton", true)->setDisabled(true);
}

IngameMainMenu::~IngameMainMenu() {
	delete _quitPrompt;

	delete _controls;
	delete _sound;
	delete _video;
	delete _game;
}

void IngameMainMenu::initWidget(Widget &widget) {
	if (widget.getTag() == "BuildNumber") {
		dynamic_cast<WidgetLabel &>(widget).setText(XOREOS_NAMEVERSION);
		dynamic_cast<WidgetLabel &>(widget).setColor(0.6, 0.6, 0.6, 1.0);
		return;
	}

	if (widget.getTag() == "SaveCharButton#Caption") {
		dynamic_cast<WidgetLabel &>(widget).setText(TalkMan.getString(6560));
		return;
	}
}

void IngameMainMenu::callbackActive(Widget &widget) {
	if ((widget.getTag() == "ResumeGameButton") ||
	    (widget.getTag() == "XButton")) {
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "ExitButton") {
		if (!_quitPrompt)
			_quitPrompt = new OKCancelDialog(TalkMan.getString(10308),
			                                 TalkMan.getString(8274), TalkMan.getString(8275));

		if (sub(*_quitPrompt, 0, false) == 1) {
			_returnCode = 2;
			return;
		}

		setVisible(true);
		return;
	}


	if (widget.getTag() == "GameButton") {
		if (!_game)
			_game = new OptionsGameMenu(false);

		sub(*_game);
		return;
	}

	if (widget.getTag() == "VideoOptionsButton") {
		if (!_video)
			_video = new OptionsVideoMenu(false, _module);

		sub(*_video);
		return;
	}

	if (widget.getTag() == "SoundOptionsButton") {
		if (!_sound)
			_sound = new OptionsSoundMenu(false);

		sub(*_sound);
		return;
	}

	if (widget.getTag() == "ControlsButton") {
		if (!_controls)
			_controls = new OptionsControlsMenu(false);

		sub(*_controls);
		return;
	}

}

} // End of namespace NWN

} // End of namespace Engines
