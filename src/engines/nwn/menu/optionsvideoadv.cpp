/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/optionsvideoadv.cpp
 *  The advanced video options menu.
 */

#include "engines/nwn/menu/optionsvideoadv.h"

#include "engines/aurora/util.h"

namespace Engines {

namespace NWN {

OptionsVideoAdvancedMenu::OptionsVideoAdvancedMenu() : GUI("options_adv_vid") {
	// TODO: Grass
	getWidget("GrassSlider", true)->setDisabled(true);

	// TODO: Creature wind
	getWidget("CreatureWind", true)->setDisabled(true);

	// TODO: Dynamic lights
	getWidget("LightSlider", true)->setDisabled(true);

	// TODO: Shadow casting lights
	getWidget("ShadowLights", true)->setDisabled(true);

	// TODO: Anti-Aliasing
	getWidget("AntiAliasSlider", true)->setDisabled(true);

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

	// TODO: Use large font
	getWidget("UseLargeFont", true)->setDisabled(true);
}

OptionsVideoAdvancedMenu::~OptionsVideoAdvancedMenu() {
}

void OptionsVideoAdvancedMenu::callbackActive(Widget &widget) {
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
