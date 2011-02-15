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

#include "engines/nwn/menu/main.h"
#include "engines/nwn/menu/movies.h"

#include "events/events.h"

#include "graphics/aurora/model.h"

namespace Engines {

namespace NWN {

static const uint kButtonNew     = 1;
static const uint kButtonLoad    = 2;
static const uint kButtonMulti   = 3;
static const uint kButtonMovies  = 4;
static const uint kButtonOptions = 5;
static const uint kButtonExit    = 6;
static const uint kButtonMAX     = 7;

static const char *kButtonTags[] = {
	"NONE"        , "NewButton"    , "LoadButton", "MultiButton",
	"MoviesButton", "OptionsButton", "ExitButton"
};

MainMenu::MainMenu(const ModelLoader &modelLoader, bool xp1, bool xp2, bool xp3) :
	Menu(modelLoader, "pre_main"), _xp1(0), _xp2(0) {

	if (xp1) {
		_xp1 = modelLoader.loadGUI("ctl_xp1_text");
		_xp1->setPosition(1.24, 0.00, 0.50);
	}

	if (xp2) {
		_xp2 = modelLoader.loadGUI("ctl_xp2_text");
		_xp2->setPosition(1.24, -1.47, 0.50);
	}

	for (uint i = 1; i < kButtonMAX; i++)
		addButton(kButtonTags[i], i);

	disableButton(kButtonNew);
	disableButton(kButtonLoad);
	disableButton(kButtonMulti);
	disableButton(kButtonOptions);

	_movies = new MoviesMenu(modelLoader, xp1, xp2, xp3);
}

MainMenu::~MainMenu() {
	delete _movies;

	ModelLoader::free(_xp1);
	ModelLoader::free(_xp2);
}

void MainMenu::showModels() {
	if (_xp1)
		_xp1->show();
	if (_xp2)
		_xp2->show();
}

void MainMenu::hideModels() {
	if (_xp1)
		_xp1->hide();
	if (_xp2)
		_xp2->hide();
}

void MainMenu::show() {
	Menu::show();

	showModels();
}

void MainMenu::hide() {
	Menu::hide();

	hideModels();
}

void MainMenu::mouseUp(Events::Event &event) {
	Menu::mouseUp(event);

	if        (_currentButton == kButtonNew) {
	} else if (_currentButton == kButtonLoad) {
	} else if (_currentButton == kButtonMulti) {
	} else if (_currentButton == kButtonMovies) {
		subMenu(*_movies);
	} else if (_currentButton == kButtonOptions) {
	} else if (_currentButton == kButtonExit) {
		EventMan.requestQuit();
		return;
	}

	updateMouse();
}

void MainMenu::subMenu(Menu &menu) {
	hideModels();
	hideButtons();

	menu.show();
	menu.handle();
	menu.hide();

	showButtons();
	showModels();
}

} // End of namespace NWN

} // End of namespace Engines
