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
 *  The new game menu.
 */

#include "src/common/configman.h"

#include "src/engines/nwn/module.h"

#include "src/engines/nwn/gui/main/new.h"
#include "src/engines/nwn/gui/main/newmodule.h"

namespace Engines {

namespace NWN {

NewMenu::NewMenu(Module &module, GUI &charType, ::Engines::Console *console) : GUI(console),
	_module(&module), _charType(&charType) {

	load("pre_newgame");

	_hasXP = ConfigMan.getBool("NWN_hasXP1") || ConfigMan.getBool("NWN_hasXP2");

	if (_hasXP)
		// If we have an expansion, hide the "Other modules" button, it's
		// already in the campaign menu
		getWidget("OtherButton", true)->setInvisible(true);

	if (!_hasXP)
		_modules.reset(new NewModuleMenu(*_module, *_charType, _console));
}

NewMenu::~NewMenu() {
}

void NewMenu::callbackActive(Widget &widget) {
	if (widget.getTag() == "CancelButton") {
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "OtherButton") {
		if (sub(*_modules, 0, false) == 2) {
			_returnCode = 2;
			return;
		}

		show();
		return;
	}

	if (widget.getTag() == "PreludeButton") {
		loadModule("prelude.nwm");
		return;
	}

	if (widget.getTag() == "Chapter1Button") {
		loadModule("chapter1.nwm");
		return;
	}

	if (widget.getTag() == "Chapter2Button") {
		loadModule("chapter2.nwm");
		return;
	}

	if (widget.getTag() == "Chapter3Button") {
		loadModule("chapter3.nwm");
		return;
	}

	if (widget.getTag() == "Chapter4Button") {
		loadModule("chapter4.nwm");
		return;
	}
}

void NewMenu::loadModule(const Common::UString &module) {
	try {
		_module->load(module);
	} catch (...) {
		Common::exceptionDispatcherWarning();
		return;
	}

	if (sub(*_charType, 0, false) == 2) {
		_returnCode = 2;
		return;
	}

	show();
}

} // End of namespace NWN

} // End of namespace Engines
