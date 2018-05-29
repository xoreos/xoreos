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
 *  The Jade Empire main menu.
 */

#include "src/common/configman.h"
#include "src/common/error.h"
#include "src/common/ustring.h"

#include "src/events/events.h"

#include "src/aurora/2dareg.h"
#include "src/aurora/2dafile.h"
#include "src/aurora/talkman.h"

#include "src/engines/aurora/util.h"

#include "src/engines/jade/arealayout.h"
#include "src/engines/jade/module.h"

#include "src/engines/jade/gui/main/main.h"
#include "src/engines/jade/gui/main/options.h"

#include "src/engines/kotor/gui/widgets/label.h"
#include "src/engines/kotor/gui/widgets/listbox.h"
#include "src/engines/kotor/gui/widgets/button.h"

namespace Engines {

namespace Jade {

MainMenu::MainMenu(Module &module, ::Engines::Console *console) : ::Engines::KotOR::GUI(console),
	_module(&module), _background(0) {

	load("maingame");

	/*
	 * The list box is initially empty, so we need to create the buttons
	 * for the main menu
	 */
	getListBox("ListBoxButtons")->setFill("");

	addWidget(getListBox("ListBoxButtons")->createItem("NEW_GAME"), true);
	addWidget(getListBox("ListBoxButtons")->createItem("LOAD_GAME"), true);
	addWidget(getListBox("ListBoxButtons")->createItem("MINIGAMES"), true);
	addWidget(getListBox("ListBoxButtons")->createItem("OPTIONS"), true);
	addWidget(getListBox("ListBoxButtons")->createItem("CREDITS"), true);
	addWidget(getListBox("ListBoxButtons")->createItem("EXIT"), true);

	getButton("NEW_GAME")->setText(TalkMan.getString(111));
	getButton("LOAD_GAME")->setText(TalkMan.getString(112));
	getButton("MINIGAMES")->setText(TalkMan.getString(114));
	getButton("OPTIONS")->setText(TalkMan.getString(116));
	getButton("CREDITS")->setText(TalkMan.getString(15709));
	getButton("EXIT")->setText(TalkMan.getString(112745));

	/*
	 * The Jade Empire Logo is placed in the middle of the screen
	 * and needs to be moved above the menu buttons
	 */
	getLabel("TitleLabel")->setWidth(256);
	getLabel("TitleLabel")->setHeight(65);

	float tX, tY, tZ;
	getLabel("TitleLabel")->getPosition(tX, tY, tZ);
	getLabel("TitleLabel")->setPosition(tX-80, tY+52, tZ);

	addBackground();
}

MainMenu::~MainMenu() {
	delete _background;
	_background = NULL;
}

void MainMenu::show() {
	if (_background)
		_background->show();
	::Engines::KotOR::GUI::show();
}

void MainMenu::hide() {
	if (_background)
		_background->hide();
	::Engines::KotOR::GUI::hide();
}

void MainMenu::callbackActive(Widget &widget) {
	if (widget.getTag() == "OPTIONS") {
		if (!_options)
			createOptions();

		sub(*_options);
	}

	if (widget.getTag() == "NEW_GAME") {
		try {
			_module->load("j01_town");
			_returnCode = 1;
		} catch (...) {
			Common::exceptionDispatcherWarning();
			return;
		}
		return;
	}

	if (widget.getTag() == "CREDITS") {
		playVideo("creditmovie");
		return;
	}

	if (widget.getTag() == "EXIT")
		EventMan.requestQuit();
}

void MainMenu::addBackground() {
	const Aurora::TwoDAFile &startrooms = TwoDAReg.get2DA("startrooms");

	Common::UString currentChapter = ConfigMan.getString("chapter", "1");
	if (startrooms.getRow("chapter", currentChapter).empty("room"))
		currentChapter = "1";

	const Common::UString &room = startrooms.getRow("chapter", currentChapter).getString("room");

	_background = new AreaLayout(room);
}

void MainMenu::createOptions() {
	_options.reset(new OptionsMenu());
}

} // End of namespace Jade

} // End of namespace Engines
