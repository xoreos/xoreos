/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/main.cpp
 *  The main menu.
 */

#include "common/configman.h"

#include "events/events.h"

#include "graphics/aurora/model.h"

#include "engines/nwn/menu/main.h"
#include "engines/nwn/menu/newgamefog.h"
#include "engines/nwn/menu/new.h"
#include "engines/nwn/menu/newcamp.h"
#include "engines/nwn/menu/moviesbase.h"
#include "engines/nwn/menu/moviescamp.h"
#include "engines/nwn/menu/options.h"

#include "engines/aurora/model.h"
#include "engines/aurora/util.h"

namespace Engines {

namespace NWN {

MainMenu::MainMenu() {
	load("pre_main");

	bool hasXP1 = ConfigMan.getBool("NWN_hasXP1");
	bool hasXP2 = ConfigMan.getBool("NWN_hasXP2");

	if (hasXP1) {
		WidgetPanel *xp1 = new WidgetPanel("TextXP1", "ctl_xp1_text");
		xp1->setPosition(124.0, 0.00, 50.0);
		addWidget(xp1);
	}

	if (hasXP2) {
		WidgetPanel *xp2 = new WidgetPanel("TextXP2", "ctl_xp2_text");
		xp2->setPosition(124.0, -147.0, 50.0);
		addWidget(xp2);
	}

	getWidget("LoadButton" , true)->setDisabled(true);
	getWidget("MultiButton", true)->setDisabled(true);

	if (hasXP1 || hasXP2)
		// If we have at least an expansion, create the campaign selection game menu
		_new = new NewCampMenu;
	else
		// If not, create the base game menu
		_new = new NewMenu;

	if (hasXP1 || hasXP2)
		// If we have at least an expansion, create the campaign selection movies menu
		_movies = new MoviesCampMenu;
	else
		// If not, create the base game movies menu
		_movies = new MoviesBaseMenu;

	_options = new OptionsMenu(true);
}

MainMenu::~MainMenu() {
	delete _new;
	delete _options;
	delete _movies;
}

void MainMenu::callbackRun() {
	int startCode = _startCode;
	_startCode = 0;

	if ((startCode == 2) || (startCode == 3))
		callNew(startCode);
}

void MainMenu::callbackActive(Widget &widget) {
	if (widget.getTag() == "ExitButton") {
		EventMan.requestQuit();
		return;
	}

	if (widget.getTag() == "NewButton") {
		callNew();
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

void MainMenu::callNew(int startCode) {
	std::vector<Graphics::Aurora::Model *> fogs;
	fogs.resize(4);

	for (uint i = 0; i < fogs.size(); i++) {
		fogs[i] = createNewGameFog();
		fogs[i]->show();
	}

	int code = sub(*_new, startCode);
	if ((code == 2) || (code == 3))
		_returnCode = code;

	for (uint i = 0; i < fogs.size(); i++) {
		fogs[i]->hide();
		delete fogs[i];
	}
}

} // End of namespace NWN

} // End of namespace Engines
