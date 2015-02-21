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
 *  The new character generator.
 */

#include "src/engines/aurora/widget.h"

#include "src/engines/nwn/module.h"
#include "src/engines/nwn/gui/widgets/button.h"

#include "src/engines/nwn/gui/chargen/charsex.h"
#include "src/engines/nwn/gui/chargen/charrace.h"
#include "src/engines/nwn/gui/chargen/charportrait.h"

#include "src/engines/nwn/gui/chargen/chargen.h"

namespace Engines {

namespace NWN {

CharGenMenu::CharGenMenu(Module &module) : _module(&module) {
	load("cg_main");

	// TODO: Character trait buttons
	getWidget("ClassButton"    , true)->setDisabled(true);
	getWidget("AlignButton"    , true)->setDisabled(true);
	getWidget("AbilitiesButton", true)->setDisabled(true);
	getWidget("PackagesButton" , true)->setDisabled(true);
	getWidget("CustomizeButton", true)->setDisabled(true);

	// TODO: Play
	getWidget("PlayButton" , true)->setDisabled(true);

	init();
}

CharGenMenu::~CharGenMenu() {
	for (uint it = 0; it < _chargenGuis.size(); ++it)
		delete _chargenGuis[it];

	delete _charChoices.character;
	_charChoices.character = 0;
}

void CharGenMenu::reset() {
	delete _charChoices.character;
	_charChoices.character = new Creature();

	for (std::vector<CharGenBase *>::iterator g = _chargenGuis.begin(); g != _chargenGuis.end(); ++g)
		(*g)->reset();

	for (std::vector<WidgetButton *>::iterator b = ++_charButtons.begin(); b != _charButtons.end(); ++b)
		(*b)->setDisabled(true);
}

void CharGenMenu::callbackActive(Widget &widget) {
	for (uint it = 0; it < _chargenGuis.size(); ++it) {
		if (widget.getTag() == _charButtons[it]->getTag()) {
			if (sub(*_chargenGuis[it]) == 2) {
				if (it == _chargenGuis.size() - 1)
					return;
				_charButtons[it + 1]->setDisabled(false);
				_chargenGuis[it + 1]->reset();
				for (uint next = it + 2; next < _charButtons.size(); ++next) {
					_charButtons[next]->setDisabled(true);
					_chargenGuis[next]->reset();
				}
				return;
			}
		}
	}

	if (widget.getTag() == "CancelButton") {
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "ResetButton") {
		reset();
		return;
	}
}

void CharGenMenu::init() {
	delete _charChoices.character;
	_charChoices.character = new Creature();

	_charButtons.push_back(getButton("GenderButton", true));
	_charButtons.push_back(getButton("RaceButton", true));
	_charButtons.push_back(getButton("PortraitButton", true));

	for (std::vector<WidgetButton *>::iterator b = ++_charButtons.begin(); b != _charButtons.end(); ++b)
		(*b)->setDisabled(true);

	_chargenGuis.push_back(new CharSex());
	_chargenGuis.push_back(new CharRace());
	_chargenGuis.push_back(new CharPortrait());
}

} // End of namespace NWN

} // End of namespace Engines
