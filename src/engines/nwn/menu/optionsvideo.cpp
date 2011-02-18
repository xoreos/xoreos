/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/optionsvideo.cpp
 *  The video options menu.
 */

#include "engines/nwn/menu/optionsvideo.h"

#include "engines/aurora/util.h"

namespace Engines {

namespace NWN {

OptionsVideoMenu::OptionsVideoMenu() : GUI("options_video") {
	// TODO: Resolution
	getWidget("VideoModeButton", true)->setDisabled(true);

	// TODO: Video quality
	getWidget("VidQualSlider", true)->setDisabled(true);
	getWidget("TextureSlider", true)->setDisabled(true);
	getWidget("ApplyButton"  , true)->setDisabled(true);

	// TODO: Sky boxes
	getWidget("SkyboxBox", true)->setDisabled(true);

	// TODO: Environment shadows
	getWidget("EnvShadowBox", true)->setDisabled(true);

	// TODO: Creature shadows
	getWidget("ShadowSlider", true)->setDisabled(true);

	// TODO: Gamma
	getWidget("GammaSlider"     , true)->setDisabled(true);
	getWidget("GammaResetButton", true)->setDisabled(true);

	// TODO: Advanced video settings
	getWidget("AdvVideoButton", true)->setDisabled(true);
}

OptionsVideoMenu::~OptionsVideoMenu() {
}

void OptionsVideoMenu::callbackActive(Widget &widget) {
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
