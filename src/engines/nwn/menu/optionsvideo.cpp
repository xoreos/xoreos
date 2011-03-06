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

#include "common/util.h"
#include "common/configman.h"

#include "aurora/talkman.h"

#include "graphics/graphics.h"

#include "engines/nwn/menu/optionsvideo.h"
#include "engines/nwn/menu/optionsresolution.h"
#include "engines/nwn/menu/optionsvideoadv.h"

#include "engines/aurora/util.h"

namespace Engines {

namespace NWN {

OptionsVideoMenu::OptionsVideoMenu(bool isMain) {
	load("options_video");

	if (isMain) {
		WidgetPanel *backdrop = new WidgetPanel(*this, "PNL_MAINMENU", "pnl_mainmenu");
		backdrop->setPosition(0.0, 0.0, -10.0);
		addWidget(backdrop);
	}

	// TODO: Video quality
	getWidget("VidQualSlider", true)->setDisabled(true);

	// TODO: Sky boxes
	Widget *skyBox = getWidget("SkyboxBox");
	if (skyBox)
		skyBox->setDisabled(true);

	// TODO: Environment shadows
	getWidget("EnvShadowBox", true)->setDisabled(true);

	// TODO: Creature shadows
	getWidget("ShadowSlider", true)->setDisabled(true);

	_resolution = new OptionsResolutionMenu(isMain);
	_advanced   = new OptionsVideoAdvancedMenu(isMain);
}

OptionsVideoMenu::~OptionsVideoMenu() {
	delete _advanced;
	delete _resolution;
}

void OptionsVideoMenu::show() {
	_gamma = GfxMan.getGamma();

	if (_gamma == 0.0) {
		getWidget("GammaSlider"     , true)->setDisabled(true);
		getWidget("GammaResetButton", true)->setDisabled(true);
	} else {
		getWidget("GammaSlider"     , true)->setDisabled(false);
		getWidget("GammaResetButton", true)->setDisabled(false);
	}

	int gammaValue = CLIP(_gamma - 0.1f, 0.0f, 1.9f) * 10;

	getSlider("GammaSlider", true)->setState(gammaValue);

	_textureLevel = ConfigMan.getInt("texturepack", 1);
	getSlider("TextureSlider", true)->setState(_textureLevel);

	updateTextureQualityLabel();

	GUI::show();
}

void OptionsVideoMenu::initWidget(Widget &widget) {
	if (widget.getTag() == "GammaSlider") {
		dynamic_cast<WidgetSlider &>(widget).setSteps(19);
		return;
	}

	if (widget.getTag() == "TextureSlider") {
		dynamic_cast<WidgetSlider &>(widget).setSteps(3);
		return;
	}
}

void OptionsVideoMenu::callbackActive(Widget &widget) {
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

	if (widget.getTag() == "VideoModeButton") {
		sub(*_resolution);
		return;
	}

	if (widget.getTag() == "AdvVideoButton") {
		sub(*_advanced);
		return;
	}

	if (widget.getTag() == "GammaSlider") {
		float gamma = dynamic_cast<WidgetSlider &>(widget).getState() / 10.0;

		GfxMan.setGamma(gamma + 0.1);
		return;
	}

	if (widget.getTag() == "GammaResetButton") {
		GfxMan.setGamma(_gamma);

		int gammaValue = CLIP(_gamma - 0.1f, 0.0f, 1.9f) * 10;

		getSlider("GammaSlider", true)->setState(gammaValue);
		return;
	}

	if (widget.getTag() == "TextureSlider") {
		_textureLevel = dynamic_cast<WidgetSlider &>(widget).getState();

		updateTextureQualityLabel();
		return;
	}

	if (widget.getTag() == "ApplyButton") {
		ConfigMan.setInt("texturepack", _textureLevel);
		return;
	}

}

void OptionsVideoMenu::updateTextureQualityLabel() {
	getLabel("TextureQualDesc", true)->setText(TalkMan.getString(7031 + _textureLevel));
}

void OptionsVideoMenu::adoptChanges() {
	ConfigMan.setDouble("gamma", GfxMan.getGamma(), true);
	ConfigMan.setInt("texturepack", _textureLevel);
}

void OptionsVideoMenu::revertChanges() {
	GfxMan.setGamma(_gamma);
}

} // End of namespace NWN

} // End of namespace Engines
