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
 *  The main menu.
 */

#include "src/common/configman.h"

#include "src/events/events.h"

#include "src/engines/nwn/module.h"

#include "src/engines/nwn/gui/widgets/panel.h"

#include "src/engines/nwn/gui/main/main.h"
#include "src/engines/nwn/gui/main/newgamefog.h"
#include "src/engines/nwn/gui/main/new.h"
#include "src/engines/nwn/gui/main/newcamp.h"
#include "src/engines/nwn/gui/main/moviesbase.h"
#include "src/engines/nwn/gui/main/moviescamp.h"
#include "src/engines/nwn/gui/main/options.h"
#include "src/engines/nwn/gui/main/chartype.h"

namespace Engines {

namespace NWN {

MainMenu::MainMenu(Module &module, ::Engines::Console *console) : GUI(console),
	_module(&module) {

	load("pre_main");

	bool hasXP1 = ConfigMan.getBool("NWN_hasXP1");
	bool hasXP2 = ConfigMan.getBool("NWN_hasXP2");

	_hasXP = hasXP1 || hasXP2;

	if (hasXP1) {
		WidgetPanel *xp1 = new WidgetPanel(*this, "TextXP1", "ctl_xp1_text");
		xp1->setPosition(124.0f, 0.00f, -50.0f);
		addWidget(xp1);
	}

	if (hasXP2) {
		WidgetPanel *xp2 = new WidgetPanel(*this, "TextXP2", "ctl_xp2_text");
		xp2->setPosition(124.0f, -147.0f, -50.0f);
		addWidget(xp2);
	}

	getWidget("LoadButton" , true)->setDisabled(true);
	getWidget("MultiButton", true)->setDisabled(true);

	_charType.reset(new CharTypeMenu(*_module, _console));
}

MainMenu::~MainMenu() {
}

void MainMenu::createNew() {
	if (_new)
		return;

	if (_hasXP)
		// If we have at least an expansion, create the campaign selection game menu
		_new.reset(new NewCampMenu(*_module, *_charType, _console));
	else
		// If not, create the base game menu
		_new.reset(new NewMenu(*_module, *_charType, _console));
}

void MainMenu::createMovies() {
	if (_movies)
		return;

	if (_hasXP)
		// If we have at least an expansion, create the campaign selection movies menu
		_movies.reset(new MoviesCampMenu(_console));
	else
		// If not, create the base game movies menu
		_movies.reset(new MoviesBaseMenu(_console));
}

void MainMenu::createOptions() {
	if (_options)
		return;

	_options.reset(new OptionsMenu(_module->getGameVersion(), _console));
}

void MainMenu::show() {
	GUI::show();

	getWidget("NewButton", true)->show();
}

void MainMenu::abort() {
	// If we're aborting the main menu, quit everything
	EventMan.requestQuit();

	GUI::abort();
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
