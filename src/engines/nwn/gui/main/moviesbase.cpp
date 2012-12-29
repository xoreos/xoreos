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

/** @file engines/nwn/gui/main/moviesbase.cpp
 *  The base game movies menu.
 */

#include "engines/aurora/util.h"
#include "engines/aurora/widget.h"

#include "engines/nwn/gui/main/moviesbase.h"

namespace Engines {

namespace NWN {

MoviesBaseMenu::MoviesBaseMenu() {
	load("pre_movies");
}

MoviesBaseMenu::~MoviesBaseMenu() {
}

void MoviesBaseMenu::callbackActive(Widget &widget) {
	if (widget.getTag() == "CloseButton") {
		_returnCode = 1;
		return;
	}

	if      (widget.getTag() == "PreludeButton")
		playVideo("prelude");
	else if (widget.getTag() == "IntroButton")
		playVideo("prelude");
	else if (widget.getTag() == "Chpt1Button")
		playVideo("prelude_chap1");
	else if (widget.getTag() == "Chpt2Button")
		playVideo("chap1_chap2");
	else if (widget.getTag() == "Chpt3Button")
		playVideo("chap2_chap3");
	else if (widget.getTag() == "Chpt4Button")
		playVideo("chap3_chap4");
	else if (widget.getTag() == "EndButton")
		playVideo("ending");
	else if (widget.getTag() == "CreditsButton")
		playVideo("credits");

	updateMouse();
}

} // End of namespace NWN

} // End of namespace Engines
