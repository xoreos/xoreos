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
 *  The KotOR 2 custom character panel.
 */

#include "src/engines/aurora/widget.h"

#include "src/engines/kotor2/gui/chargen/customchar.h"

namespace Engines {

namespace KotOR2 {

CustomCharPanel::CustomCharPanel(CharacterGeneration *chargen, Console *console) :
		GUI(console), _chargenMenu(chargen) {
	load("custpnl_p");
}

void CustomCharPanel::callbackActive(Widget &widget) {
	if (widget.getTag() == "BTN_CANCEL") {
		_chargenMenu->resetStep();
		_chargenMenu->showQuickOrCustom();
		return;
	}
}

} // End of namespace KotOR2

} // End of namespace Engines
