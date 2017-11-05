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

#include "src/common/util.h"

#include "src/aurora/talkman.h"

#include "src/engines/kotor/gui/widgets/label.h"

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

	getLabel("VIT_ARROW_LBL")->setText("");
	getLabel("DEF_ARROW_LBL")->setText("");

	getLabel("LBL_NAME")->setText("");

	// Set the class title according to the class of the character
	switch (pc->getClass()) {
		case kClassSoldier:
			getLabel("LBL_CLASS")->setText(TalkMan.getString(134));
			break;
		case kClassScout:
			getLabel("LBL_CLASS")->setText(TalkMan.getString(133));
			break;
		case kClassScoundrel:
			getLabel("LBL_CLASS")->setText(TalkMan.getString(135));
			break;
		default:
			getLabel("LBL_CLASS")->setText("");
	}

	getLabel("WILL_ARROW_LBL")->setText("");
	getLabel("REFL_ARROW_LBL")->setText("");
	getLabel("FORT_ARROW_LBL")->setText("");

	getLabel("PORTRAIT_LBL")->setFill(_pc->getPortrait());

	getWidget("NEW_LBL")->setInvisible(true);
	getWidget("OLD_LBL")->setInvisible(true);

	getWidget("LBL_LEVEL_VAL")->setInvisible(true);
	getWidget("LBL_LEVEL")->setInvisible(true);

	float subSceneWidth = getLabel("MODEL_LBL")->getWidth();
	float subSceneHeight = getLabel("MODEL_LBL")->getHeight();

	Common::Matrix4x4 projection;
	projection.perspective(22.72f, subSceneWidth/subSceneHeight, 0.1f, 10.0f);

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
	Common::Matrix4x4 transformation(modelView);
	transformation.translate(-4.87251f, 0.0f, -0.776371f);
	transformation.rotate(-90.0f, 0.0f, 0.0f, 1.0f);

	getLabel("MODEL_LBL")->setSubScene(_charSubScene.get());
	_charSubScene->add(_pc->getModel());
	_charSubScene->setProjectionMatrix(projection);
	_charSubScene->setGlobalTransformationMatrix(transformation);

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
		getLabel("PORTRAIT_LBL")->setFill(_pc->getPortrait());
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
		getLabel("LBL_NAME")->setText(info.getName());
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
	try {
		_module->usePC(_pc->getCharacter());
		_module->load("end_m01aa");
	} catch (...) {
		Common::exceptionDispatcherWarning();
		return;
	}
}

} // End of namespace KotOR

} // End of namespace Engines
