/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
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

		show();
	}
}

} // End of namespace NWN

} // End of namespace Engines
