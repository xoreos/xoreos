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


#include "src/common/util.h"
#include "src/common/configman.h"

#include "src/aurora/talkman.h"

#include "src/engines/aurora/widget.h"
#include "src/engines/kotor/gui/options/gameplay.h"
#include "src/engines/kotor/gui/options/mousesettings.h"
#include "src/engines/kotor/gui/options/keyboardconfig.h"
#include "src/engines/kotor/gui/widgets/button.h"
#include "src/engines/kotor/gui/widgets/checkbox.h"

namespace Engines {

namespace KotOR {

OptionsGameplayMenu::OptionsGameplayMenu(::Engines::Console *console) : GUI(console) {
	load("optgameplay");

	_mousesettings.reset(new OptionsMouseSettingsMenu(_console));
	_keyboardconfiguration.reset(new OptionsKeyboardConfigurationMenu(_console));

	//Hardcoded, the gui file returns 1.0, 1.0, 1.0, 1.0
	getButton("BTN_DIFFLEFT", true)->setColor(0.0f, 0.658824f, 0.980392f, 1.0f);
	getButton("BTN_DIFFLEFT", true)->setStaticHighlight();
	getButton("BTN_DIFFRIGHT", true)->setColor(0.0f, 0.658824f, 0.980392f, 1.0f);
	getButton("BTN_DIFFRIGHT", true)->setStaticHighlight();
	getCheckBox("CB_LEVELUP", true)->setColor(0.0f, 0.658824f, 0.980392f, 1.0f);
	getCheckBox("CB_INVERTCAM", true)->setColor(0.0f, 0.658824f, 0.980392f, 1.0f);
	getCheckBox("CB_AUTOSAVE", true)->setColor(0.0f, 0.658824f, 0.980392f, 1.0f);
	getCheckBox("CB_REVERSE", true)->setColor(0.0f, 0.658824f, 0.980392f, 1.0f);
	getCheckBox("CB_DISABLEMOVE", true)->setColor(0.0f, 0.658824f, 0.980392f, 1.0f);
}
OptionsGameplayMenu::~OptionsGameplayMenu() {
}


void OptionsGameplayMenu::show() {
	GUI::show();

	_difficulty = CLIP(ConfigMan.getInt("difficultylevel", 1), 0, 2);
	updateDifficulty(_difficulty);

	_autoLevelUp = ConfigMan.getBool("autolevelup", false);
	setCheckBoxState("CB_LEVELUP", _autoLevelUp);

	_mouseMove = ConfigMan.getBool("mousemove", false);
	setCheckBoxState("CB_INVERTCAM", _mouseMove);

	_autoSave = ConfigMan.getBool("autosave", true);
	setCheckBoxState("CB_AUTOSAVE", _autoSave);

	_reverseMinigameY = ConfigMan.getBool("reverseminigameyaxis", false);
	setCheckBoxState("CB_REVERSE", _reverseMinigameY);

	_combatMovement = ConfigMan.getBool("combatmovement", true);
	setCheckBoxState("CB_DISABLEMOVE", _combatMovement);
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
		adoptChanges();
		sub(*_mousesettings);
		return;
	}

	if (widget.getTag() == "BTN_KEYMAP") {
		adoptChanges();
		sub(*_keyboardconfiguration);
		return;
	}

	if (widget.getTag() == "BTN_DEFAULT") {
		_difficulty = 1;
		updateDifficulty(_difficulty);

		_autoLevelUp = false;
		setCheckBoxState("CB_LEVELUP", _autoLevelUp);

		_mouseMove = false;
		setCheckBoxState("CB_INVERTCAM", _mouseMove);

		_autoSave = true;
		setCheckBoxState("CB_AUTOSAVE", _autoSave);

		_reverseMinigameY = false;
		setCheckBoxState("CB_REVERSE", _reverseMinigameY);

		_combatMovement = true;
		setCheckBoxState("CB_DISABLEMOVE", _combatMovement);
	}

	if (widget.getTag() == "BTN_BACK") {
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "CB_LEVELUP") {
		_autoLevelUp = getCheckBoxState("CB_LEVELUP");
		return;
	}

	if (widget.getTag() == "CB_INVERTCAM") {
		_mouseMove = getCheckBoxState("CB_INVERTCAM");
		return;
	}

	if (widget.getTag() == "CB_AUTOSAVE") {
		_autoSave = getCheckBoxState("CB_AUTOSAVE");
		return;
	}

	if (widget.getTag() == "CB_REVERSE") {
		_reverseMinigameY = getCheckBoxState("CB_REVERSE");
		return;
	}

	if (widget.getTag() == "CB_DISABLEMOVE") {
		_combatMovement = getCheckBoxState("CB_DISABLEMOVE");
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
	ConfigMan.setInt("difficultylevel", _difficulty, true);
	ConfigMan.setBool("autolevelup", _autoLevelUp, true);
	ConfigMan.setBool("mousemove", _mouseMove, true);
	ConfigMan.setBool("autosave", _autoSave, true);
	ConfigMan.setBool("reverseminigameyaxis", _reverseMinigameY, true);
	ConfigMan.setBool("combatmovement", _combatMovement, true);
}

} // End of namespace KotOR

} // End of namespace Engines



