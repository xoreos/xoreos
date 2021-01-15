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

#include <memory>

#include "external/glm/mat4x4.hpp"
#include "external/glm/gtc/type_ptr.hpp"
#include "external/glm/gtc/matrix_transform.hpp"

#include "src/common/util.h"
#include "src/common/strutil.h"

#include "src/aurora/talkman.h"

#include "src/graphics/aurora/subscenequad.h"
#include "src/graphics/aurora/model.h"

#include "src/engines/odyssey/label.h"

#include "src/engines/kotor/creature.h"

#include "src/engines/kotor/gui/chargen/charactergeneration.h"
#include "src/engines/kotor/gui/chargen/quickorcustom.h"
#include "src/engines/kotor/gui/chargen/quickchar.h"
#include "src/engines/kotor/gui/chargen/customchar.h"
#include "src/engines/kotor/gui/chargen/chargenportrait.h"
#include "src/engines/kotor/gui/chargen/chargenname.h"
#include "src/engines/kotor/gui/chargen/chargenabilities.h"
#include "src/engines/kotor/gui/chargen/chargenskills.h"
#include "src/engines/kotor/gui/chargen/chargenfeats.h"

namespace Engines {

namespace KotOR {

CharacterGenerationMenu::CharacterGenerationMenu(KotORBase::Module *module,
                                                 CharacterGenerationInfo *pc,
                                                 Console *console) :
		KotORBase::GUI(console),
		_module(module),
		_pc(pc),
		_step(0),
		_charSubScene(new Graphics::Aurora::SubSceneQuad) {

	load("maincg");

	addBackground(KotORBase::kBackgroundTypeMenu);

	static const char * const kEmptyLabels[] = {
		"VIT_ARROW_LBL", "DEF_ARROW_LBL", "LBL_NAME",
		"WILL_ARROW_LBL", "REFL_ARROW_LBL", "FORT_ARROW_LBL",
		"VIT_ARROW_LBL", "DEF_ARROW_LBL", "LBL_NAME"
	};

	for (size_t i = 0; i < ARRAYSIZE(kEmptyLabels); i++) {
		Odyssey::WidgetLabel *label = getLabel(kEmptyLabels[i]);
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

	Odyssey::WidgetLabel *lblClass = getLabel("LBL_CLASS");
	if (lblClass) {
		// Set the class title according to the class of the character
		switch (pc->getClass()) {
			case KotORBase::kClassSoldier:
				lblClass->setText(TalkMan.getString(134));
				break;
			case KotORBase::kClassScout:
				lblClass->setText(TalkMan.getString(133));
				break;
			case KotORBase::kClassScoundrel:
				lblClass->setText(TalkMan.getString(135));
				break;
			default:
				lblClass->setText("");
		}
	}

	Odyssey::WidgetLabel *lblPortrait = getLabel("PORTRAIT_LBL");
	if (lblPortrait)
		lblPortrait->setFill(_pc->getPortrait());

	Odyssey::WidgetLabel *lblModel = getLabel("MODEL_LBL");

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

		_pcModel.reset(Creature::createModel(_pc));
		if (_pcModel) {
			_pcModel->playAnimation("pause1", true, -1);
			_charSubScene->add(_pcModel.get());
		}

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

	_quickOrCustom = std::make_unique<QuickOrCustomPanel>(this);
	addChild(_quickOrCustom.get());
}

void CharacterGenerationMenu::showQuick() {
	if (_quickOrCustom)
		removeChild(_quickOrCustom.get());
	if (_customChar)
		removeChild(_customChar.get());

	_quickChar = std::make_unique<QuickCharPanel>(this);
	addChild(_quickChar.get());
}

void CharacterGenerationMenu::showCustom() {
	if (_quickOrCustom)
		removeChild(_quickOrCustom.get());
	if (_quickChar)
		removeChild(_quickChar.get());

	_customChar = std::make_unique<CustomCharPanel>(this);
	addChild(_customChar.get());
}

void CharacterGenerationMenu::showPortrait() {
	// Operate on a copy of the character object
	CharacterGenerationInfo info = *_pc;

	_charGenMenu = std::make_unique<CharacterGenerationPortraitMenu>(info);

	sub(*_charGenMenu);
	if (_charGenMenu->isAccepted()) {
		*_pc = info;

		Odyssey::WidgetLabel *lblPortrait = getLabel("PORTRAIT_LBL");
		if (lblPortrait)
			lblPortrait->setFill(_pc->getPortrait());

		std::unique_ptr<Graphics::Aurora::Model> head(Creature::createHeadModel(_pc));
		if (head) {
			GfxMan.lockFrame();
			_pcModel->attachModel("headhook", head.release());
			_pcModel->playAnimation("pause1", true, -1);
			GfxMan.unlockFrame();
		}

		_step += 1;
	}
}

void CharacterGenerationMenu::showName() {
	// Operate on a copy of the character object
	CharacterGenerationInfo info = *_pc;

	_charGenMenu = std::make_unique<CharacterGenerationNameMenu>(info);

	sub(*_charGenMenu);
	if (_charGenMenu->isAccepted()) {
		*_pc = info;

		Odyssey::WidgetLabel *lblName = getLabel("LBL_NAME");
		if (lblName)
			lblName->setText(info.getName());

		_step += 1;
	}
}

void CharacterGenerationMenu::showAbilities() {
	// Operate on a copy of the character object
	CharacterGenerationInfo info = *_pc;

	_charGenMenu = std::make_unique<CharacterGenerationAbilitiesMenu>(info);

	sub(*_charGenMenu);
	if (_charGenMenu->isAccepted()) {
		*_pc = info;

		Odyssey::WidgetLabel *lblStrength = getLabel("STR_AB_LBL");
		if (lblStrength)
			lblStrength->setText(Common::composeString(
						info.getAbilities().strength));
		Odyssey::WidgetLabel *lblDexterity = getLabel("DEX_AB_LBL");
		if (lblDexterity)
			lblDexterity->setText(Common::composeString(
						info.getAbilities().dexterity));
		Odyssey::WidgetLabel *lblConstitution = getLabel("CON_AB_LBL");
		if (lblConstitution)
			lblConstitution->setText(Common::composeString(
						info.getAbilities().constitution));
		Odyssey::WidgetLabel *lblWisdom = getLabel("WIS_AB_LBL");
		if (lblWisdom)
			lblWisdom->setText(Common::composeString(
						info.getAbilities().wisdom));
		Odyssey::WidgetLabel *lblIntelligence = getLabel("INT_AB_LBL");
		if (lblIntelligence)
			lblIntelligence->setText(Common::composeString(
						info.getAbilities().intelligence));
		Odyssey::WidgetLabel *lblCharisma = getLabel("CHA_AB_LBL");
		if (lblCharisma)
			lblCharisma->setText(Common::composeString(
						info.getAbilities().charisma));

		_step += 1;
	}
}

void CharacterGenerationMenu::showSkills() {
	// Operate on a copy of the character object
	CharacterGenerationInfo info = *_pc;

	_charGenMenu = std::make_unique<CharacterGenerationSkillsMenu>(info);

	sub(*_charGenMenu);
	if (_charGenMenu->isAccepted()) {
		*_pc = info;

		// TODO update display

		_step += 1;
	}
}

void CharacterGenerationMenu::showFeats() {
	// Operate on a copy of the character object
	CharacterGenerationInfo info = *_pc;

	_charGenMenu = std::make_unique<CharacterGenerationFeatsMenu>(info);

	sub(*_charGenMenu);
	if (_charGenMenu->isAccepted()) {
		*_pc = info;

		// TODO update display

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
		_module->usePC(*_pc);
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
