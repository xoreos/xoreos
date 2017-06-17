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
 *  The panel to customize a custom character.
 */

#include "src/engines/kotor/gui/widgets/label.h"
#include "src/engines/kotor/gui/widgets/button.h"

#include "src/engines/kotor/gui/chargen/quickchar.h"

namespace Engines {

namespace KotOR {

QuickCharPanel::QuickCharPanel(CharacterGenerationMenu *charGenMenu, Console *console) :
		GUI(console), _charGen(charGenMenu) {

	load("quickpnl");

	setPosition(137, 15, 0);

	float width = getLabel("LBL_DECORATION")->getWidth();
	float height = getLabel("LBL_DECORATION")->getHeight();
	getLabel("LBL_DECORATION")->setScissor(5, 40, width - 5, height - 40 - 40);

	getButton("BTN_STEPNAME1")->setDisableHoverSound(true);
	getButton("BTN_STEPNAME2")->setDisableHoverSound(true);
	getButton("BTN_STEPNAME3")->setDisableHoverSound(true);

	updateButtons();
}

void QuickCharPanel::callbackActive(Widget &widget) {
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
	if (widget.getTag() == "BTN_STEPNAME2") {
		_charGen->showName();
		updateButtons();
		return;
	}
	if (widget.getTag() == "BTN_STEPNAME3") {
		_charGen->start();
		_returnCode = 2;
		return;
	}
}

void QuickCharPanel::updateButtons() {
	switch (_charGen->getStep()) {
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

	if (_charGen->getStep() == 0) {
		getWidget("BTN_BACK")->setDisabled(true);
	} else {
		getWidget("BTN_BACK")->setDisabled(false);
	}
}

} // End of namespace KotOR

} // End of namespace Engines
