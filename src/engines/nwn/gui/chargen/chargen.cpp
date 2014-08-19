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

/** @file engines/nwn/gui/chargen/chargen.cpp
 *  The new character generator.
 */

#include "engines/aurora/widget.h"

#include "engines/nwn/module.h"

#include "engines/nwn/gui/chargen/chargen.h"

namespace Engines {

namespace NWN {

CharGenMenu::CharGenMenu(Module &module) : _module(&module) {
	load("cg_main");

	// TODO: "TitleLabel" misplaced!

	// TODO: Character trait buttons
	getWidget("GenderButton"   , true)->setDisabled(true);
	getWidget("RaceButton"     , true)->setDisabled(true);
	getWidget("PortraitButton" , true)->setDisabled(true);
	getWidget("ClassButton"    , true)->setDisabled(true);
	getWidget("AlignButton"    , true)->setDisabled(true);
	getWidget("AbilitiesButton", true)->setDisabled(true);
	getWidget("PackagesButton" , true)->setDisabled(true);
	getWidget("CustomizeButton", true)->setDisabled(true);

	// TODO: Reset
	getWidget("ResetButton", true)->setDisabled(true);

	// TODO: Play
	getWidget("PlayButton" , true)->setDisabled(true);
}

CharGenMenu::~CharGenMenu() {
}

void CharGenMenu::callbackActive(Widget &widget) {
	if (widget.getTag() == "CancelButton") {
		_returnCode = 1;
		return;
	}

}

} // End of namespace NWN

} // End of namespace Engines
