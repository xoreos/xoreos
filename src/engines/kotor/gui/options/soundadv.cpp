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
 *  The advanced sound menu.
 */

#include "src/common/configman.h"

#include "src/aurora/talkman.h"

#include "src/engines/aurora/widget.h"

#include "src/engines/odyssey/button.h"
#include "src/engines/odyssey/checkbox.h"

#include "src/engines/kotor/gui/options/soundadv.h"

namespace Engines {

namespace KotOR {

OptionsSoundAdvancedMenu::OptionsSoundAdvancedMenu(::Engines::Console *console) : KotORBase::GUI(console) {
	load("optsoundadv");

	addBackground(KotORBase::kBackgroundTypeMenu);

	// Hardcoded, the gui file returns incorrect values
	getButton("BTN_EAXLEFT", true)->setColor(0.0f, 0.658824f, 0.980392f, 1.0f);
	getButton("BTN_EAXLEFT", true)->setStaticHighlight();
	getButton("BTN_EAXRIGHT", true)->setColor(0.0f, 0.658824f, 0.980392f, 1.0f);
	getButton("BTN_EAXRIGHT", true)->setStaticHighlight();
	getCheckBox("CB_FORCESOFTWARE", true)->setColor(0.0f, 0.658824f, 0.980392f, 1.0f);

	readConfig();
}

OptionsSoundAdvancedMenu::~OptionsSoundAdvancedMenu() {
}

void OptionsSoundAdvancedMenu::show() {
	readConfig();
	displayConfig();

	GUI::show();
}

void OptionsSoundAdvancedMenu::callbackActive(Widget &widget) {

	if (widget.getTag() == "BTN_EAXRIGHT") {
		if (0) { // if we have EAX
			_eax++;
			if (_eax > 3) {
				_eax = 3;
			}
			updateEAX(_eax);
		}
		return;
	}

	if (widget.getTag() == "BTN_EAXLEFT") {
		_eax--;
		if (_eax < 0) {
			_eax = 0;
		}
		updateEAX(_eax);
		return;
	}

	if (widget.getTag() == "BTN_DEFAULT") {
		setDefault();
		readConfig();
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

	if (widget.getTag() == "CB_FORCESOFTWARE") {
		_forceSoftware = getCheckBoxState("CB_FORCESOFTWARE");
		return;
	}
}

void OptionsSoundAdvancedMenu::setDefault() {
	_eax = ConfigMan.getDefaultInt("eax");
	_forceSoftware = ConfigMan.getDefaultBool("forcesoftware");
}

void OptionsSoundAdvancedMenu::readConfig() {
	_eax = CLIP(ConfigMan.getInt("eax"), 0, 3);
	_forceSoftware = ConfigMan.getBool("forcesoftware");
}

void OptionsSoundAdvancedMenu::displayConfig() {
	updateEAX(_eax);

	setCheckBoxState("CB_FORCESOFTWARE", _forceSoftware);
}

void OptionsSoundAdvancedMenu::updateEAX(int eax) {
	Odyssey::WidgetButton &eaxButton = *getButton("BTN_EAX", true);
	Odyssey::WidgetButton &leftButton = *getButton("BTN_EAXLEFT", true);
	Odyssey::WidgetButton &rightButton = *getButton("BTN_EAXRIGHT", true);

	eaxButton.setText(TalkMan.getString(48573 + eax));

	if (_eax == 0) {
		leftButton.hide();
	} else {
		leftButton.show();
	}

	if (_eax == 3) {
		rightButton.hide();
	} else {
		rightButton.show();
	}
}

void OptionsSoundAdvancedMenu::adoptChanges() {
	ConfigMan.setInt("eax", _eax, true);
	ConfigMan.setBool("forcesoftware", _forceSoftware, true);
}

} // End of namespace KotOR

} // End of namespace Engines
