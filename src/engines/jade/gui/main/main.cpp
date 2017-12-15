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

#include "src/common/util.h"

#include "src/aurora/talkman.h"

#include "src/graphics/windowman.h"

#include "src/engines/jade/module.h"

#include "src/engines/jade/gui/main/main.h"

#include "src/engines/kotor/gui/widgets/label.h"
#include "src/engines/kotor/gui/widgets/listbox.h"
#include "src/engines/kotor/gui/widgets/button.h"

namespace Engines {

namespace Jade {

MainMenu::MainMenu(Module &module, ::Engines::Console *console) : ::Engines::KotOR::GUI(console),
	_module(&module) {

	load("maingame");

	/*
	 * The list box is initially empty, so we need to create the buttons
	 * for the main menu
	 */
	getListBox("ListBoxButtons")->setFill("");

	addWidget(getListBox("ListBoxButtons")->createItem("NEW_GAME"));
	addWidget(getListBox("ListBoxButtons")->createItem("LOAD_GAME"));
	addWidget(getListBox("ListBoxButtons")->createItem("MINIGAMES"));
	addWidget(getListBox("ListBoxButtons")->createItem("OPTIONS"));
	addWidget(getListBox("ListBoxButtons")->createItem("CREDITS"));
	addWidget(getListBox("ListBoxButtons")->createItem("EXIT"));

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

	// No clue for this text, we make it invisible
	getLabel("LabelLegend")->setInvisible(true);
	getLabel("Labellegends")->setInvisible(true);
}

MainMenu::~MainMenu() {
}

void MainMenu::callbackActive(Widget &widget) {
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
}

} // End of namespace Jade

} // End of namespace Engines
