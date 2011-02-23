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

#include "events/events.h"

#include "graphics/aurora/model.h"

#include "engines/nwn/menu/main.h"
#include "engines/nwn/menu/new.h"
#include "engines/nwn/menu/newcamp.h"
#include "engines/nwn/menu/moviesbase.h"
#include "engines/nwn/menu/moviescamp.h"
#include "engines/nwn/menu/options.h"

#include "engines/aurora/model.h"

namespace Engines {

namespace NWN {

MainMenu::MainMenu(bool xp1, bool xp2, bool xp3) : _xp1(0), _xp2(0) {
	load("pre_main");

	if (xp1) {
		_xp1 = loadModelGUI("ctl_xp1_text");
		_xp1->setPosition(1.24, 0.00, 0.50);
	}

	if (xp2) {
		_xp2 = loadModelGUI("ctl_xp2_text");
		_xp2->setPosition(1.24, -1.47, 0.50);
	}

	getWidget("LoadButton" , true)->setDisabled(true);
	getWidget("MultiButton", true)->setDisabled(true);

	if (xp1 || xp2)
		// If we have at least an expansion, create the campaign selection game menu
		_new = new NewCampMenu(xp1, xp2, xp3);
	else
		// If not, create the base game menu
		_new = new NewMenu(xp1, xp2, xp3);

	if (xp1 || xp2)
		// If we have at least an expansion, create the campaign selection movies menu
		_movies = new MoviesCampMenu(xp1, xp2, xp3);
	else
		// If not, create the base game movies menu
		_movies = new MoviesBaseMenu;

	_options = new OptionsMenu;
}

MainMenu::~MainMenu() {
	delete _new;
	delete _options;
	delete _movies;

	delete _xp2;
	delete _xp1;
}

void MainMenu::show() {
	GUI::show();

	if (_xp1)
		_xp1->show();
	if (_xp2)
		_xp2->show();
}

void MainMenu::hide() {
	GUI::hide();

	if (_xp1)
		_xp1->hide();
	if (_xp2)
		_xp2->hide();
}

void MainMenu::callbackActive(Widget &widget) {
	if (widget.getTag() == "ExitButton") {
		EventMan.requestQuit();
		return;
	}

	if (widget.getTag() == "NewButton") {
		sub(*_new);
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
