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
 *  The panel to customize a quick character.
 */

#include "src/engines/odyssey/label.h"
#include "src/engines/odyssey/button.h"

#include "src/engines/kotor/gui/chargen/customchar.h"

namespace Engines {

namespace KotOR {

CustomCharPanel::CustomCharPanel(CharacterGenerationMenu *charGenMenu, Console *console) :
		KotORBase::GUI(console),
		_charGen(charGenMenu) {

	load("custpnl");

	setPosition(137, 16, 0);

	float width = getLabel("LBL_BG")->getWidth();
	float height = getLabel("LBL_BG")->getHeight();

	getLabel("LBL_BG")->setScissor(5, 40, width - 5, height - 60);

	getButton("BTN_STEPNAME1")->setDisableHoverSound(true);
	getButton("BTN_STEPNAME2")->setDisableHoverSound(true);
	getButton("BTN_STEPNAME3")->setDisableHoverSound(true);
	getButton("BTN_STEPNAME4")->setDisableHoverSound(true);
	getButton("BTN_STEPNAME5")->setDisableHoverSound(true);
	getButton("BTN_STEPNAME6")->setDisableHoverSound(true);

	updateButtons();
}

void CustomCharPanel::callbackActive(Widget &widget) {
	if (widget.getTag() == "BTN_CANCEL") {
		_charGen->showQuickOrCustom();
		return;
	}

	if (widget.getTag() == "BTN_BACK") {
		_charGen->decStep();
		updateButtons();
		return;
	}

	if (widget.getTag() == "BTN_STEPNAME1") {
		_charGen->showPortrait();
		updateButtons();
		return;
	}
	// Attributes / Abilities
	if (widget.getTag() == "BTN_STEPNAME2") {
		_charGen->showAbilities();
		updateButtons();
		return;
	}
	// Skills
	if (widget.getTag() == "BTN_STEPNAME3") {
		_charGen->showSkills();
		updateButtons();
		return;
	}
	// Feats
	if (widget.getTag() == "BTN_STEPNAME4") {
		_charGen->showFeats();
		updateButtons();
		return;
	}
	if (widget.getTag() == "BTN_STEPNAME5") {
		_charGen->showName();
		updateButtons();
		return;
	}
	if (widget.getTag() == "BTN_STEPNAME6") {
		_charGen->start();
		GfxMan.lockFrame();
		_returnCode = 2;
		return;
	}
	// TODO implement the custom character generation
}

void CustomCharPanel::updateButtons() {
	switch (_charGen->getStep()) {
		case 0:
			getButton("BTN_STEPNAME1")->setDisabled(false);
			getButton("BTN_STEPNAME2")->setDisabled(true);
			getButton("BTN_STEPNAME3")->setDisabled(true);
			getButton("BTN_STEPNAME4")->setDisabled(true);
			getButton("BTN_STEPNAME5")->setDisabled(true);
			getButton("BTN_STEPNAME6")->setDisabled(true);

			getButton("BTN_STEPNAME1")->setPermanentHighlight(true);
			getButton("BTN_STEPNAME2")->setPermanentHighlight(false);
			getButton("BTN_STEPNAME3")->setPermanentHighlight(false);
			getButton("BTN_STEPNAME4")->setPermanentHighlight(false);
			getButton("BTN_STEPNAME5")->setPermanentHighlight(false);
			getButton("BTN_STEPNAME6")->setPermanentHighlight(false);

			getButton("BTN_STEPNAME1")->setDisableHighlight(false);
			getButton("BTN_STEPNAME2")->setDisableHighlight(true);
			getButton("BTN_STEPNAME3")->setDisableHighlight(true);
			getButton("BTN_STEPNAME4")->setDisableHighlight(true);
			getButton("BTN_STEPNAME5")->setDisableHighlight(true);
			getButton("BTN_STEPNAME6")->setDisableHighlight(true);
			break;

		case 1:
			getButton("BTN_STEPNAME1")->setDisabled(true);
			getButton("BTN_STEPNAME2")->setDisabled(false);
			getButton("BTN_STEPNAME3")->setDisabled(true);
			getButton("BTN_STEPNAME4")->setDisabled(true);
			getButton("BTN_STEPNAME5")->setDisabled(true);
			getButton("BTN_STEPNAME6")->setDisabled(true);

			getButton("BTN_STEPNAME1")->setPermanentHighlight(false);
			getButton("BTN_STEPNAME2")->setPermanentHighlight(true);
			getButton("BTN_STEPNAME3")->setPermanentHighlight(false);
			getButton("BTN_STEPNAME4")->setPermanentHighlight(false);
			getButton("BTN_STEPNAME5")->setPermanentHighlight(false);
			getButton("BTN_STEPNAME6")->setPermanentHighlight(false);

			getButton("BTN_STEPNAME1")->setDisableHighlight(true);
			getButton("BTN_STEPNAME2")->setDisableHighlight(false);
			getButton("BTN_STEPNAME3")->setDisableHighlight(true);
			getButton("BTN_STEPNAME4")->setDisableHighlight(true);
			getButton("BTN_STEPNAME5")->setDisableHighlight(true);
			getButton("BTN_STEPNAME6")->setDisableHighlight(true);
			break;

		case 2:
			getButton("BTN_STEPNAME1")->setDisabled(true);
			getButton("BTN_STEPNAME2")->setDisabled(true);
			getButton("BTN_STEPNAME3")->setDisabled(false);
			getButton("BTN_STEPNAME4")->setDisabled(true);
			getButton("BTN_STEPNAME5")->setDisabled(true);
			getButton("BTN_STEPNAME6")->setDisabled(true);

			getButton("BTN_STEPNAME1")->setPermanentHighlight(false);
			getButton("BTN_STEPNAME2")->setPermanentHighlight(false);
			getButton("BTN_STEPNAME3")->setPermanentHighlight(true);
			getButton("BTN_STEPNAME4")->setPermanentHighlight(false);
			getButton("BTN_STEPNAME5")->setPermanentHighlight(false);
			getButton("BTN_STEPNAME6")->setPermanentHighlight(false);

			getButton("BTN_STEPNAME1")->setDisableHighlight(true);
			getButton("BTN_STEPNAME2")->setDisableHighlight(true);
			getButton("BTN_STEPNAME3")->setDisableHighlight(false);
			getButton("BTN_STEPNAME4")->setDisableHighlight(true);
			getButton("BTN_STEPNAME5")->setDisableHighlight(true);
			getButton("BTN_STEPNAME6")->setDisableHighlight(true);
			break;

		case 3:
			getButton("BTN_STEPNAME1")->setDisabled(true);
			getButton("BTN_STEPNAME2")->setDisabled(true);
			getButton("BTN_STEPNAME3")->setDisabled(true);
			getButton("BTN_STEPNAME4")->setDisabled(false);
			getButton("BTN_STEPNAME5")->setDisabled(true);
			getButton("BTN_STEPNAME6")->setDisabled(true);

			getButton("BTN_STEPNAME1")->setPermanentHighlight(false);
			getButton("BTN_STEPNAME2")->setPermanentHighlight(false);
			getButton("BTN_STEPNAME3")->setPermanentHighlight(false);
			getButton("BTN_STEPNAME4")->setPermanentHighlight(true);
			getButton("BTN_STEPNAME5")->setPermanentHighlight(false);
			getButton("BTN_STEPNAME6")->setPermanentHighlight(false);

			getButton("BTN_STEPNAME1")->setDisableHighlight(true);
			getButton("BTN_STEPNAME2")->setDisableHighlight(true);
			getButton("BTN_STEPNAME3")->setDisableHighlight(true);
			getButton("BTN_STEPNAME4")->setDisableHighlight(false);
			getButton("BTN_STEPNAME5")->setDisableHighlight(true);
			getButton("BTN_STEPNAME6")->setDisableHighlight(true);
			break;

		case 4:
			getButton("BTN_STEPNAME1")->setDisabled(true);
			getButton("BTN_STEPNAME2")->setDisabled(true);
			getButton("BTN_STEPNAME3")->setDisabled(true);
			getButton("BTN_STEPNAME4")->setDisabled(true);
			getButton("BTN_STEPNAME5")->setDisabled(false);
			getButton("BTN_STEPNAME6")->setDisabled(true);

			getButton("BTN_STEPNAME1")->setPermanentHighlight(false);
			getButton("BTN_STEPNAME2")->setPermanentHighlight(false);
			getButton("BTN_STEPNAME3")->setPermanentHighlight(false);
			getButton("BTN_STEPNAME4")->setPermanentHighlight(false);
			getButton("BTN_STEPNAME5")->setPermanentHighlight(true);
			getButton("BTN_STEPNAME6")->setPermanentHighlight(false);

			getButton("BTN_STEPNAME1")->setDisableHighlight(true);
			getButton("BTN_STEPNAME2")->setDisableHighlight(true);
			getButton("BTN_STEPNAME3")->setDisableHighlight(true);
			getButton("BTN_STEPNAME4")->setDisableHighlight(true);
			getButton("BTN_STEPNAME5")->setDisableHighlight(false);
			getButton("BTN_STEPNAME6")->setDisableHighlight(true);
			break;

		default:
		case 5:
			getButton("BTN_STEPNAME1")->setDisabled(true);
			getButton("BTN_STEPNAME2")->setDisabled(true);
			getButton("BTN_STEPNAME3")->setDisabled(true);
			getButton("BTN_STEPNAME4")->setDisabled(true);
			getButton("BTN_STEPNAME5")->setDisabled(true);
			getButton("BTN_STEPNAME6")->setDisabled(false);

			getButton("BTN_STEPNAME1")->setPermanentHighlight(false);
			getButton("BTN_STEPNAME2")->setPermanentHighlight(false);
			getButton("BTN_STEPNAME3")->setPermanentHighlight(false);
			getButton("BTN_STEPNAME4")->setPermanentHighlight(false);
			getButton("BTN_STEPNAME5")->setPermanentHighlight(false);
			getButton("BTN_STEPNAME6")->setPermanentHighlight(true);

			getButton("BTN_STEPNAME1")->setDisableHighlight(true);
			getButton("BTN_STEPNAME2")->setDisableHighlight(true);
			getButton("BTN_STEPNAME3")->setDisableHighlight(true);
			getButton("BTN_STEPNAME4")->setDisableHighlight(true);
			getButton("BTN_STEPNAME5")->setDisableHighlight(true);
			getButton("BTN_STEPNAME6")->setDisableHighlight(false);
			break;
	}

	if (_charGen->getStep() == 0) {
		getWidget("BTN_BACK")->setDisabled(true);
	} else {
		getWidget("BTN_BACK")->setDisabled(false);
	}
}

} // End of namespace KotOR

} // End of namespace Engines
