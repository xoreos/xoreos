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
 *  The graphics menu.
 */

#include "src/common/configman.h"

#include "src/engines/aurora/widget.h"

#include "src/engines/kotor/gui/options/graphics.h"
#include "src/engines/kotor/gui/options/graphicsadv.h"

#include "src/engines/kotor/gui/widgets/checkbox.h"

namespace Engines {

namespace KotOR {

OptionsGraphicsMenu::OptionsGraphicsMenu(::Engines::Console *console) : GUI(console) {
	load("optgraphics");

	_advanced.reset(new OptionsGraphicsAdvancedMenu(_console));

	//Hardcoded, the gui file returns incorrect values
	getCheckBox("CB_SHADOWS", true)->setColor(0.0f, 0.658824f, 0.980392f, 1.0f);
	getCheckBox("CB_GRASS", true)->setColor(0.0f, 0.658824f, 0.980392f, 1.0f);
}

OptionsGraphicsMenu::~OptionsGraphicsMenu() {
}

void OptionsGraphicsMenu::show() {
	GUI::show();

	_shadows = ConfigMan.getBool("shadows", true);
	setCheckBoxState("CB_SHADOWS", _shadows);

	_grass = ConfigMan.getBool("grass", true);
	setCheckBoxState("CB_GRASS", _grass);
}

void OptionsGraphicsMenu::callbackActive(Widget &widget) {

	if (widget.getTag() == "BTN_ADVANCED") {
		adoptChanges();
		sub(*_advanced);
		return;
	}

	if (widget.getTag() == "BTN_DEFAULT") {
		_shadows = true;
		setCheckBoxState("CB_SHADOWS", _shadows);

		_grass = true;
		setCheckBoxState("CB_GRASS", _grass);
	}

	if (widget.getTag() == "BTN_BACK") {
		adoptChanges();
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "CB_SHADOWS") {
		_shadows = getCheckBoxState("CB_SHADOWS");
		return;
	}

	if (widget.getTag() == "CB_GRASS") {
		_grass = getCheckBoxState("CB_GRASS");
		return;
	}
}

void OptionsGraphicsMenu::adoptChanges() {
	ConfigMan.setBool("shadows", _shadows, false);
	ConfigMan.setBool("grass", _grass, false);
}

} // End of namespace KotOR

} // End of namespace Engines

