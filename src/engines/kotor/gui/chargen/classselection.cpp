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
 *  The class selection menu.
 */

#include "src/aurora/talkman.h"

#include "src/engines/kotor/creature.h"

#include "src/engines/kotor/gui/widgets/kotorwidget.h"

#include "src/engines/kotor/gui/chargen/classselection.h"
#include "src/engines/kotor/gui/chargen/charactergeneration.h"

namespace Engines {

namespace KotOR {

ClassSelectionMenu::ClassSelectionMenu(Module *module, ::Engines::Console *console) : GUI(console),
	_hoveredButton(0), _module(module) {

	load("classsel");

	addBackground(kBackgroundTypeMenu);

	// Get the six class buttons
	_maleScoundrelButton = getButton("BTN_SEL1");
	_maleScoutButton = getButton("BTN_SEL2");
	_maleSoldierButton = getButton("BTN_SEL3");
	_femaleSoldierButton = getButton("BTN_SEL4");
	_femaleScoutButton = getButton("BTN_SEL5");
	_femaleScoundrelButton = getButton("BTN_SEL6");

	// Get the description label
	_labelDesc = getLabel("LBL_DESC");
	_labelDesc->setWrapped(true);
	_labelDesc->setText(TalkMan.getString(32111));

	// Get the title label
	_labelTitle = getLabel("LBL_CLASS");
	_labelTitle->setText("");

	// Get the class descriptions
	_soldierDesc = TalkMan.getString(32111);
	_scoutDesc = TalkMan.getString(32110);
	_scoundrelDesc = TalkMan.getString(32109);

	// Combine the titles with gender prefix and class name
	Common::UString malePrefix = TalkMan.getString(358);
	Common::UString femalePrefix = TalkMan.getString(359);
	_soldierMaleTitle = malePrefix + " " + TalkMan.getString(134);
	_soldierFemaleTitle = femalePrefix + " " + TalkMan.getString(134);
	_scoutMaleTitle = malePrefix + " " + TalkMan.getString(133);
	_scoutFemaleTitle = femalePrefix + " " + TalkMan.getString(133);
	_scoundrelMaleTitle = malePrefix + " " + TalkMan.getString(135);
	_scoundrelFemaleTitle = femalePrefix + " " + TalkMan.getString(135);

	// Create the random characters
	_maleSoldier = CharacterGenerationInfo::createRandomMaleSoldier();
	_maleScout = CharacterGenerationInfo::createRandomMaleScout();
	_maleScoundrel = CharacterGenerationInfo::createRandomMaleScoundrel();
	_femaleSoldier = CharacterGenerationInfo::createRandomFemaleSoldier();
	_femaleScout = CharacterGenerationInfo::createRandomFemaleScout();
	_femaleScoundrel = CharacterGenerationInfo::createRandomFemaleScoundrel();

	float subSceneWidth = getLabel("3D_MODEL2")->getWidth();
	float subSceneHeight = getLabel("3D_MODEL2")->getHeight();

	Common::Matrix4x4 projection;
	projection.perspective(22.72f, subSceneWidth/subSceneHeight, 0.1f, 10.0f);

	/*
	 * TODO: These values are extracted using apitrace from the original game.
	 * They should be replaced with a more straight forward matrix transformation expression.
	 */
	float modelView[] = {
			0.00979373, -0.040304, 0.999139, 0,
			0.999952, 0.000393929, -0.00978577, 0,
			8.34465e-7, 0.999187, 0.0403059, 0,
			0, 0, 0, 1
	};
	Common::Matrix4x4 transformation(modelView);
	transformation.translate(-4.87294f, 0.0880559f, -1.06834f);
	transformation.rotate(-90.0f, 0.0f, 0.0f, 1.0f);

	setupClassSubScene("3D_MODEL1", _maleScoundrelSubScene, _maleScoundrel->getModel(),
	                   projection, transformation);
	setupClassSubScene("3D_MODEL2", _maleScoutSubScene, _maleScout->getModel(),
	                   projection, transformation);
	setupClassSubScene("3D_MODEL3", _maleSoldierSubScene, _maleSoldier->getModel(),
	                   projection, transformation);
	setupClassSubScene("3D_MODEL4", _femaleSoldierSubScene, _femaleSoldier->getModel(),
	                   projection, transformation);
	setupClassSubScene("3D_MODEL5", _femaleScoutSubScene, _femaleScout->getModel(),
	                   projection, transformation);
	setupClassSubScene("3D_MODEL6", _femaleScoundrelSubScene, _femaleScoundrel->getModel(),
	                   projection, transformation);
}

ClassSelectionMenu::~ClassSelectionMenu() {
	delete _maleSoldier;
	delete _maleScout;
	delete _maleScoundrel;
	delete _femaleSoldier;
	delete _femaleScout;
	delete _femaleScoundrel;
}

void ClassSelectionMenu::createCharacterGeneration(CharacterGenerationInfo* info) {
	_charGen.reset(new CharacterGenerationMenu(_module, info));
}

void ClassSelectionMenu::callbackRun() {
	// Check if a specific button is hovered and set title and description
	if (_maleSoldierButton->isHovered() && _hoveredButton != _maleSoldierButton) {
		_labelDesc->setText(_soldierDesc);
		_labelTitle->setText(_soldierMaleTitle);
		_hoveredButton = _maleSoldierButton;
		return;
	}
	if (_femaleSoldierButton->isHovered() && _hoveredButton != _femaleSoldierButton) {
		_labelDesc->setText(_soldierDesc);
		_labelTitle->setText(_soldierFemaleTitle);
		_hoveredButton = _femaleSoldierButton;
		return;
	}
	if (_maleScoutButton->isHovered() && _hoveredButton != _maleScoutButton) {
		_labelDesc->setText(_scoutDesc);
		_labelTitle->setText(_scoutMaleTitle);
		_hoveredButton = _maleScoutButton;
		return;
	}
	if (_femaleScoutButton->isHovered() && _hoveredButton != _femaleScoutButton) {
		_labelDesc->setText(_scoutDesc);
		_labelTitle->setText(_scoutFemaleTitle);
		_hoveredButton = _femaleScoutButton;
		return;
	}
	if (_maleScoundrelButton->isHovered() && _hoveredButton != _maleScoundrelButton) {
		_labelDesc->setText(_scoundrelDesc);
		_labelTitle->setText(_scoundrelMaleTitle);
		_hoveredButton = _maleScoundrelButton;
		return;
	}
	if (_femaleScoundrelButton->isHovered() && _hoveredButton != _femaleScoundrelButton) {
		_labelDesc->setText(_scoundrelDesc);
		_labelTitle->setText(_scoundrelFemaleTitle);
		_hoveredButton = _femaleScoundrelButton;
		return;
	}
}

void ClassSelectionMenu::callbackActive(Widget &widget) {
	// Return to the main menu
	if (widget.getTag() == "BTN_BACK") {
		_returnCode = 1;
		return;
	}

	// Start the character generation with
	if (widget.getTag() == "BTN_SEL1") {
		createCharacterGeneration(_maleScoundrel);
		if (sub(*_charGen) == 2) {
			_returnCode = 2;
		}
	}
	if (widget.getTag() == "BTN_SEL2") {
		createCharacterGeneration(_maleScout);
		if (sub(*_charGen) == 2) {
			_returnCode = 2;
		}
	}
	if (widget.getTag() == "BTN_SEL3") {
		createCharacterGeneration(_maleSoldier);
		if (sub(*_charGen) == 2) {
			_returnCode = 2;
		}
	}
	if (widget.getTag() == "BTN_SEL4") {
		createCharacterGeneration(_femaleSoldier);
		if (sub(*_charGen) == 2) {
			_returnCode = 2;
		}
	}
	if (widget.getTag() == "BTN_SEL5") {
		createCharacterGeneration(_femaleScout);
		if (sub(*_charGen) == 2) {
			_returnCode = 2;
		}
	}
	if (widget.getTag() == "BTN_SEL6") {
		createCharacterGeneration(_femaleScoundrel);
		if (sub(*_charGen) == 2) {
			_returnCode = 2;
		}
	}
}

void ClassSelectionMenu::setupClassSubScene(const Common::UString &widgetName,
		Graphics::Aurora::SubSceneQuad &subScene, Graphics::Aurora::Model *model,
		Common::Matrix4x4 &projection, Common::Matrix4x4 &transformation) {
	getLabel(widgetName)->setSubScene(&subScene);
	// TODO: Should randomly switch between pause1, pause2 and pause3
	model->playAnimation("pause1", true, -1);
	subScene.add(model);
	subScene.setProjectionMatrix(projection);
	subScene.setGlobalTransformationMatrix(transformation);
}

} // End of namespace KotOR

} // End of namespace Engines
