/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
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
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file engines/kotor2/gui/main/main.cpp
 *  The KotOR 2 main menu.
 */

#include "common/util.h"

#include "events/events.h"

#include "engines/kotor/gui/widgets/kotorwidget.h"

#include "engines/kotor2/module.h"

#include "engines/kotor2/gui/main/main.h"

namespace Engines {

namespace KotOR2 {

MainMenu::MainMenu(Module &module) : _module(&module) {
	load("mainmenu16x12_p");
}

MainMenu::~MainMenu() {
}

void MainMenu::initWidget(Widget &widget) {
	// ...BioWare...
	if (widget.getTag() == "LBL_GAMELOGO") {
		dynamic_cast< ::Engines::KotOR::KotORWidget & >(widget).setFill("kotor2logo");
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
}

void MainMenu::callbackActive(Widget &widget) {
	if (widget.getTag() == "BTN_EXIT") {
		EventMan.requestQuit();

		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "BTN_NEWGAME") {
		if (_module->load("001EBO"))
			_returnCode = 2;
		return;
	}
}

} // End of namespace KotOR2

} // End of namespace Engines
