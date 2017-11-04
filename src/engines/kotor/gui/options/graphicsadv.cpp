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

#include "src/aurora/talkman.h"

#include "src/engines/aurora/widget.h"

#include "src/engines/kotor/gui/options/graphicsadv.h"

#include "src/engines/kotor/gui/widgets/button.h"
#include "src/engines/kotor/gui/widgets/checkbox.h"

namespace Engines {

namespace KotOR {

OptionsGraphicsAdvancedMenu::OptionsGraphicsAdvancedMenu(::Engines::Console *console) : GUI(console) {
	load("optgraphicsadv");

	//Hardcoded, the gui file returns incorrect values
	getButton("BTN_TEXQUALLEFT", true)->setColor(0.0f, 0.658824f, 0.980392f, 1.0f);
	getButton("BTN_TEXQUALLEFT", true)->setStaticHighlight();
	getButton("BTN_TEXQUALRIGHT", true)->setColor(0.0f, 0.658824f, 0.980392f, 1.0f);
	getButton("BTN_TEXQUALRIGHT", true)->setStaticHighlight();
	getButton("BTN_ANTIALIASLEFT", true)->setColor(0.0f, 0.658824f, 0.980392f, 1.0f);
	getButton("BTN_ANTIALIASLEFT", true)->setStaticHighlight();
	getButton("BTN_ANTIALIASRIGHT", true)->setColor(0.0f, 0.658824f, 0.980392f, 1.0f);
	getButton("BTN_ANTIALIASRIGHT", true)->setStaticHighlight();
	getButton("BTN_ANISOTROPYLEFT", true)->setColor(0.0f, 0.658824f, 0.980392f, 1.0f);
	getButton("BTN_ANISOTROPYLEFT", true)->setStaticHighlight();
	getButton("BTN_ANISOTROPYRIGHT", true)->setColor(0.0f, 0.658824f, 0.980392f, 1.0f);
	getButton("BTN_ANISOTROPYRIGHT", true)->setStaticHighlight();
	getCheckBox("CB_FRAMEBUFF", true)->setColor(0.0f, 0.658824f, 0.980392f, 1.0f);
	getCheckBox("CB_VSYNC", true)->setColor(0.0f, 0.658824f, 0.980392f, 1.0f);
	getCheckBox("CB_SOFTSHADOWS", true)->setColor(0.0f, 0.658824f, 0.980392f, 1.0f);

}

OptionsGraphicsAdvancedMenu::~OptionsGraphicsAdvancedMenu() {

}

void OptionsGraphicsAdvancedMenu::show() {
	GUI::show();

	_textureQuality = CLIP(ConfigMan.getInt("texturequality", 0), 0, 2);
	updateTextureQuality(_textureQuality);

	_antiAliasing = CLIP(ConfigMan.getInt("antialiasing", 0), 0, 3);
	updateAntiAliasing(_antiAliasing);

	_anisotropy = CLIP(ConfigMan.getInt("anisotropy", 0), 0, 4);
	updateAnisotropy(_anisotropy);

	_frameBufferEffects = ConfigMan.getBool("framebuffereffects", true);
	setCheckBoxState("CB_FRAMEBUFF", _frameBufferEffects);

	_softShadows = ConfigMan.getBool("softshadows", true);
	setCheckBoxState("CB_SOFTSHADOWS", _softShadows);

	_vsync = ConfigMan.getBool("vsync", false);
	setCheckBoxState("CB_VSYNC", _vsync);
}

void OptionsGraphicsAdvancedMenu::callbackActive(Widget &widget) {

	if (widget.getTag() == "BTN_TEXQUALRIGHT") {
		_textureQuality++;
		if (_textureQuality > 2) {
			_textureQuality = 2;
		}
		updateTextureQuality(_textureQuality);
		return;
	}

	if (widget.getTag() == "BTN_TEXQUALLEFT") {
		_textureQuality--;
		if (_textureQuality < 0) {
			_textureQuality = 0;
		}
		updateTextureQuality(_textureQuality);
		return;
	}

	if (widget.getTag() == "BTN_ANTIALIASRIGHT") {
		_antiAliasing++;
		if (_antiAliasing > 3) {
			_antiAliasing = 3;
		}
		updateAntiAliasing(_antiAliasing);
		return;
	}

	if (widget.getTag() == "BTN_ANTIALIASLEFT") {
		_antiAliasing--;
		if (_antiAliasing < 0) {
			_antiAliasing = 0;
		}
		updateAntiAliasing(_antiAliasing);
		return;
	}

	if (widget.getTag() == "BTN_ANISOTROPYRIGHT") {
		_anisotropy++;
		if (_anisotropy > 4) {
			_anisotropy = 4;
		}
		updateAnisotropy(_anisotropy);
		return;
	}

	if (widget.getTag() == "BTN_ANISOTROPYLEFT") {
		_anisotropy--;
		if (_anisotropy < 0) {
			_anisotropy = 0;
		}
		updateAnisotropy(_anisotropy);
		return;
	}

	if (widget.getTag() == "BTN_DEFAULT") {
		_textureQuality = 0;
		updateTextureQuality(_textureQuality);

		_antiAliasing = 0;
		updateAntiAliasing(_antiAliasing);

		_anisotropy = 0;
		updateAnisotropy(_anisotropy);

		_frameBufferEffects = true;
		setCheckBoxState("CB_FRAMEBUFF", _frameBufferEffects);

		_softShadows = true;
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

void OptionsGraphicsAdvancedMenu::updateTextureQuality(int textureQuality) {
	WidgetButton &texQualButton = *getButton("BTN_TEXQUAL", true);
	WidgetButton &leftButton = *getButton("BTN_TEXQUALLEFT", true);
	WidgetButton &rightButton = *getButton("BTN_TEXQUALRIGHT", true);

	texQualButton.setText(TalkMan.getString(48003 + textureQuality));

	if (_textureQuality == 0) {
		leftButton.hide();
	} else {
		leftButton.show();
	}

	if (_textureQuality == 2) {
		rightButton.hide();
	} else {
		rightButton.show();
	}
}

void OptionsGraphicsAdvancedMenu::updateAntiAliasing(int antiAliasing) {
	WidgetButton &antiAliasButton = *getButton("BTN_ANTIALIAS", true);
	WidgetButton &leftButton = *getButton("BTN_ANTIALIASLEFT", true);
	WidgetButton &rightButton = *getButton("BTN_ANTIALIASRIGHT", true);

	Common::UString text;

	if (antiAliasing == 0)
		text = TalkMan.getString(47996);
	else if (antiAliasing == 1)
		text = TalkMan.getString(47997);
	else if (antiAliasing == 2)
		text = TalkMan.getString(47999);
	else if (antiAliasing == 3)
		text = TalkMan.getString(49125);

	antiAliasButton.setText(text);

	if (_antiAliasing == 0) {
		leftButton.hide();
	} else {
		leftButton.show();
	}

	if (_antiAliasing == 3) {
		rightButton.hide();
	} else {
		rightButton.show();
	}
}

void OptionsGraphicsAdvancedMenu::updateAnisotropy(int anisotropy) {
	WidgetButton &anisotropyButton = *getButton("BTN_ANISOTROPY", true);
	WidgetButton &leftButton = *getButton("BTN_ANISOTROPYLEFT", true);
	WidgetButton &rightButton = *getButton("BTN_ANISOTROPYRIGHT", true);

	anisotropyButton.setText(TalkMan.getString(49079 + anisotropy));

	if (_anisotropy == 0) {
		leftButton.hide();
	} else {
		leftButton.show();
	}

	if (_anisotropy == 4) {
		rightButton.hide();
	} else {
		rightButton.show();
	}
}

void OptionsGraphicsAdvancedMenu::adoptChanges() {
	ConfigMan.setInt("texturequality", _textureQuality, true);
	ConfigMan.setInt("antialiasing", _antiAliasing, true);
	ConfigMan.setInt("anisotropy", _anisotropy, true);
	ConfigMan.setBool("framebuffereffects", _softShadows, true);
	ConfigMan.setBool("softshadows", _softShadows, true);
	ConfigMan.setBool("vsync", _vsync, true);
}

} // End of namespace KotOR

} // End of namespace Engines



