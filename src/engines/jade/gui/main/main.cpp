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

#include "src/engines/odyssey/label.h"
#include "src/engines/odyssey/listbox.h"
#include "src/engines/odyssey/button.h"

#include "src/engines/jade/arealayout.h"
#include "src/engines/jade/module.h"

#include "src/engines/jade/gui/main/main.h"
#include "src/engines/jade/gui/main/options.h"
#include "src/engines/jade/gui/chargen/characterselection.h"

namespace Engines {

namespace Jade {

MainMenu::MainMenu(Module &module, ::Engines::Console *console) : ::Engines::Jade::GUI(console),
	_module(&module), _background(0) {

	load("maingame");

	/*
	 * The list box is initially empty, so we need to create the buttons
	 * for the main menu
	 */
	Odyssey::WidgetListBox *listBoxButtons = getListBox("ListBoxButtons");
	listBoxButtons->setFill("");
	listBoxButtons->createItemWidgets(6);

	listBoxButtons->addItem(TalkMan.getString(111));    // New Game
	listBoxButtons->addItem(TalkMan.getString(112));    // Load Game
	listBoxButtons->addItem(TalkMan.getString(114));    // Minigames
	listBoxButtons->addItem(TalkMan.getString(116));    // Options
	listBoxButtons->addItem(TalkMan.getString(15709));  // Credits
	listBoxButtons->addItem(TalkMan.getString(112745)); // Exit

	listBoxButtons->refreshItemWidgets();

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
	::Engines::Jade::GUI::show();
}

void MainMenu::hide() {
	if (_background)
		_background->hide();
	::Engines::Jade::GUI::hide();
}

void MainMenu::callbackActive(Widget &widget) {
	if (widget.getTag() == "ListBoxButtons_ITEM_3") {
		if (!_options)
			createOptions();

		sub(*_options);
	}

	if (widget.getTag() == "ListBoxButtons_ITEM_0") {
		CharacterGeneration characterSelection(_module);
		uint32_t returnCode = sub(characterSelection);
		if (returnCode == 2)
			_returnCode = returnCode;
		return;
	}

	if (widget.getTag() == "ListBoxButtons_ITEM_4") {
		playVideo("creditmovie");
		return;
	}

	if (widget.getTag() == "ListBoxButtons_ITEM_5")
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
	_options = std::make_unique<OptionsMenu>();
}

} // End of namespace Jade

} // End of namespace Engines
