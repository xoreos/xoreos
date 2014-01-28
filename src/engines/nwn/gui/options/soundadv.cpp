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

/** @file engines/nwn/gui/options/soundadv.cpp
 *  The NWN advanced sound options menu.
 */

#include "engines/nwn/gui/widgets/panel.h"

#include "engines/nwn/gui/options/soundadv.h"

namespace Engines {

namespace NWN {

OptionsSoundAdvancedMenu::OptionsSoundAdvancedMenu(bool isMain) : _isMain(isMain) {
	setPosition(0.0, 0.0, -290.0);
	load("options_advsound");

	if (_isMain) {
		WidgetPanel *backdrop = new WidgetPanel(*this, "PNL_MAINMENU", "pnl_mainmenu");
		backdrop->setPosition(0.0, 0.0, -300.0);
		addWidget(backdrop);
	}

	// TODO: Sound providers
	getWidget("ProviderList", true)->setDisabled(true);

	// TODO: 2D/3D bias
	getWidget("2D3DBiasSlider", true)->setDisabled(true);

	// TODO: EAX level
	getWidget("EAXSlider", true)->setDisabled(true);

	// TODO: Test the sound settings
	getWidget("TestButton", true)->setDisabled(true);
}

OptionsSoundAdvancedMenu::~OptionsSoundAdvancedMenu() {
}

void OptionsSoundAdvancedMenu::fixWidgetType(const Common::UString &tag, WidgetType &type) {
	if (tag == "ProviderList")
		type = kWidgetTypeListBox;
}

void OptionsSoundAdvancedMenu::callbackActive(Widget &widget) {
	if ((widget.getTag() == "CancelButton") ||
	    (widget.getTag() == "XButton")) {
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "OkButton") {
		_returnCode = 2;
		return;
	}
}

} // End of namespace NWN

} // End of namespace Engines
