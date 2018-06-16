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
 *  The KotOR 2 quick character panel.
 */

#include "src/engines/aurora/kotorjadegui/button.h"

#include "src/engines/kotor2/gui/chargen/quickchar.h"

namespace Engines {

namespace KotOR2 {

QuickCharPanel::QuickCharPanel(CharacterGeneration *chargen, Console *console) : GUI(console), _chargenMenu(chargen) {
	load("quickpnl_p");

	getButton("BTN_STEPNAME1")->setDisableHoverSound(true);
	getButton("BTN_STEPNAME2")->setDisableHoverSound(true);
	getButton("BTN_STEPNAME3")->setDisableHoverSound(true);

	updateButtons();
}

void QuickCharPanel::updateButtons() {
	switch (_chargenMenu->getStep()) {
		case 0:
			getButton("BTN_STEPNAME1")->setDisabled(false);
			getButton("BTN_STEPNAME2")->setDisabled(true);
			getButton("BTN_STEPNAME3")->setDisabled(true);

			getButton("BTN_STEPNAME1")->setPermanentHighlight(true);
			getButton("BTN_STEPNAME2")->setPermanentHighlight(false);
			getButton("BTN_STEPNAME3")->setPermanentHighlight(false);

			getButton("BTN_STEPNAME1")->setDisableHighlight(false);
			getButton("BTN_STEPNAME2")->setDisableHighlight(true);
			getButton("BTN_STEPNAME3")->setDisableHighlight(true);
			break;

		case 1:
			getButton("BTN_STEPNAME1")->setDisabled(true);
			getButton("BTN_STEPNAME2")->setDisabled(false);
			getButton("BTN_STEPNAME3")->setDisabled(true);

			getButton("BTN_STEPNAME1")->setPermanentHighlight(false);
			getButton("BTN_STEPNAME2")->setPermanentHighlight(true);
			getButton("BTN_STEPNAME3")->setPermanentHighlight(false);

			getButton("BTN_STEPNAME1")->setDisableHighlight(true);
			getButton("BTN_STEPNAME2")->setDisableHighlight(false);
			getButton("BTN_STEPNAME3")->setDisableHighlight(true);
			break;
		default:
		case 2:
			getButton("BTN_STEPNAME1")->setDisabled(true);
			getButton("BTN_STEPNAME2")->setDisabled(true);
			getButton("BTN_STEPNAME3")->setDisabled(false);

			getButton("BTN_STEPNAME1")->setPermanentHighlight(false);
			getButton("BTN_STEPNAME2")->setPermanentHighlight(false);
			getButton("BTN_STEPNAME3")->setPermanentHighlight(true);

			getButton("BTN_STEPNAME1")->setDisableHighlight(true);
			getButton("BTN_STEPNAME2")->setDisableHighlight(true);
			getButton("BTN_STEPNAME3")->setDisableHighlight(false);
			break;
	}

	if (_chargenMenu->getStep() == 0) {
		getWidget("BTN_BACK")->setDisabled(true);
	} else {
		getWidget("BTN_BACK")->setDisabled(false);
	}
}

void QuickCharPanel::callbackActive(Widget &widget) {
	if (widget.getTag() == "BTN_CANCEL") {
		_chargenMenu->resetStep();
		_chargenMenu->showQuickOrCustom();
		return;
	}
	if (widget.getTag() == "BTN_BACK") {
		_chargenMenu->decStep();
		updateButtons();
		return;
	}

	if (widget.getTag() == "BTN_STEPNAME1") {
		_chargenMenu->showPortrait();
		updateButtons();
		return;
	}
	if (widget.getTag() == "BTN_STEPNAME2") {
		_chargenMenu->showName();
		updateButtons();
		return;
	}
	if (widget.getTag() == "BTN_STEPNAME3") {
		_chargenMenu->start();
		_returnCode = 2;
		return;
	}
}

} // End of namespace KotOR2

} // End of namespace Engines
