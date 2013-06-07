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

/** @file engines/nwn/gui/main/mainmenu.cpp
 *  The main menu.
 */

#include "common/configman.h"

#include "events/events.h"

#include "engines/nwn/module.h"

#include "engines/nwn/gui/widgets/panel.h"

#include "engines/nwn/gui/main/mainmenu.h"
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
		xp1->setPosition(124.0, 0.00, -50.0);
		addWidget(xp1);
	}

	if (hasXP2) {
		WidgetPanel *xp2 = new WidgetPanel(*this, "TextXP2", "ctl_xp2_text");
		xp2->setPosition(124.0, -147.0, -50.0);
		addWidget(xp2);
	}

	getWidget("LoadButton" , true)->setDisabled(true);
	getWidget("MultiButton", true)->setDisabled(true);

	_charType = new CharTypeMenu(*_module);

	_new     = 0;
	_movies  = 0;
	_options = 0;
}

MainMenu::~MainMenu() {
	delete _options;
	delete _movies;
	delete _new;

	delete _charType;
}

void MainMenu::createNew() {
	if (_new)
		return;

	if (_hasXP)
		// If we have at least an expansion, create the campaign selection game menu
		_new = new NewCampMenu(*_module, *_charType);
	else
		// If not, create the base game menu
		_new = new NewMenu(*_module, *_charType);
}

void MainMenu::createMovies() {
	if (_movies)
		return;

	if (_hasXP)
		// If we have at least an expansion, create the campaign selection movies menu
		_movies = new MoviesCampMenu;
	else
		// If not, create the base game movies menu
		_movies = new MoviesBaseMenu;
}

void MainMenu::createOptions() {
	if (_options)
		return;

	_options = new OptionsMenu();
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
		createNew();

		NewGameFogs fogs(ConfigMan.getInt("menufogcount", 4));
		fogs.show();

		if (sub(*_new, 0, false) == 2) {
			_returnCode = 2;
			return;
		}

		show();
		return;
	}

	if (widget.getTag() == "MoviesButton") {
		createMovies();

		sub(*_movies);
		return;
	}

	if (widget.getTag() == "OptionsButton") {
		createOptions();

		sub(*_options);
		return;
	}

}

} // End of namespace NWN

} // End of namespace Engines
