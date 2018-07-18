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
 *  The KotOR main menu.
 */

#include <boost/bind.hpp>

#include "glm/gtc/matrix_transform.hpp"

#include "src/common/util.h"

#include "src/sound/sound.h"

#include "src/events/events.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/widget.h"

#include "src/engines/kotor/module.h"
#include "src/engines/kotor/version.h"

#include "src/engines/kotor/gui/guibackground.h"
#include "src/engines/kotor/gui/saveload.h"

#include "src/engines/kotor/gui/loadscreen/loadscreen.h"

#include "src/engines/aurora/model.h"
#include "src/engines/aurora/kotorjadegui/button.h"
#include "src/engines/aurora/kotorjadegui/listbox.h"

#include "src/engines/kotor/gui/main/main.h"
#include "src/engines/kotor/gui/main/movies.h"
#include "src/engines/kotor/gui/main/options.h"

#include "src/engines/kotor/gui/chargen/classselection.h"

namespace Engines {

namespace KotOR {

MainMenu::MainMenu(const Version &gameVersion, Module &module, ::Engines::Console *console) : GUI(console),
	_module(&module), _gameVersion(&gameVersion) {

	_module->loadTexturePack();

	load((_gameVersion->getPlatform() == Aurora::kPlatformXbox) ? "mainmenu" : "mainmenu16x12");

	getListBox("LB_MODULES")->setInvisible(true);

	addBackground(kBackgroundTypeMenu);

	startMainMusic();

	_malakScene.reset(new Graphics::Aurora::SubSceneQuad);

	_malakModel.reset(loadModelObject("mainmenu"));

	if (_malakModel) {
		_malakModel->playAnimation("default", true, -1);
		_malakModel->setOrientation(1.0f, 0.0f, 0.0f, -90.0f);

		_malakScene->add(_malakModel.get());

		getLabel("LBL_3DVIEW")->setSubScene(_malakScene.get());
		// TODO: Possibly change this to perspective projection.
		glm::mat4 projection = glm::ortho(-2.0f, 2.0f, -0.2f, 2.6f, -3.0f, 3.0f);
		_malakScene->setProjectionMatrix(projection);
	}
}

MainMenu::~MainMenu() {
}

void MainMenu::createClassSelection() {
	hide();
	LoadScreen loadScreen("chargen", _console);
	loadScreen.show();

	// Load the texture pack, needed for the character generator
	_module->loadTexturePack();

	// Create the class selection menu
	_classSelection.reset(new ClassSelectionMenu(_module, _console));

	loadScreen.hide();
	show();
}

void MainMenu::createMovies() {
	if (_movies)
		return;

	// Create the movies menu
	_movies.reset(new MoviesMenu(_console));
}

void MainMenu::createOptions() {
	if (_options)
		return;

	// Create the options menu
	_options.reset(new OptionsMenu(*_gameVersion, _console));

}

void MainMenu::createLoadGame() {
	if (_loadGame)
		return;

	// Create the load game menu
	_loadGame.reset(new SaveLoadMenu(*_module, _console));
}

void MainMenu::startMainMusic() {
	_menuMusic = playSound("mus_theme_cult", Sound::kSoundTypeMusic, true);
}

void MainMenu::startCharGenMusic() {
	_menuMusic = playSound("mus_theme_rep", Sound::kSoundTypeMusic, true);
}

void MainMenu::stopMenuMusic() {
	SoundMan.stopChannel(_menuMusic);
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

		widget.setPosition(x, y, z - 10.0f);
	}
}

void MainMenu::callbackActive(Widget &widget) {

	if (widget.getTag() == "BTN_NEWGAME") {
		createClassSelection();

		// Stop the currently running main music
		stopMenuMusic();

		// Start the charGen music
		startCharGenMusic();
		if (sub(*_classSelection) == 2) {
			_returnCode = 2;
			stopMenuMusic();
			return;
		}

		// If we return from the chargen we stop the music and play the main music
		stopMenuMusic();
		startMainMusic();

		return;
	}

	if (widget.getTag() == "BTN_LOADGAME") {
		createLoadGame();

		if (sub(*_loadGame) == 2) {
			_returnCode = 2;
			stopMenuMusic();
		}

		return;
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
