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
 *  The auto pause menu.
 */

#include "src/common/configman.h"

#include "src/engines/aurora/widget.h"

#include "src/engines/odyssey/checkbox.h"

#include "src/engines/kotor/gui/options/autopause.h"

namespace Engines {

namespace KotOR {

OptionsAutoPauseMenu::OptionsAutoPauseMenu(::Engines::Console *console) : KotORBase::GUI(console) {
	load("optautopause");

	addBackground(KotORBase::kBackgroundTypeMenu);

	// Hardcoded, the gui file returns 1.0, 1.0, 1.0, 1.0
	getCheckBox("CB_ENDROUND", true)->setColor(0.0f, 0.658824f, 0.980392f, 1.0f);
	getCheckBox("CB_ENEMYSIGHTED", true)->setColor(0.0f, 0.658824f, 0.980392f, 1.0f);
	getCheckBox("CB_MINESIGHTED", true)->setColor(0.0f, 0.658824f, 0.980392f, 1.0f);
	getCheckBox("CB_PARTYKILLED", true)->setColor(0.0f, 0.658824f, 0.980392f, 1.0f);
	getCheckBox("CB_ACTIONMENU", true)->setColor(0.0f, 0.658824f, 0.980392f, 1.0f);
	getCheckBox("CB_TRIGGERS", true)->setColor(0.0f, 0.658824f, 0.980392f, 1.0f);

	readConfig();
}

OptionsAutoPauseMenu::~OptionsAutoPauseMenu() {
}

void OptionsAutoPauseMenu::show() {
	readConfig();
	displayConfig();

	GUI::show();
}

void OptionsAutoPauseMenu::callbackActive(Widget &widget) {
	if (widget.getTag() == "BTN_DEFAULT") {
		setDefault();
		displayConfig();
	}

	if (widget.getTag() == "BTN_BACK") {
		adoptChanges();
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "CB_ENDROUND") {
		_endOfCombatRound = getCheckBoxState("CB_ENDROUND");
		return;
	}

	if (widget.getTag() == "CB_ENEMYSIGHTED") {
		_enemySighted = getCheckBoxState("CB_ENEMYSIGHTED");
		return;
	}

	if (widget.getTag() == "CB_MINESIGHTED") {
		_mineSighted = getCheckBoxState("CB_MINESIGHTED");
		return;
	}

	if (widget.getTag() == "CB_PARTYKILLED") {
		_partyMemberDown = getCheckBoxState("CB_PARTYKILLED");
		return;
	}

	if (widget.getTag() == "CB_ACTIONMENU") {
		_actionMenuUsed = getCheckBoxState("CB_ACTIONMENU");
		return;
	}

	if (widget.getTag() == "CB_TRIGGERS") {
		_newTargetSelected = getCheckBoxState("CB_TRIGGERS");
		return;
	}
}

void OptionsAutoPauseMenu::setDefault() {
	_endOfCombatRound = ConfigMan.getDefaultBool("endofcombatround");
	_enemySighted = ConfigMan.getDefaultBool("enemysighted");
	_mineSighted = ConfigMan.getDefaultBool("minesighted");
	_partyMemberDown = ConfigMan.getDefaultBool("partymemberdown");
	_actionMenuUsed = ConfigMan.getDefaultBool("actionmenuused");
	_newTargetSelected = ConfigMan.getDefaultBool("newtargetselected");
}

void OptionsAutoPauseMenu::readConfig() {
	_endOfCombatRound = ConfigMan.getBool("endofcombatround");
	_enemySighted = ConfigMan.getBool("enemysighted");
	_mineSighted = ConfigMan.getBool("minesighted");
	_partyMemberDown = ConfigMan.getBool("partymemberdown");
	_actionMenuUsed = ConfigMan.getBool("actionmenuused");
	_newTargetSelected = ConfigMan.getBool("newtargetselected");
}

void OptionsAutoPauseMenu::displayConfig() {
	setCheckBoxState("CB_ENDROUND", _endOfCombatRound);
	setCheckBoxState("CB_ENEMYSIGHTED", _enemySighted);
	setCheckBoxState("CB_MINESIGHTED", _mineSighted);
	setCheckBoxState("CB_PARTYKILLED", _partyMemberDown);
	setCheckBoxState("CB_ACTIONMENU", _actionMenuUsed);
	setCheckBoxState("CB_TRIGGERS", _newTargetSelected);
}

void OptionsAutoPauseMenu::adoptChanges() {
	ConfigMan.setBool("endofcombatround", _endOfCombatRound, true);
	ConfigMan.setBool("enemysighted", _enemySighted, true);
	ConfigMan.setBool("minesighted", _mineSighted, true);
	ConfigMan.setBool("partymemberdown", _partyMemberDown, true);
	ConfigMan.setBool("actionmenuused", _actionMenuUsed, true);
	ConfigMan.setBool("newtargetselected", _newTargetSelected, true);
}

} // End of namespace KotOR

} // End of namespace Engines
