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
 *  The gameplay menu.
 */


#include "common/util.h"
#include "common/configman.h"

#include "aurora/talkman.h"

#include "engines/aurora/widget.h"
#include "engines/kotor/gui/options/gameplay.h"
#include "engines/kotor/gui/options/mousesettings.h"
#include "engines/kotor/gui/options/keyboardconfig.h"
#include "engines/kotor/gui/widgets/button.h"
#include "engines/kotor/gui/widgets/checkbox.h"

namespace Engines {

namespace KotOR {

OptionsGameplayMenu::OptionsGameplayMenu() {
	load("optgameplay");
	_mousesettings = new OptionsMouseSettingsMenu();
	_keyboardconfiguration = new OptionsKeyboardConfigurationMenu();
	//Hardcoded, the gui file returns 1.0, 1.0, 1.0, 1.0
	getButton("BTN_DIFFLEFT", true)->setColor(0, 0.658824, 0.980392, 1);
	getButton("BTN_DIFFRIGHT", true)->setColor(0, 0.658824, 0.980392, 1);
	getCheckBox("CB_LEVELUP", true)->setColor(0, 0.658824, 0.980392, 1);
	getCheckBox("CB_INVERTCAM", true)->setColor(0, 0.658824, 0.980392, 1);
	getCheckBox("CB_AUTOSAVE", true)->setColor(0, 0.658824, 0.980392, 1);
	getCheckBox("CB_REVERSE", true)->setColor(0, 0.658824, 0.980392, 1);
	getCheckBox("CB_DISABLEMOVE", true)->setColor(0, 0.658824, 0.980392, 1);
}
OptionsGameplayMenu::~OptionsGameplayMenu() {
	delete _mousesettings;
	delete _keyboardconfiguration;
}


void OptionsGameplayMenu::show() {
	GUI::show();

	_difficulty = CLIP(ConfigMan.getInt("Difficulty Level", 0), 0, 2);
	updateDifficulty(_difficulty);
}


void OptionsGameplayMenu::callbackActive(Widget &widget) {

	if (widget.getTag() == "BTN_DIFFRIGHT") {
		_difficulty++;
		if (_difficulty > 2) {
			_difficulty = 2;
		}
		updateDifficulty(_difficulty);
		return;
	}

	if (widget.getTag() == "BTN_DIFFLEFT") {
		_difficulty--;
		if (_difficulty < 0) {
			_difficulty = 0;
		}
		updateDifficulty(_difficulty);
		return;
	}

	if (widget.getTag() == "BTN_MOUSE") {
		sub(*_mousesettings);
		return;
	}

	if (widget.getTag() == "BTN_KEYMAP") {
		sub(*_keyboardconfiguration);
		return;
	}

	if (widget.getTag() == "BTN_DEFAULT") {
		_difficulty = 1;
		updateDifficulty(_difficulty);
	}

	if (widget.getTag() == "BTN_BACK") {
		_returnCode = 1;
		return;
	}
}

void OptionsGameplayMenu::updateDifficulty(int difficulty) {
	WidgetButton &diffButton = *getButton("BTN_DIFFICULTY", true);
	WidgetButton &leftButton = *getButton("BTN_DIFFLEFT", true);
	WidgetButton &rightButton = *getButton("BTN_DIFFRIGHT", true);

	diffButton.setText(TalkMan.getString(42335 + difficulty));

	if (_difficulty == 0) {
		leftButton.hide();
	} else {
		leftButton.show();
	}

	if (_difficulty == 2) {
		rightButton.hide();
	} else {
		rightButton.show();
	}
}

void OptionsGameplayMenu::adoptChanges() {
	ConfigMan.setInt("Difficulty Level", _difficulty, true);
}

} // End of namespace KotOR

} // End of namespace Engines



