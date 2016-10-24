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
 *  The campaign movies menu.
 */

#include "src/common/configman.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/widget.h"

#include "src/engines/nwn/gui/main/moviescamp.h"
#include "src/engines/nwn/gui/main/moviesbase.h"

namespace Engines {

namespace NWN {

MoviesCampMenu::MoviesCampMenu(::Engines::Console *console) : GUI(console) {
	load("pre_camp_movies");

	Widget *button = 0;

	button = getWidget("NWNXP1Button");
	if (button)
		button->setDisabled(!ConfigMan.getBool("NWN_hasXP1"));
	button = getWidget("NWNXP2Button");
	if (button)
		button->setDisabled(!ConfigMan.getBool("NWN_hasXP2"));
	button = getWidget("NWNXP3Button");
	if (button)
		button->setDisabled(true);

	_base.reset(new MoviesBaseMenu(_console));
}

MoviesCampMenu::~MoviesCampMenu() {
}

void MoviesCampMenu::callbackActive(Widget &widget) {
	if (widget.getTag() == "CancelButton") {
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "NWNButton") {
		sub(*_base);
		return;
	}

	if        (widget.getTag() == "NWNXP1Button") {
		// No GUI file? Hardcoded?

		// Just play them one after another for now...
		playVideo("xp1_intro");
		playVideo("xp1_chap1_chap2");
		playVideo("xp1_chap2_chap3");
		playVideo("xp1_closing");
	} else if (widget.getTag() == "NWNXP2Button") {
		// No GUI file? Harcoded?

		// Just play the intro for now...
		// (Which is the only actual video anyway, the rest is in-game cinematics)
		playVideo("xp2_intro");
	}

	updateMouse();
}

} // End of namespace NWN

} // End of namespace Engines
