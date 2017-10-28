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
 *  The advanced graphics menu.
 */

#include "src/common/configman.h"

#include "src/engines/aurora/widget.h"

#include "src/engines/kotor/gui/options/graphicsadv.h"

#include "src/engines/kotor/gui/widgets/checkbox.h"

namespace Engines {

namespace KotOR {

OptionsGraphicsAdvancedMenu::OptionsGraphicsAdvancedMenu(::Engines::Console *console) : GUI(console) {
	load("optgraphicsadv");

	//Hardcoded, the gui file returns incorrect values
	getCheckBox("CB_FRAMEBUFF", true)->setColor(0.0f, 0.658824f, 0.980392f, 1.0f);
	getCheckBox("CB_VSYNC", true)->setColor(0.0f, 0.658824f, 0.980392f, 1.0f);
	getCheckBox("CB_SOFTSHADOWS", true)->setColor(0.0f, 0.658824f, 0.980392f, 1.0f);

}

OptionsGraphicsAdvancedMenu::~OptionsGraphicsAdvancedMenu() {

}

void OptionsGraphicsAdvancedMenu::show() {
	GUI::show();

	_frameBufferEffects = ConfigMan.getBool("framebuffereffects", false);
	setCheckBoxState("CB_FRAMEBUFF", _frameBufferEffects);

	_softShadows = ConfigMan.getBool("softshadows", false);
	setCheckBoxState("CB_SOFTSHADOWS", _softShadows);

	_vsync = ConfigMan.getBool("vsync", false);
	setCheckBoxState("CB_VSYNC", _vsync);
}

void OptionsGraphicsAdvancedMenu::callbackActive(Widget &widget) {

	if (widget.getTag() == "BTN_DEFAULT") {
		_frameBufferEffects = false;
		setCheckBoxState("CB_FRAMEBUFF", _frameBufferEffects);

		_softShadows = false;
		setCheckBoxState("CB_SOFTSHADOWS", _softShadows);

		_vsync = false;
		setCheckBoxState("CB_VSYNC", _vsync);
	}

	if (widget.getTag() == "BTN_CANCEL") {
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "BTN_BACK") {
		adoptChanges();
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "CB_FRAMEBUFF") {
		_frameBufferEffects = getCheckBoxState("CB_FRAMEBUFF");
		return;
	}

	if (widget.getTag() == "CB_SOFTSHADOWS") {
		_softShadows = getCheckBoxState("CB_SOFTSHADOWS");
		return;
	}

	if (widget.getTag() == "CB_VSYNC") {
		_vsync = getCheckBoxState("CB_VSYNC");
		return;
	}
}

void OptionsGraphicsAdvancedMenu::adoptChanges() {
	ConfigMan.setBool("framebuffereffects", _softShadows, true);
	ConfigMan.setBool("softshadows", _softShadows, true);
	ConfigMan.setBool("vsync", _vsync, true);
}

} // End of namespace KotOR

} // End of namespace Engines



