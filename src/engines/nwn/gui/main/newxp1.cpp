/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/main/newxp1.cpp
 *  The new game menu, expansion 1.
 */

#include "aurora/talkman.h"

#include "engines/nwn/module.h"

#include "engines/nwn/gui/widgets/label.h"

#include "engines/nwn/gui/main/newxp1.h"

namespace Engines {

namespace NWN {

NewXP1Menu::NewXP1Menu(Module &module, GUI &charType) : _module(&module), _charType(&charType) {
	load("pre_newgame");

	getWidget("OtherButton"   , true)->setInvisible(true);
	getWidget("PreludeButton" , true)->setInvisible(true);
	getWidget("Chapter4Button", true)->setInvisible(true);
}

NewXP1Menu::~NewXP1Menu() {
}

void NewXP1Menu::initWidget(Widget &widget) {
	if (widget.getTag() == "Chapter1Button#Caption") {
		dynamic_cast<WidgetLabel &>(widget).setText(TalkMan.getString(40041));
		return;
	}

	if (widget.getTag() == "Chapter2Button#Caption") {
		dynamic_cast<WidgetLabel &>(widget).setText(TalkMan.getString(40042));
		return;
	}

	if (widget.getTag() == "Chapter3Button#Caption") {
		dynamic_cast<WidgetLabel &>(widget).setText(TalkMan.getString(40043));
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
