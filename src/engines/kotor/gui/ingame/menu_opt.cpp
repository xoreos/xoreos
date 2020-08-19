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
 *  The ingame options menu.
 */

#include "menu_opt.h"

#include "src/engines/aurora/widget.h"

#include "src/engines/kotor/gui/saveload.h"

namespace Engines {

namespace KotOR {

MenuOptions::MenuOptions(Console *console) :
		KotORBase::GUI(console),
		_selectedItem(0) {

	load("optionsingame");
}

uint8_t MenuOptions::pollSelectedItem() {
	uint8_t result = _selectedItem;
	_selectedItem = 0;
	return result;
}

void MenuOptions::callbackActive(Widget &widget) {
	if (widget.getTag() == "BTN_LOADGAME") {
		_selectedItem = kOptionsItemLoadGame;
		return;
	}

	if (widget.getTag() == "BTN_SAVEGAME") {
		_selectedItem = kOptionsItemSaveGame;
		return;
	}

	if (widget.getTag() == "BTN_QUIT") {
		_selectedItem = kOptionsItemExitGame;
		return;
	}

	if (widget.getTag() == "BTN_EXIT") {
		_returnCode = 1;
		return;
	}
}

} // End of namespace KotOR

} // End of namespace Engines
