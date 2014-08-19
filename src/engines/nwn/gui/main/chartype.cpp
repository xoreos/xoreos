/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

/** @file engines/nwn/gui/main/chartype.cpp
 *  The new/premade character selection menu.
 */

#include "engines/nwn/gui/chargen/chargen.h"

#include "engines/nwn/gui/main/chartype.h"
#include "engines/nwn/gui/main/charpremade.h"

namespace Engines {

namespace NWN {

CharTypeMenu::CharTypeMenu(Module &module) : _module(&module) {
	load("pre_chartype");

	_charGen     = new CharGenMenu(*_module);
	_charPremade = new CharPremadeMenu(*_module);
}

CharTypeMenu::~CharTypeMenu() {
	delete _charPremade;
	delete _charGen;
}

void CharTypeMenu::callbackActive(Widget &widget) {
	if (widget.getTag() == "CancelButton") {
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "CreateNewButton") {
		sub(*_charGen);
		return;
	}

	if (widget.getTag() == "UsePremadeButton") {
		if (sub(*_charPremade, 0, false) == 2) {
			_returnCode = 2;
			return;
		}

		show();
		return;
	}

}

} // End of namespace NWN

} // End of namespace Engines
