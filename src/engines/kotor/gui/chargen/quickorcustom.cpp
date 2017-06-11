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
 *  The panel to choose between quick or custom characters.
 */

#include "src/engines/kotor/gui/widgets/label.h"

#include "src/engines/kotor/gui/chargen/quickorcustom.h"
#include "src/engines/kotor/gui/chargen/charactergeneration.h"

namespace Engines {

namespace KotOR {

QuickOrCustomPanel::QuickOrCustomPanel(CharacterGenerationMenu *charGenMenu, Console *console) :
		GUI(console), _charGenMenu(charGenMenu) {

	load("qorcpnl");

	setPosition(139, 13, 0);

	float width, height;
	width = getLabel("LBL_RBG")->getWidth();
	height = getLabel("LBL_RBG")->getHeight();

	getLabel("LBL_RBG")->setScissor(5, 0, width - 5, height);

	width = getLabel("LBL_DECORATION")->getWidth();
	height = getLabel("LBL_DECORATION")->getHeight();

	getLabel("LBL_DECORATION")->setScissor(5, 0, width - 5, height);
}

QuickOrCustomPanel::~QuickOrCustomPanel() {
}

void QuickOrCustomPanel::callbackActive(Widget &widget) {
	if (widget.getTag() == "BTN_BACK") {
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "QUICK_CHAR_BTN") {
		_charGenMenu->showQuick();
		return;
	}
	if (widget.getTag() == "CUST_CHAR_BTN") {
		_charGenMenu->showCustom();
		return;
	}
}

} // End of namespace KotOR

} // End of namespace Engines
