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

/** @file engines/kotor/gui/main/main.cpp
 *  The KotOR main menu.
 */

#include "common/util.h"

#include "events/events.h"

#include "engines/aurora/widget.h"

#include "engines/kotor/module.h"

#include "engines/kotor/gui/main/main.h"
#include "engines/kotor/gui/main/movies.h"
#include "engines/kotor/gui/main/options.h"

#include "gui/widgets/button.h"

namespace Engines {

namespace KotOR {

MainMenu::MainMenu(Module &module, bool isXbox) : _module(&module), _isXbox(isXbox) {
	load(isXbox ? "mainmenu" : "mainmenu16x12");

	addBackground("back");

	_movies = 0;
	_options = 0;
}

MainMenu::~MainMenu() {
}


void MainMenu::createMovies() {
	if (_movies)
		return;

	// Create the movies menu
	_movies = new MoviesMenu;
}

void MainMenu::createOptions() {
	if (_options)
		return;

	// Create the options menu
	_options = new OptionsMenu;

}

void MainMenu::initWidget(Widget &widget) {
	// BioWare logo, the original game doesn't display it.
	if (widget.getTag() == "LBL_BW") {
		widget.setInvisible(true);
		return;
	}

	// LucasArts logo, the original game doesn't display it.
	if (widget.getTag() == "LBL_LUCAS") {
		widget.setInvisible(true);
		return;
	}

	// Warp button? O_o
	if ((widget.getTag() == "BTN_WARP") || (widget.getTag() == "LBL_WARP")) {
		widget.setInvisible(true);
		return;
	}

	// "Y" label
	if (widget.getTag() == "LBL_Y") {
		widget.setInvisible(true);
		return;
	}

	// New downloadable content is available, bluhbluh.
	if (widget.getTag() == "LBL_NEWCONTENT") {
		widget.setInvisible(true);
		return;
	}

	// New game button. Forcibly move it to the front, for the Xbox version
	if (widget.getTag() == "BTN_NEWGAME") {
		float x, y, z;
		widget.getPosition(x, y, z);

		widget.setPosition(x, y, z - 10.0);
	}
}

void MainMenu::callbackActive(Widget &widget) {

	if (widget.getTag() == "BTN_NEWGAME") {
		try {
			_module->load("lev_m40aa");
		} catch (Common::Exception &e) {
			Common::printException(e, "WARNING: ");
			return;
		}

		_returnCode = 2;
		return;
	}

	if (widget.getTag() == "BTN_LOADGAME") {

	}

	if (widget.getTag() == "BTN_MOVIES") {
		createMovies();

		sub(*_movies);
		return;
	}

	if (widget.getTag() == "BTN_OPTIONS") {
		createOptions();

		sub(*_options);
		return;
	}

	if (widget.getTag() == "BTN_EXIT") {
		EventMan.requestQuit();

		_returnCode = 1;
		return;
	}
}

} // End of namespace KotOR

} // End of namespace Engines
