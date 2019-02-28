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
 *  The KotOR 2 quick or custom panel.
 */

#include "src/engines/aurora/widget.h"

#include "src/engines/kotor2/gui/chargen/quickorcustom.h"

namespace Engines {

namespace KotOR2 {

QuickOrCustomPanel::QuickOrCustomPanel(CharacterGeneration *chargenMenu, Console *console) :
		KotORBase::GUI(console),
		_chargenMenu(chargenMenu) {

	load("qorcpnl_p");
}

void QuickOrCustomPanel::callbackActive(Widget &widget) {
	if (widget.getTag() == "BTN_BACK") {
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "QUICK_CHAR_BTN") {
		_chargenMenu->showQuickChar();
		return;
	}
	if (widget.getTag() == "CUST_CHAR_BTN") {
		_chargenMenu->showCustomChar();
	}
}

} // End of namespace KotOR2

} // End of namespace Engines
