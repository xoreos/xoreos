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
 *  The options menu.
 */

#include "src/version/version.h"

#include "src/engines/nwn/version.h"

#include "src/engines/nwn/gui/widgets/panel.h"
#include "src/engines/nwn/gui/widgets/label.h"

#include "src/engines/nwn/gui/options/game.h"
#include "src/engines/nwn/gui/options/video.h"
#include "src/engines/nwn/gui/options/sound.h"
#include "src/engines/nwn/gui/options/controls.h"

#include "src/engines/nwn/gui/main/options.h"

namespace Engines {

namespace NWN {

OptionsMenu::OptionsMenu(const Version &gameVersion, ::Engines::Console *console) : GUI(console),
	_gameVersion(&gameVersion) {

	load("options_pregame");

	WidgetPanel *backdrop = new WidgetPanel(*this, "PNL_MAINMENU", "pnl_mainmenu");
	backdrop->setPosition(0.0f, 0.0f, 100.0f);
	addWidget(backdrop);

	_game.reset    (new OptionsGameMenu    (true, _console));
	_video.reset   (new OptionsVideoMenu   (true, _console));
	_sound.reset   (new OptionsSoundMenu   (true, _console));
	_controls.reset(new OptionsControlsMenu(true, _console));
}

OptionsMenu::~OptionsMenu() {
}

void OptionsMenu::initWidget(Widget &widget) {
	if (widget.getTag() == "BuildNumber") {
		Common::UString version = Common::UString(::Version::getProjectNameVersion()) +
		                          " v" + _gameVersion->getVersionString();

		dynamic_cast<WidgetLabel &>(widget).setText(version);
		dynamic_cast<WidgetLabel &>(widget).setColor(0.6f, 0.6f, 0.6f, 1.0f);
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
