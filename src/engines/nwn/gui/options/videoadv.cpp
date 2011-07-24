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

/** @file engines/nwn/gui/options/videoadv.cpp
 *  The NWN advanced video options menu.
 */

#include "common/util.h"
#include "common/maths.h"
#include "common/configman.h"

#include "aurora/talkman.h"

#include "graphics/graphics.h"

#include "engines/nwn/gui/widgets/panel.h"
#include "engines/nwn/gui/widgets/label.h"
#include "engines/nwn/gui/widgets/checkbox.h"
#include "engines/nwn/gui/widgets/slider.h"

#include "engines/nwn/gui/options/videoadv.h"

namespace Engines {

namespace NWN {

OptionsVideoAdvancedMenu::OptionsVideoAdvancedMenu(bool isMain) {
	load("options_adv_vid");

	if (isMain) {
		WidgetPanel *backdrop = new WidgetPanel(*this, "PNL_MAINMENU", "pnl_mainmenu");
		backdrop->setPosition(0.0, 0.0, 100.0);
		addWidget(backdrop);
	}

	// TODO: Grass
	getWidget("GrassSlider", true)->setDisabled(true);

	// TODO: Creature wind
	getWidget("CreatureWind", true)->setDisabled(true);

	// TODO: Dynamic lights
	getWidget("LightSlider", true)->setDisabled(true);

	// TODO: Shadow casting lights
	getWidget("ShadowLights", true)->setDisabled(true);

	// TODO: Texture animations
	getWidget("TexAnimButton", true)->setDisabled(true);

	// TODO: Environment mapping on creatures
	getWidget("EnvMappingButton", true)->setDisabled(true);

	// TODO: High visual effects
	getWidget("VisEffectsBox", true)->setDisabled(true);

	// TODO: Shiny water
	getWidget("ShinyWaterBox", true)->setDisabled(true);

	// TODO: Use vsync
	getWidget("VsyncBox", true)->setDisabled(true);
}

OptionsVideoAdvancedMenu::~OptionsVideoAdvancedMenu() {
}

void OptionsVideoAdvancedMenu::show() {
	_oldFSAA = MAX(GfxMan.getCurrentFSAA(), 0);

	int fsaa = _oldFSAA;
	if (fsaa > 0)
		fsaa = MAX(0, Common::intLog2(fsaa));

	getSlider("AntiAliasSlider", true)->setState(fsaa);

	updateFSAALabel(fsaa);

	WidgetCheckBox *largeFonts = getCheckBox("UseLargeFont");
	if (largeFonts)
		largeFonts->setState(ConfigMan.getBool("largefonts"));

	GUI::show();
}

void OptionsVideoAdvancedMenu::fixWidgetType(const Common::UString &tag, WidgetType &type) {
	if (tag == "CreatureWind")
		type = kWidgetTypeSlider;
}

void OptionsVideoAdvancedMenu::initWidget(Widget &widget) {
	if (widget.getTag() == "AntiAliasSlider") {
		int maxFSAA = GfxMan.getMaxFSAA();

		if (maxFSAA <= 0)
			// No antialiasing available
			dynamic_cast<WidgetSlider &>(widget).setDisabled(true);
		else
			dynamic_cast<WidgetSlider &>(widget).setSteps(MAX(0, Common::intLog2(maxFSAA)));

		return;
	}
}

void OptionsVideoAdvancedMenu::callbackActive(Widget &widget) {
	if ((widget.getTag() == "CancelButton") ||
	    (widget.getTag() == "XButton")) {

		revertChanges();
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "OkButton") {

		adoptChanges();
		_returnCode = 2;
		return;
	}

	if (widget.getTag() == "AntiAliasSlider") {
		int fsaa = dynamic_cast<WidgetSlider &>(widget).getState();

		updateFSAALabel(fsaa);

		if (fsaa > 0)
			fsaa = 1 << fsaa;

		GfxMan.setFSAA(fsaa);
		return;
	}
}

void OptionsVideoAdvancedMenu::updateFSAALabel(int n) {
	Common::UString text;
	if      (n == 0)
		text = TalkMan.getString(67539);
	else if (n == 1)
		text = TalkMan.getString(67540);
	else if (n == 2)
		text = TalkMan.getString(67542);
	else
		text = Common::UString::sprintf("%dx %s", 1 << n, TalkMan.getString(67538).c_str());

	getLabel("AntialiasLabel", true)->setText(text);
}

void OptionsVideoAdvancedMenu::adoptChanges() {
	ConfigMan.setInt("fsaa", GfxMan.getCurrentFSAA(), true);

	WidgetCheckBox *largeFonts = getCheckBox("UseLargeFont");
	if (largeFonts)
		ConfigMan.setBool("largefonts", largeFonts->getState(), true);
}

void OptionsVideoAdvancedMenu::revertChanges() {
	GfxMan.setFSAA(_oldFSAA);
}

} // End of namespace NWN

} // End of namespace Engines
