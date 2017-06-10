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

#include "src/engines/kotor/gui/chargen/classselection.h"
#include "src/engines/kotor/gui/widgets/kotorwidget.h"
#include "src/engines/kotor/gui/chargen/charactergeneration.h"

namespace Engines {

namespace KotOR {

ClassSelectionMenu::ClassSelectionMenu(Module *module, ::Engines::Console *console) : GUI(console),
	_module(module) {

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
	Common::UString malePrefix, femalePrefix;
	malePrefix = TalkMan.getString(358);
	femalePrefix = TalkMan.getString(359);
	_soldierMaleTitle = malePrefix + " " + TalkMan.getString(134);
	_soldierFemaleTitle = femalePrefix + " " + TalkMan.getString(134);
	_scoutMaleTitle = malePrefix + " " + TalkMan.getString(133);
	_scoutFemaleTitle = femalePrefix + " " + TalkMan.getString(133);
	_scoundrelMaleTitle = malePrefix + " " + TalkMan.getString(135);
	_scoundrelFemaleTitle = femalePrefix + " " + TalkMan.getString(135);
}

void ClassSelectionMenu::createCharacterGeneration() {
	if(_charGen)
		return;

	_charGen.reset(new CharacterGenerationMenu(_module));
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

	// start the character generation with
	if (widget.getTag() == "BTN_SEL1") {
		createCharacterGeneration();
		sub(*_charGen);
	}
	if (widget.getTag() == "BTN_SEL2") {
		createCharacterGeneration();
		sub(*_charGen);
	}
	if (widget.getTag() == "BTN_SEL3") {
		createCharacterGeneration();
		sub(*_charGen);
	}
	if (widget.getTag() == "BTN_SEL4") {
		createCharacterGeneration();
		sub(*_charGen);
	}
	if (widget.getTag() == "BTN_SEL5") {
		createCharacterGeneration();
		sub(*_charGen);
	}
	if (widget.getTag() == "BTN_SEL6") {
		createCharacterGeneration();
		sub(*_charGen);
	}
}

} // End of namespace KotOR

} // End of namespace Engines
