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
	 *  The new campaign menu.
	 */

#include "src/common/configman.h"

#include "src/engines/nwn/game.h"

#include "src/engines/nwn/gui/main/newcamp.h"
#include "src/engines/nwn/gui/main/new.h"
#include "src/engines/nwn/gui/main/newxp1.h"
#include "src/engines/nwn/gui/main/newxp2.h"
#include "src/engines/nwn/gui/main/newmodule.h"
#include "src/engines/nwn/gui/main/newpremium.h"

namespace Engines {

namespace NWN {

NewCampMenu::NewCampMenu(Module &module, GUI &charType, ::Engines::Console *console) : GUI(console),
	_module(&module), _charType(&charType) {

	load("pre_campaign");

	Widget *button = 0;

	button = getWidget("NWNXP1Button");
	if (button)
		button->setDisabled(!ConfigMan.getBool("NWN_hasXP1"));
	button = getWidget("NWNXP2Button");
	if (button)
		button->setDisabled(!ConfigMan.getBool("NWN_hasXP2"));
	button = getWidget("NWNXP3Button");
	if (button)
		button->setDisabled(!Game::hasPremiumModules());

	_base.reset   (new NewMenu       (*_module, *_charType, _console));
	_xp1.reset    (new NewXP1Menu    (*_module, *_charType, _console));
	_xp2.reset    (new NewXP2Menu    (*_module, *_charType, _console));
	_modules.reset(new NewModuleMenu (*_module, *_charType, _console));
	_premium.reset(new NewPremiumMenu(*_module, *_charType, _console));
}

NewCampMenu::~NewCampMenu() {
}

void NewCampMenu::callbackActive(Widget &widget) {
	if (widget.getTag() == "CancelButton") {
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "NWNButton") {
		if (sub(*_base, 0, false) == 2) {
			_returnCode = 2;
			return;
		}

		show();
		return;
	}

	if (widget.getTag() == "NWNXP1Button") {
		if (sub(*_xp1, 0, false) == 2) {
			_returnCode = 2;
			return;
		}

		show();
		return;
	}

	if (widget.getTag() == "NWNXP2Button") {
		if (sub(*_xp2, 0, false) == 2) {
			_returnCode = 2;
			return;
		}

		show();
		return;
	}

	if (widget.getTag() == "NWNXP3Button") {
		if (sub(*_premium, 0, false) == 2) {
			_returnCode = 2;
			return;
		}

		show();
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

}

} // End of namespace NWN

} // End of namespace Engines
