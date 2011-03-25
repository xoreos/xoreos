/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
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
		fsaa = log2(fsaa);

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
			dynamic_cast<WidgetSlider &>(widget).setSteps(log2(maxFSAA));

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
