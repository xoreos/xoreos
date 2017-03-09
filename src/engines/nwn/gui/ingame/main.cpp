/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * xoreos is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * xoreos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xoreos. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file
 *  The NWN ingame main menu.
 */

#include "src/version/version.h"

#include "src/aurora/talkman.h"

#include "src/engines/nwn/version.h"

#include "src/engines/nwn/gui/widgets/button.h"
#include "src/engines/nwn/gui/widgets/label.h"

#include "src/engines/nwn/gui/dialogs/yesnocancel.h"
#include "src/engines/nwn/gui/dialogs/okcancel.h"

#include "src/engines/nwn/gui/options/game.h"
#include "src/engines/nwn/gui/options/video.h"
#include "src/engines/nwn/gui/options/sound.h"
#include "src/engines/nwn/gui/options/controls.h"

#include "src/engines/nwn/gui/ingame/main.h"

namespace Engines {

namespace NWN {

IngameMainMenu::IngameMainMenu(const Version &gameVersion, ::Engines::Console *console) :
	GUI(console), _gameVersion(&gameVersion) {

	load("options_main");

	// TODO: Load game
	getWidget("LoadButton", true)->setDisabled(true);

	// TODO: Save game
	getWidget("SaveButton", true)->setDisabled(true);

	// TODO: Save character
	getWidget("SaveCharButton", true)->setDisabled(true);

	_game.reset    (new OptionsGameMenu    (false, _console));
	_video.reset   (new OptionsVideoMenu   (false, _console));
	_sound.reset   (new OptionsSoundMenu   (false, _console));
	_controls.reset(new OptionsControlsMenu(false, _console));

	_quitPrompt.reset(new OKCancelDialog(TalkMan.getString(10308),
	                                     TalkMan.getString(8274), TalkMan.getString(8275)));
}

IngameMainMenu::~IngameMainMenu() {
}

void IngameMainMenu::initWidget(Widget &widget) {
	if (widget.getTag() == "BuildNumber") {
		Common::UString version = Common::UString(::Version::getProjectNameVersion()) +
		                          " v" + _gameVersion->getVersionString();

		dynamic_cast<WidgetLabel &>(widget).setText(version);
		dynamic_cast<WidgetLabel &>(widget).setColor(0.6f, 0.6f, 0.6f, 1.0f);
		return;
	}

	if (widget.getTag() == "SaveCharButton") {
		dynamic_cast<WidgetButton &>(widget).setText(TalkMan.getString(6560));
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
		if (sub(*_quitPrompt, 0, false) == 1) {
			_returnCode = 2;
			return;
		}

		show();
		return;
	}

	if (widget.getTag() == "GameButton") {
		sub(*_game);
		return;
	}

	if (widget.getTag() == "VideoOptionsButton") {
		sub(*_video);
		return;
	}

	if (widget.getTag() == "SoundOptionsButton") {
		sub(*_sound);
		return;
	}

	if (widget.getTag() == "ControlsButton") {
		sub(*_controls);
		return;
	}

}

} // End of namespace NWN

} // End of namespace Engines
