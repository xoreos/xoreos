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

#include "src/engines/kotor/gui/chargen/quickchar.h"

namespace Engines {

namespace KotOR {

QuickCharPanel::QuickCharPanel(CharacterGenerationMenu *charGenMenu, Console *console) :
		GUI(console),
		_charGen(charGenMenu) {

	load("quickpnl");

	setPosition(137, 15, 0);

	float width, height;
	width = getLabel("LBL_DECORATION")->getWidth();
	height = getLabel("LBL_DECORATION")->getHeight();
	getLabel("LBL_DECORATION")->setScissor(5, 40, width-5, height-40-40);
}

void QuickCharPanel::callbackActive(Widget &widget) {
	if (widget.getTag() == "BTN_CANCEL") {
		_charGen->showQuickOrCustom();
		return;
	}
}

} // End of namespace KotOR

} // End of namespace Engines
