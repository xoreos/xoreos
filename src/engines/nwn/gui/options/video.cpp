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
 *  The NWN video options menu.
 */

#include "src/common/util.h"
#include "src/common/configman.h"

#include "src/aurora/talkman.h"

#include "src/graphics/graphics.h"

#include "src/engines/nwn/gui/widgets/panel.h"
#include "src/engines/nwn/gui/widgets/label.h"
#include "src/engines/nwn/gui/widgets/slider.h"

#include "src/engines/nwn/gui/options/video.h"
#include "src/engines/nwn/gui/options/resolution.h"
#include "src/engines/nwn/gui/options/videoadv.h"

namespace Engines {

namespace NWN {

OptionsVideoMenu::OptionsVideoMenu(bool isMain, ::Engines::Console *console) : GUI(console) {
	load("options_video");

	if (isMain) {
		WidgetPanel *backdrop = new WidgetPanel(*this, "PNL_MAINMENU", "pnl_mainmenu");
		backdrop->setPosition(0.0f, 0.0f, 100.0f);
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

	_resolution.reset(new OptionsResolutionMenu   (isMain, _console));
	_advanced.reset  (new OptionsVideoAdvancedMenu(isMain, _console));
}

OptionsVideoMenu::~OptionsVideoMenu() {
}

void OptionsVideoMenu::show() {
	_gamma = WindowMan.getGamma();

	if (_gamma == 0.0f) {
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
		float gamma = dynamic_cast<WidgetSlider &>(widget).getState() / 10.0f;

		WindowMan.setGamma(gamma + 0.1f);
		return;
	}

	if (widget.getTag() == "GammaResetButton") {
		WindowMan.setGamma(_gamma);

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
	ConfigMan.setDouble("gamma", WindowMan.getGamma(), true);
	ConfigMan.setInt("texturepack", _textureLevel);
}

void OptionsVideoMenu::revertChanges() {
	WindowMan.setGamma(_gamma);
}

} // End of namespace NWN

} // End of namespace Engines
