/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/main/main.cpp
 *  The main menu.
 */

#include "common/configman.h"

#include "events/events.h"

#include "engines/nwn/module.h"

#include "engines/nwn/gui/widgets/panel.h"

#include "engines/nwn/gui/main/main.h"
#include "engines/nwn/gui/main/newgamefog.h"
#include "engines/nwn/gui/main/new.h"
#include "engines/nwn/gui/main/newcamp.h"
#include "engines/nwn/gui/main/moviesbase.h"
#include "engines/nwn/gui/main/moviescamp.h"
#include "engines/nwn/gui/main/options.h"
#include "engines/nwn/gui/main/chartype.h"

namespace Engines {

namespace NWN {

MainMenu::MainMenu(Module &module) : _module(&module) {
	load("pre_main");

	bool hasXP1 = ConfigMan.getBool("NWN_hasXP1");
	bool hasXP2 = ConfigMan.getBool("NWN_hasXP2");

	_hasXP = hasXP1 || hasXP2;

	if (hasXP1) {
		WidgetPanel *xp1 = new WidgetPanel(*this, "TextXP1", "ctl_xp1_text");
		xp1->setPosition(124.0, 0.00, 50.0);
		addWidget(xp1);
	}

	if (hasXP2) {
		WidgetPanel *xp2 = new WidgetPanel(*this, "TextXP2", "ctl_xp2_text");
		xp2->setPosition(124.0, -147.0, 50.0);
		addWidget(xp2);
	}

	getWidget("LoadButton" , true)->setDisabled(true);
	getWidget("MultiButton", true)->setDisabled(true);

	_charType = new CharTypeMenu(*_module);

	if (_hasXP)
		// If we have at least an expansion, create the campaign selection game menu
		_new = new NewCampMenu(*_module, *_charType);
	else
		// If not, create the base game menu
		_new = new NewMenu(*_module, *_charType);

	if (_hasXP)
		// If we have at least an expansion, create the campaign selection movies menu
		_movies = new MoviesCampMenu;
	else
		// If not, create the base game movies menu
		_movies = new MoviesBaseMenu;

	_options = new OptionsMenu();
}

MainMenu::~MainMenu() {
	delete _options;
	delete _movies;
	delete _new;

	delete _charType;
}

void MainMenu::show() {
	GUI::show();

	getWidget("NewButton#Caption", true)->show();
}

void MainMenu::callbackActive(Widget &widget) {
	if (widget.getTag() == "ExitButton") {
		EventMan.requestQuit();
		return;
	}

	if (widget.getTag() == "NewButton") {
		NewGameFogs fogs(4);
		fogs.show();

		if (sub(*_new, 0, false) == 2) {
			_returnCode = 2;
			return;
		}

		show();
		return;
	}

	if (widget.getTag() == "MoviesButton") {
		sub(*_movies);
		return;
	}

	if (widget.getTag() == "OptionsButton") {
		sub(*_options);
		return;
	}

}

} // End of namespace NWN

} // End of namespace Engines
