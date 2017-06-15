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

#include "src/engines/kotor/gui/widgets/label.h"

#include "src/engines/kotor/gui/chargen/customchar.h"

namespace Engines {

namespace KotOR {

CustomCharPanel::CustomCharPanel(CharacterGenerationMenu *charGenMenu, Console *console) :
		GUI(console), _charGen(charGenMenu) {

	load("custpnl");

	setPosition(137, 16, 0);

	float width = getLabel("LBL_BG")->getWidth();
	float height = getLabel("LBL_BG")->getHeight();

	getLabel("LBL_BG")->setScissor(5, 40, width - 5, height - 60);
}

void CustomCharPanel::callbackActive(Widget &widget) {
	if (widget.getTag() == "BTN_CANCEL") {
		_charGen->showQuickOrCustom();
		return;
	}

	if (widget.getTag() == "BTN_BACK") {
		_charGen->decStep();
		return;
	}

	// TODO implement the custom character generation
}

} // End of namespace KotOR

} // End of namespace Engines
