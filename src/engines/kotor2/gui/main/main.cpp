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
 *  The KotOR 2 main menu.
 */

#include "src/common/util.h"

#include "src/sound/sound.h"

#include "src/events/events.h"

#include "src/engines/aurora/util.h"

#include "src/engines/odyssey/widget.h"

#include "src/engines/kotorbase/module.h"

#include "src/engines/kotor2/gui/gui.h"

#include "src/engines/kotor2/gui/main/main.h"
#include "src/engines/kotor2/gui/chargen/classselection.h"

namespace Engines {

namespace KotOR2 {

MainMenu::MainMenu(KotORBase::Module &module, ::Engines::Console *console) :
		KotORBase::GUI(console),
		_module(&module) {

	load("mainmenu8x6_p");

	startMainMusic();
}

MainMenu::~MainMenu() {
}

void MainMenu::initWidget(Widget &widget) {
	KotOR2::initWidget(widget);

	// ...BioWare...
	if (widget.getTag() == "LBL_GAMELOGO") {
		dynamic_cast< Odyssey::Widget & >(widget).setFill("kotor2logo");
		return;
	}

	// Warp button? O_o
	if (widget.getTag() == "BTN_WARP") {
		widget.setInvisible(true);
		return;
	}

	// New downloadable content is available, bluhbluh.
	if (widget.getTag() == "LBL_NEWCONTENT") {
		widget.setInvisible(true);
		return;
	}

	// Whatever this module stuff is about
	if (widget.getTag() == "LB_MODULES") {
		widget.setInvisible(true);
		return;
	}
}

void MainMenu::createClassSelection() {
	if (!_classSelection)
		_classSelection.reset(new ClassSelection(_module));
}

void MainMenu::startMainMusic() {
	_menuMusic = playSound("mus_sion", Sound::kSoundTypeMusic, true);
}

void MainMenu::startChargenMusic() {
	_menuMusic = playSound("mus_main", Sound::kSoundTypeMusic, true);
}

void MainMenu::stopMusic() {
	SoundMan.stopChannel(_menuMusic);
}

void MainMenu::callbackActive(Widget &widget) {
	if (widget.getTag() == "BTN_EXIT") {
		EventMan.requestQuit();

		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "BTN_NEWGAME") {
		createClassSelection();
		stopMusic();
		startChargenMusic();
		if (sub(*_classSelection) == 2) {
			stopMusic();
			_returnCode = 2;
			return;
		}
		stopMusic();
		startMainMusic();
		return;
	}
}

} // End of namespace KotOR2

} // End of namespace Engines
