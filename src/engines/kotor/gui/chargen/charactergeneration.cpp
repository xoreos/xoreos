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
 *  The primary character generation menu.
 */

#include "glm/mat4x4.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "src/common/util.h"

#include "src/aurora/talkman.h"

#include "src/engines/aurora/kotorjadegui/label.h"

#include "src/engines/kotor/gui/chargen/charactergeneration.h"
#include "src/engines/kotor/gui/chargen/quickorcustom.h"
#include "src/engines/kotor/gui/chargen/quickchar.h"
#include "src/engines/kotor/gui/chargen/customchar.h"
#include "src/engines/kotor/gui/chargen/chargenportrait.h"
#include "src/engines/kotor/gui/chargen/chargenname.h"

namespace Engines {

namespace KotOR {

CharacterGenerationMenu::CharacterGenerationMenu(Module *module, CharacterGenerationInfo *pc, Console *console) :
		GUI(console), _module(module), _pc(pc), _step(0), _charSubScene(new Graphics::Aurora::SubSceneQuad) {

	load("maincg");

	addBackground(kBackgroundTypeMenu);

	static const char * const kEmptyLabels[] = {
		"VIT_ARROW_LBL", "DEF_ARROW_LBL", "LBL_NAME",
		"WILL_ARROW_LBL", "REFL_ARROW_LBL", "FORT_ARROW_LBL",
		"VIT_ARROW_LBL", "DEF_ARROW_LBL", "LBL_NAME"
	};

	for (size_t i = 0; i < ARRAYSIZE(kEmptyLabels); i++) {
		WidgetLabel *label = getLabel(kEmptyLabels[i]);
		if (label)
			label->setText("");
	}

	static const char * const kInvisibleWidgets[] = {
		"NEW_LBL", "OLD_LBL", "LBL_LEVEL", "LBL_LEVEL_VAL"
	};

	for (size_t i = 0; i < ARRAYSIZE(kInvisibleWidgets); i++) {
		Widget *widget = getWidget(kInvisibleWidgets[i]);
		if (widget)
			widget->setInvisible(true);
	}

	WidgetLabel *lblClass = getLabel("LBL_CLASS");
	if (lblClass) {
		// Set the class title according to the class of the character
		switch (pc->getClass()) {
			case kClassSoldier:
				lblClass->setText(TalkMan.getString(134));
				break;
			case kClassScout:
				lblClass->setText(TalkMan.getString(133));
				break;
			case kClassScoundrel:
				lblClass->setText(TalkMan.getString(135));
				break;
			default:
				lblClass->setText("");
		}
	}

	WidgetLabel *lblPortrait = getLabel("PORTRAIT_LBL");
	if (lblPortrait)
		lblPortrait->setFill(_pc->getPortrait());

	WidgetLabel *lblModel = getLabel("MODEL_LBL");

	float subSceneWidth  = lblModel ? lblModel->getWidth()  : 1.0f;
	float subSceneHeight = lblModel ? lblModel->getHeight() : 1.0f;

	glm::mat4 projection(glm::perspective(Common::deg2rad(22.72f), subSceneWidth/subSceneHeight, 0.1f, 10.0f));

	/*
	 * TODO: These values are extracted using apitrace from the original game.
	 * They should be replaced with a more straight forward matrix transformation expression.
	 */
	float modelView[] = {
			0.000900542, 0.0176735, 0.999843, 0,
			0.999999, 0.000900542, -0.00091657, 0,
			-0.00091657, 0.999843, -0.0176727, 0,
			0, 0, 0, 1
	};

	glm::mat4 transformation(glm::make_mat4(modelView));

	transformation = glm::translate(transformation, glm::vec3(-4.87251f, 0.0f, -0.776371f));

	transformation = glm::rotate(transformation,
			Common::deg2rad(-90.0f),
			glm::vec3(0.0f, 0.0f, 1.0f));

	if (lblModel) {
		lblModel->setSubScene(_charSubScene.get());

		if (_pc->getModel())
			_charSubScene->add(_pc->getModel());

		_charSubScene->setProjectionMatrix(projection);
		_charSubScene->setGlobalTransformationMatrix(transformation);
	}

	showQuickOrCustom();
}

CharacterGenerationMenu::~CharacterGenerationMenu() {
}

void CharacterGenerationMenu::showQuickOrCustom() {
	if (_quickChar)
		removeChild(_quickChar.get());
	if (_customChar)
		removeChild(_customChar.get());

	_step = 0;

	_quickOrCustom.reset(new QuickOrCustomPanel(this));
	addChild(_quickOrCustom.get());
}

void CharacterGenerationMenu::showQuick() {
	if (_quickOrCustom)
		removeChild(_quickOrCustom.get());
	if (_customChar)
		removeChild(_customChar.get());

	_quickChar.reset(new QuickCharPanel(this));
	addChild(_quickChar.get());
}

void CharacterGenerationMenu::showCustom() {
	if (_quickOrCustom)
		removeChild(_quickOrCustom.get());
	if (_quickChar)
		removeChild(_quickChar.get());

	_customChar.reset(new CustomCharPanel(this));
	addChild(_customChar.get());
}

void CharacterGenerationMenu::showPortrait() {
	// Operate on a copy of the character object
	CharacterGenerationInfo info = *_pc;

	_charGenMenu.reset(new CharacterGenerationPortraitMenu(info));

	sub(*_charGenMenu);
	if (_charGenMenu->isAccepted()) {
		*_pc = info;

		WidgetLabel *lblPortrait = getLabel("PORTRAIT_LBL");
		if (lblPortrait)
			lblPortrait->setFill(_pc->getPortrait());

		_pc->recreateHead();

		_step += 1;
	}
}

void CharacterGenerationMenu::showName() {
	// Operate on a copy of the character object
	CharacterGenerationInfo info = *_pc;

	_charGenMenu.reset(new CharacterGenerationNameMenu(info));

	sub(*_charGenMenu);
	if (_charGenMenu->isAccepted()) {
		*_pc = info;

		WidgetLabel *lblName = getLabel("LBL_NAME");
		if (lblName)
			lblName->setText(info.getName());

		_step += 1;
	}
}

int CharacterGenerationMenu::getStep() {
	return _step;
}

void CharacterGenerationMenu::decStep() {
	_step = MIN(0, _step - 1);
}

void CharacterGenerationMenu::start() {
	hide();

	try {
		_module->usePC(_pc->getCharacter());
		_module->load("end_m01aa");
	} catch (...) {
		Common::exceptionDispatcherWarning();
		return;
	}

	show();
}

void CharacterGenerationMenu::callbackKeyInput(const Events::Key &key, const Events::EventType &type) {
	if (getWidget("DEF_ARROW_LBL"))
		return;

	if (key != Events::kKeyReturn || type != Events::kEventKeyUp)
		return;

	start();
	GfxMan.lockFrame();
	_returnCode = 2;
}

} // End of namespace KotOR

} // End of namespace Engines
