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

#include "src/engines/aurora/widget.h"

#include "src/engines/kotor/version.h"

#include "src/engines/kotor/gui/main/options.h"
#include "src/engines/kotor/gui/options/gameplay.h"
#include "src/engines/kotor/gui/options/feedback.h"
#include "src/engines/kotor/gui/options/autopause.h"
#include "src/engines/kotor/gui/options/graphics.h"
#include "src/engines/kotor/gui/options/sound.h"

#include "src/engines/aurora/kotorjadegui/button.h"
#include "src/engines/aurora/kotorjadegui/label.h"

namespace Engines {

namespace KotOR {

OptionsMenu::OptionsMenu(const Version &gameVersion, ::Engines::Console *console) : GUI(console) {
	load("optionsmain");

	addBackground(kBackgroundTypeMenu);

	_gameplay.reset(new OptionsGameplayMenu(_console));
	_feedback.reset(new OptionsFeedbackMenu(_console));
	_autopause.reset(new OptionsAutoPauseMenu(_console));
	_graphics.reset(new OptionsGraphicsMenu(_console));
	_sound.reset(new OptionsSoundMenu(_console));

	Common::UString versionString = Common::UString(::Version::getProjectNameVersion());

	if (gameVersion.hasVersion())
		versionString += " v" + gameVersion.getVersionString();

	WidgetLabel *lblVersion = new WidgetLabel(*this, "Version");
	addWidget(lblVersion);

	lblVersion->createText("fnt_console", versionString);

	const float x =  (800.0f / 2.0f) - lblVersion->getWidth();
	const float y = -(600.0f / 2.0f);

	lblVersion->setPosition(x, y, -50.0f);
	lblVersion->setTextColor(0.6f, 0.6f, 0.6f, 1.0f);
}

OptionsMenu::~OptionsMenu() {
}

void OptionsMenu::callbackActive(Widget &widget) {
	if (widget.getTag() == "BTN_GAMEPLAY") {
		sub(*_gameplay);
		return;
	}

	if (widget.getTag() == "BTN_FEEDBACK") {
		sub(*_feedback);
		return;
	}

	if (widget.getTag() == "BTN_AUTOPAUSE") {
		sub(*_autopause);
		return;
	}

	if (widget.getTag() == "BTN_GRAPHICS") {
		sub(*_graphics);
		return;
	}

	if (widget.getTag() == "BTN_SOUND") {
		sub(*_sound);
		return;
	}

	if (widget.getTag() == "BTN_BACK") {
		_returnCode = 1;
		return;
	}
}

} // End of namespace KotOR

} // End of namespace Engines
