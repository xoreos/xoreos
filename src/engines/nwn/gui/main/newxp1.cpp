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
 *  The new game menu, expansion 1.
 */

#include "src/aurora/talkman.h"

#include "src/engines/nwn/module.h"

#include "src/engines/nwn/gui/widgets/button.h"

#include "src/engines/nwn/gui/main/newxp1.h"

namespace Engines {

namespace NWN {

NewXP1Menu::NewXP1Menu(Module &module, GUI &charType, ::Engines::Console *console) : GUI(console),
	_module(&module), _charType(&charType) {

	load("pre_newgame");

	getWidget("OtherButton"   , true)->setInvisible(true);
	getWidget("PreludeButton" , true)->setInvisible(true);
	getWidget("Chapter4Button", true)->setInvisible(true);
}

NewXP1Menu::~NewXP1Menu() {
}

void NewXP1Menu::initWidget(Widget &widget) {
	if (widget.getTag() == "Chapter1Button") {
		dynamic_cast<WidgetButton &>(widget).setText(TalkMan.getString(40041));
		return;
	}

	if (widget.getTag() == "Chapter2Button") {
		dynamic_cast<WidgetButton &>(widget).setText(TalkMan.getString(40042));
		return;
	}

	if (widget.getTag() == "Chapter3Button") {
		dynamic_cast<WidgetButton &>(widget).setText(TalkMan.getString(40043));
		return;
	}
}

void NewXP1Menu::callbackActive(Widget &widget) {
	if (widget.getTag() == "CancelButton") {
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "Chapter1Button") {
		loadModule("xp1-chapter 1.nwm");
		return;
	}

	if (widget.getTag() == "Chapter2Button") {
		loadModule("xp1-interlude.nwm");
		return;
	}

	if (widget.getTag() == "Chapter3Button") {
		loadModule("xp1-chapter 2.nwm");
		return;
	}

}

void NewXP1Menu::loadModule(const Common::UString &module) {
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
