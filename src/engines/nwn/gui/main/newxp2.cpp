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
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file engines/nwn/gui/main/newxp2.cpp
 *  The new game menu, expansion 2.
 */

#include "aurora/talkman.h"

#include "engines/nwn/module.h"

#include "engines/nwn/gui/widgets/label.h"

#include "engines/nwn/gui/main/newxp2.h"

namespace Engines {

namespace NWN {

NewXP2Menu::NewXP2Menu(Module &module, GUI &charType) : _module(&module), _charType(&charType) {
	setPosition(0.0, 0.0, -300.0);
	load("pre_newgame");

	getWidget("OtherButton"   , true)->setInvisible(true);
	getWidget("PreludeButton" , true)->setInvisible(true);
	getWidget("Chapter4Button", true)->setInvisible(true);
}

NewXP2Menu::~NewXP2Menu() {
}

void NewXP2Menu::initWidget(Widget &widget) {
	if (widget.getTag() == "Chapter1Button#Caption") {
		dynamic_cast<WidgetLabel &>(widget).setText(TalkMan.getString(100777));
		return;
	}

	if (widget.getTag() == "Chapter2Button#Caption") {
		dynamic_cast<WidgetLabel &>(widget).setText(TalkMan.getString(100778));
		return;
	}

	if (widget.getTag() == "Chapter3Button#Caption") {
		dynamic_cast<WidgetLabel &>(widget).setText(TalkMan.getString(100779));
		return;
	}
}

void NewXP2Menu::callbackActive(Widget &widget) {
	if (widget.getTag() == "CancelButton") {
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "Chapter1Button") {
		loadModule("xp2_chapter1.nwm");
		return;
	}

	if (widget.getTag() == "Chapter2Button") {
		loadModule("xp2_chapter2.nwm");
		return;
	}

	if (widget.getTag() == "Chapter3Button") {
		loadModule("xp2_chapter3.nwm");
		return;
	}

}

void NewXP2Menu::loadModule(const Common::UString &module) {
	if (_module->loadModule(module)) {
		if (sub(*_charType, 0, false) == 2) {
			_returnCode = 2;
			return;
		}

		setVisible(true);
	}
}

} // End of namespace NWN

} // End of namespace Engines
