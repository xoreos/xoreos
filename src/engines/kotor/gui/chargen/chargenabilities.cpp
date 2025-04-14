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
 *  The menu for modifying the abilities of the character.
 */

#include "src/common/strutil.h"

#include "src/aurora/talkman.h"

#include "src/engines/odyssey/button.h"
#include "src/engines/odyssey/label.h"
#include "src/engines/odyssey/listbox.h"

#include "src/engines/kotor/gui/chargen/chargeninfo.h"

#include "src/engines/kotor/gui/chargen/chargenabilities.h"

#define AB_MIN_POINTS  8 // TODO what is the correct value? read from files?
#define AB_MAX_POINTS 18 // TODO what is the correct value? read from files?
#define REMAIN_POINTS 30 // TODO what is the correct value? read from files?

namespace Engines {

namespace KotOR {

CharacterGenerationAbilitiesMenu::CharacterGenerationAbilitiesMenu(KotORBase::CharacterGenerationInfo &info,
                                                                 Console *console) :
		CharacterGenerationBaseMenu(info, console) {

	load("abchrgen");

	addBackground(KotORBase::kBackgroundTypeMenu);

	_remainingSections = REMAIN_POINTS;

	_strength = AB_MIN_POINTS;
	_dexterity = AB_MIN_POINTS;
	_constitution = AB_MIN_POINTS;
	_wisdom = AB_MIN_POINTS;
	_intelligence = AB_MIN_POINTS;
	_charisma = AB_MIN_POINTS;

	_remainingSectionsLabel = getLabel("REMAINING_SELECTIONS_LBL");
	_costPointsLabel = getLabel("COST_POINTS_LBL");
	_abilityModLabel = getLabel("LBL_ABILITY_MOD");

	//_descLabel = getLabel("DESC_LBL");
	_descListBox = getListBox("LB_DESC", true);
	// TODO find out how this ListBox works
	// 1. Try: get protoitem directly - fail
	//_descProtoItem = getProtoItem("PROTOITEM", true);
	//_descLabelProtoItem = getLabel("PROTOITEM", true);
	// 2. Try: access protoitem - fail
       	//_descText = _descListBox->selectItemByWidgetTag("PROTOITEM");
	// 3. Try: add item with content (string)
	//_descListBox->addItem(_charismaDesc);
	//_descListBox->addItem("PROTOITEM");
	//
	//_descTextLabel->setWrapped(true);

	_strengthLabel = getLabel("STR_LBL");
	_dexterityLabel = getLabel("DEX_LBL");
	_constitutionLabel = getLabel("CON_LBL");
	_wisdomLabel = getLabel("WIS_LBL");
	_intelligenceLabel = getLabel("WIS_LBL");
	_charismaLabel = getLabel("CHA_LBL");

	_strengthButton = getButton("STR_POINTS_BTN");
	_dexterityButton = getButton("DEX_POINTS_BTN");
	_constitutionButton = getButton("CON_POINTS_BTN");
	_wisdomButton = getButton("WIS_POINTS_BTN");
	_intelligenceButton = getButton("INT_POINTS_BTN");
	_charismaButton = getButton("CHA_POINTS_BTN");

	_strengthPlusButton = getButton("STR_PLUS_BTN");
	_dexterityPlusButton = getButton("DEX_PLUS_BTN");
	_constitutionPlusButton = getButton("CON_PLUS_BTN");
	_wisdomPlusButton = getButton("WIS_PLUS_BTN");
	_intelligencePlusButton = getButton("INT_PLUS_BTN");
	_charismaPlusButton = getButton("CHA_PLUS_BTN");

	_strengthMinusButton = getButton("STR_MINUS_BTN");
	_dexterityMinusButton = getButton("DEX_MINUS_BTN");
	_constitutionMinusButton = getButton("CON_MINUS_BTN");
	_wisdomMinusButton = getButton("WIS_MINUS_BTN");
	_intelligenceMinusButton = getButton("INT_MINUS_BTN");
	_charismaMinusButton = getButton("CHA_MINUS_BTN");

	_strengthButton->setText(Common::composeString(_strength));
	_dexterityButton->setText(Common::composeString(_dexterity));
	_constitutionButton->setText(Common::composeString(_charisma));
	_wisdomButton->setText(Common::composeString(_wisdom));
	_intelligenceButton->setText(Common::composeString(_intelligence));
	_charismaButton->setText(Common::composeString(_charisma));

	_strengthMinusButton->setInvisible(true);
	_dexterityMinusButton->setInvisible(true);
	_constitutionMinusButton->setInvisible(true);
	_wisdomMinusButton->setInvisible(true);
	_intelligenceMinusButton->setInvisible(true);
	_charismaMinusButton->setInvisible(true);

	_strengthDesc = TalkMan.getString(222); // TODO read from gui
	_dexterityDesc = TalkMan.getString(223); // TODO read from gui
	_constitutionDesc = TalkMan.getString(224); // TODO read from gui
	_wisdomDesc = TalkMan.getString(225); // TODO read from gui
	_intelligenceDesc = TalkMan.getString(226); // TODO read from gui
	_charismaDesc = TalkMan.getString(227); // TODO read from gui

	_remainingSectionsLabel->setText(Common::composeString(_remainingSections));
}

void CharacterGenerationAbilitiesMenu::callbackRun() {
	// Check if a specific button is hovered and set modifier, cost and description
	if ((_strengthButton->isHovered()
				|| _strengthLabel->isHovered()
				|| _strengthPlusButton->isHovered()
				|| _strengthMinusButton->isHovered()
	    ) && _hoveredButton != _strengthButton) {

		_strengthLabel->setHighlight(true);
		_dexterityLabel->setHighlight(false);
		_constitutionLabel->setHighlight(false);
		_wisdomLabel->setHighlight(false);
		_intelligenceLabel->setHighlight(false);
		_charismaLabel->setHighlight(false);

		updateCostModifier(_strength);
		_hoveredButton = _strengthButton;
		return;
	}
	if ((_dexterityButton->isHovered()
				|| _dexterityLabel->isHovered()
				|| _dexterityPlusButton->isHovered()
				|| _dexterityMinusButton->isHovered()
	    ) && _hoveredButton != _dexterityButton) {

		_strengthLabel->setHighlight(false);
		_dexterityLabel->setHighlight(true);
		_constitutionLabel->setHighlight(false);
		_wisdomLabel->setHighlight(false);
		_intelligenceLabel->setHighlight(false);
		_charismaLabel->setHighlight(false);

		updateCostModifier(_dexterity);
		_hoveredButton = _dexterityButton;
		return;
	}
	if ((_constitutionButton->isHovered()
				|| _constitutionLabel->isHovered()
				|| _constitutionPlusButton->isHovered()
				|| _constitutionMinusButton->isHovered()
	    ) && _hoveredButton != _constitutionButton) {

		_strengthLabel->setHighlight(false);
		_dexterityLabel->setHighlight(false);
		_constitutionLabel->setHighlight(true);
		_wisdomLabel->setHighlight(false);
		_intelligenceLabel->setHighlight(false);
		_charismaLabel->setHighlight(false);

		updateCostModifier(_wisdom);
		_hoveredButton = _constitutionButton;
		return;
	}
	if ((_wisdomButton->isHovered()
				|| _wisdomLabel->isHovered()
				|| _wisdomPlusButton->isHovered()
				|| _wisdomMinusButton->isHovered()
	    ) && _hoveredButton != _wisdomButton) {

		_strengthLabel->setHighlight(false);
		_dexterityLabel->setHighlight(false);
		_constitutionLabel->setHighlight(false);
		_wisdomLabel->setHighlight(true);
		_intelligenceLabel->setHighlight(false);
		_charismaLabel->setHighlight(false);

		updateCostModifier(_wisdom);
		_hoveredButton = _wisdomButton;
		return;
	}
	if ((_intelligenceButton->isHovered()
				|| _intelligenceLabel->isHovered()
				|| _intelligencePlusButton->isHovered()
				|| _intelligenceMinusButton->isHovered()
	    ) && _hoveredButton != _intelligenceButton) {

		_strengthLabel->setHighlight(false);
		_dexterityLabel->setHighlight(false);
		_constitutionLabel->setHighlight(false);
		_wisdomLabel->setHighlight(false);
		_intelligenceLabel->setHighlight(true);
		_charismaLabel->setHighlight(false);

		updateCostModifier(_intelligence);
		_hoveredButton = _intelligenceButton;
		return;
	}
	if ((_charismaButton->isHovered()
				|| _charismaLabel->isHovered()
				|| _charismaPlusButton->isHovered()
				|| _charismaMinusButton->isHovered()
	    ) && _hoveredButton != _charismaButton) {

		_strengthLabel->setHighlight(false);
		_dexterityLabel->setHighlight(false);
		_constitutionLabel->setHighlight(false);
		_wisdomLabel->setHighlight(false);
		_intelligenceLabel->setHighlight(false);
		_charismaLabel->setHighlight(true);

		updateCostModifier(_charisma);
		_hoveredButton = _charismaButton;
		return;
	}

}

void CharacterGenerationAbilitiesMenu::updateCostModifier(uint8_t abilityPoints) {
	// TODO read from game files?
	switch (abilityPoints) {
		case 8:
		case 9:
			_costPointsLabel->setText("1");
			_abilityModLabel->setText("-1");
			break;
		case 10:
		case 11:
			_costPointsLabel->setText("1");
			_abilityModLabel->setText("-");
			break;
		case 12:
		case 13:
			_costPointsLabel->setText("1");
			_abilityModLabel->setText("+1");
			break;
		case 14:
		case 15:
			_costPointsLabel->setText("2");
			_abilityModLabel->setText("+2");
			break;
		case 16:
		case 17:
			_costPointsLabel->setText("3");
			_abilityModLabel->setText("+3");
			break;
		case 18:
			_costPointsLabel->setText("3");
			_abilityModLabel->setText("+4");
			break;
	}
}

uint8_t CharacterGenerationAbilitiesMenu::getAbilityCost(uint8_t abilityPoints) {
	switch (abilityPoints) {
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
			return 1;
		case 14:
		case 15:
			return 2;
		case 16:
		case 17:
		case 18:
			return 3;
		default:
			return 1; // TODO what to do in this case? Error message?
	}
}

void CharacterGenerationAbilitiesMenu::callbackActive(Widget &widget) {
	if (widget.getTag() == "BTN_BACK") {
		_returnCode = 1;
		return;
	}
	if (widget.getTag() == "BTN_ACCEPT") {
		// TODO save abilities to character info
		// TODO check that all points have been spent
		accept();
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "STR_PLUS_BTN") {
		if (_remainingSections <= 0 || _strength >= AB_MAX_POINTS) {
			return; // Button should not be clickable in this case
		}
		switch (_strength) {
			case 8:
			case 9:
			case 10:
			case 11:
			case 12:
			case 13:
				_remainingSections--;
				break;
			case 14:
			case 15:
				_remainingSections = _remainingSections - 2;
				break;
			case 16:
			case 17:
				_remainingSections = _remainingSections - 3;
				break;
		}
		_strength++;

		_remainingSectionsLabel->setText(Common::composeString(_remainingSections));
		_strengthButton->setText(Common::composeString(_strength));
		updateCostModifier(_strength);

		_strengthMinusButton->setInvisible(false);
		_strengthMinusButton->show();
		if (_strength >= AB_MAX_POINTS) {
			_strengthPlusButton->setInvisible(true);
			_strengthPlusButton->hide();
		}
		if (3 >= _remainingSections) {
			disablePlusButtons();
		}

		return;
	}

	if (widget.getTag() == "STR_MINUS_BTN") {
		if (_strength <= AB_MIN_POINTS) {
			return; // Button should not be clickable in this case
		}
		switch (_strength) {
			case 9:
			case 10:
			case 11:
			case 12:
			case 13:
			case 14:
				_remainingSections++;
				break;
			case 15:
			case 16:
				_remainingSections = _remainingSections + 2;
				break;
			case 17:
			case 18:
				_remainingSections = _remainingSections + 3;
				break;
		}
		_strength--;

		_remainingSectionsLabel->setText(Common::composeString(_remainingSections));
		_strengthButton->setText(Common::composeString(_strength));
		updateCostModifier(_strength);

		_strengthPlusButton->setInvisible(false);
		_strengthPlusButton->show();
		if (_strength <= AB_MIN_POINTS) {
			_strengthMinusButton->setInvisible(true);
			_strengthMinusButton->hide();
		}
		if (3 >= _remainingSections) {
			enablePlusButtons();
		}

		return;
	}

	if (widget.getTag() == "DEX_PLUS_BTN") {
		if (_remainingSections <= 0 || _dexterity >= AB_MAX_POINTS) {
			return; // Button should not be clickable in this case
		}
		switch (_dexterity) {
			case 8:
			case 9:
			case 10:
			case 11:
			case 12:
			case 13:
				_remainingSections--;
				break;
			case 14:
			case 15:
				_remainingSections = _remainingSections - 2;
				break;
			case 16:
			case 17:
				_remainingSections = _remainingSections - 3;
				break;
		}
		_dexterity++;

		_remainingSectionsLabel->setText(Common::composeString(_remainingSections));
		_dexterityButton->setText(Common::composeString(_dexterity));
		updateCostModifier(_dexterity);

		_dexterityMinusButton->setInvisible(false);
		_dexterityMinusButton->show();
		if (_dexterity >= AB_MAX_POINTS) {
			_dexterityPlusButton->setInvisible(true);
			_dexterityPlusButton->hide();
		}
		if (3 >= _remainingSections) {
			disablePlusButtons();
		}

		return;
	}

	if (widget.getTag() == "DEX_MINUS_BTN") {
		if (_dexterity <= AB_MIN_POINTS) {
			return; // Button should not be clickable in this case
		}
		switch (_dexterity) {
			case 9:
			case 10:
			case 11:
			case 12:
			case 13:
			case 14:
				_remainingSections++;
				break;
			case 15:
			case 16:
				_remainingSections = _remainingSections + 2;
				break;
			case 17:
			case 18:
				_remainingSections = _remainingSections + 3;
				break;
		}
		_dexterity--;

		_remainingSectionsLabel->setText(Common::composeString(_remainingSections));
		_dexterityButton->setText(Common::composeString(_dexterity));
		updateCostModifier(_dexterity);

		_dexterityPlusButton->setInvisible(false);
		_dexterityPlusButton->show();
		if (_dexterity <= AB_MIN_POINTS) {
			_dexterityMinusButton->setInvisible(true);
			_dexterityMinusButton->hide();
		}
		if (3 >= _remainingSections) {
			enablePlusButtons();
		}

		return;
	}

	if (widget.getTag() == "CON_PLUS_BTN") {
		if (_remainingSections <= 0 || _constitution >= AB_MAX_POINTS) {
			return; // Button should not be clickable in this case
		}
		switch (_constitution) {
			case 8:
			case 9:
			case 10:
			case 11:
			case 12:
			case 13:
				_remainingSections--;
				break;
			case 14:
			case 15:
				_remainingSections = _remainingSections - 2;
				break;
			case 16:
			case 17:
				_remainingSections = _remainingSections - 3;
				break;
		}
		_constitution++;

		_remainingSectionsLabel->setText(Common::composeString(_remainingSections));
		_constitutionButton->setText(Common::composeString(_constitution));
		updateCostModifier(_constitution);

		_constitutionMinusButton->setInvisible(false);
		_constitutionMinusButton->show();
		if (_constitution >= AB_MAX_POINTS) {
			_constitutionPlusButton->setInvisible(true);
			_constitutionPlusButton->hide();
		}
		if (3 >= _remainingSections) {
			disablePlusButtons();
		}

		return;
	}

	if (widget.getTag() == "CON_MINUS_BTN") {
		if (_constitution <= AB_MIN_POINTS) {
			return; // Button should not be clickable in this case
		}
		switch (_constitution) {
			case 9:
			case 10:
			case 11:
			case 12:
			case 13:
			case 14:
				_remainingSections++;
				break;
			case 15:
			case 16:
				_remainingSections = _remainingSections + 2;
				break;
			case 17:
			case 18:
				_remainingSections = _remainingSections + 3;
				break;
		}
		_constitution--;

		_remainingSectionsLabel->setText(Common::composeString(_remainingSections));
		_constitutionButton->setText(Common::composeString(_constitution));
		updateCostModifier(_constitution);

		_constitutionPlusButton->setInvisible(false);
		_constitutionPlusButton->show();
		if (_constitution <= AB_MIN_POINTS) {
			_constitutionMinusButton->setInvisible(true);
			_constitutionMinusButton->hide();
		}
		if (3 >= _remainingSections) {
			enablePlusButtons();
		}

		return;
	}

	if (widget.getTag() == "WIS_PLUS_BTN") {
		if (_remainingSections <= 0 || _wisdom >= AB_MAX_POINTS) {
			return; // Button should not be clickable in this case
		}
		switch (_wisdom) {
			case 8:
			case 9:
			case 10:
			case 11:
			case 12:
			case 13:
				_remainingSections--;
				break;
			case 14:
			case 15:
				_remainingSections = _remainingSections - 2;
				break;
			case 16:
			case 17:
				_remainingSections = _remainingSections - 3;
				break;
		}
		_wisdom++;

		_remainingSectionsLabel->setText(Common::composeString(_remainingSections));
		_wisdomButton->setText(Common::composeString(_wisdom));
		updateCostModifier(_wisdom);

		_wisdomMinusButton->setInvisible(false);
		_wisdomMinusButton->show();
		if (_wisdom >= AB_MAX_POINTS) {
			_wisdomPlusButton->setInvisible(true);
			_wisdomPlusButton->hide();
		}
		if (3 >= _remainingSections) {
			disablePlusButtons();
		}

		return;
	}

	if (widget.getTag() == "WIS_MINUS_BTN") {
		if (_wisdom <= AB_MIN_POINTS) {
			return; // Button should not be clickable in this case
		}
		switch (_wisdom) {
			case 9:
			case 10:
			case 11:
			case 12:
			case 13:
			case 14:
				_remainingSections++;
				break;
			case 15:
			case 16:
				_remainingSections = _remainingSections + 2;
				break;
			case 17:
			case 18:
				_remainingSections = _remainingSections + 3;
				break;
		}
		_wisdom--;

		_remainingSectionsLabel->setText(Common::composeString(_remainingSections));
		_wisdomButton->setText(Common::composeString(_wisdom));
		updateCostModifier(_wisdom);

		_wisdomPlusButton->setInvisible(false);
		_wisdomPlusButton->show();
		if (_wisdom <= AB_MIN_POINTS) {
			_wisdomMinusButton->setInvisible(true);
			_wisdomMinusButton->hide();
		}
		if (3 >= _remainingSections) {
			enablePlusButtons();
		}

		return;
	}

	if (widget.getTag() == "INT_PLUS_BTN") {
		if (_remainingSections <= 0 || _intelligence >= AB_MAX_POINTS) {
			return; // Button should not be clickable in this case
		}
		switch (_intelligence) {
			case 8:
			case 9:
			case 10:
			case 11:
			case 12:
			case 13:
				_remainingSections--;
				break;
			case 14:
			case 15:
				_remainingSections = _remainingSections - 2;
				break;
			case 16:
			case 17:
				_remainingSections = _remainingSections - 3;
				break;
		}
		_intelligence++;

		_remainingSectionsLabel->setText(Common::composeString(_remainingSections));
		_intelligenceButton->setText(Common::composeString(_intelligence));
		updateCostModifier(_intelligence);

		_intelligenceMinusButton->setInvisible(false);
		_intelligenceMinusButton->show();
		if (_intelligence >= AB_MAX_POINTS) {
			_intelligencePlusButton->setInvisible(true);
			_intelligencePlusButton->hide();
		}
		if (3 >= _remainingSections) {
			disablePlusButtons();
		}

		return;
	}

	if (widget.getTag() == "INT_MINUS_BTN") {
		if (_intelligence <= AB_MIN_POINTS) {
			return; // Button should not be clickable in this case
		}
		switch (_intelligence) {
			case 9:
			case 10:
			case 11:
			case 12:
			case 13:
			case 14:
				_remainingSections++;
				break;
			case 15:
			case 16:
				_remainingSections = _remainingSections + 2;
				break;
			case 17:
			case 18:
				_remainingSections = _remainingSections + 3;
				break;
		}
		_intelligence--;

		_remainingSectionsLabel->setText(Common::composeString(_remainingSections));
		_intelligenceButton->setText(Common::composeString(_intelligence));
		updateCostModifier(_intelligence);

		_intelligencePlusButton->setInvisible(false);
		_intelligencePlusButton->show();
		if (_intelligence <= AB_MIN_POINTS) {
			_intelligenceMinusButton->setInvisible(true);
			_intelligenceMinusButton->hide();
		}
		if (3 >= _remainingSections) {
			enablePlusButtons();
		}

		return;
	}

	if (widget.getTag() == "CHA_PLUS_BTN") {
		if (_remainingSections <= 0 || _charisma >= AB_MAX_POINTS) {
			return; // Button should not be clickable in this case
		}
		switch (_intelligence) {
			case 8:
			case 9:
			case 10:
			case 11:
			case 12:
			case 13:
				_remainingSections--;
				break;
			case 14:
			case 15:
				_remainingSections = _remainingSections - 2;
				break;
			case 16:
			case 17:
				_remainingSections = _remainingSections - 3;
				break;
		}
		_charisma++;

		_remainingSectionsLabel->setText(Common::composeString(_remainingSections));
		_charismaButton->setText(Common::composeString(_charisma));
		updateCostModifier(_charisma);

		_charismaMinusButton->setInvisible(false);
		_charismaMinusButton->show();
		if (_charisma >= AB_MAX_POINTS) {
			_charismaPlusButton->setInvisible(true);
			_charismaPlusButton->hide();
		}
		if (3 >= _remainingSections) {
			disablePlusButtons();
		}

		return;
	}

	if (widget.getTag() == "CHA_MINUS_BTN") {
		if (_charisma <= AB_MIN_POINTS) {
			return; // Button should not be clickable in this case
		}
		switch (_charisma) {
			case 9:
			case 10:
			case 11:
			case 12:
			case 13:
			case 14:
				_remainingSections++;
				break;
			case 15:
			case 16:
				_remainingSections = _remainingSections + 2;
				break;
			case 17:
			case 18:
				_remainingSections = _remainingSections + 3;
				break;
		}
		_charisma--;

		_remainingSectionsLabel->setText(Common::composeString(_remainingSections));
		_charismaButton->setText(Common::composeString(_charisma));
		updateCostModifier(_charisma);

		_charismaPlusButton->setInvisible(false);
		_charismaPlusButton->show();
		if (_charisma <= AB_MIN_POINTS) {
			_charismaMinusButton->setInvisible(true);
			_charismaMinusButton->hide();
		}
		if (3 >= _remainingSections) {
			enablePlusButtons();
		}

		return;
	}

	if (widget.getTag() == "BTN_RECOMMENDED") {
		_remainingSectionsLabel = 0;

		// FIXME thonse abilities should have been read from the classes.2da during class selection,
		// but seem to be all 0
		_strength = _info.getAbilities().strength;
		_dexterity = _info.getAbilities().dexterity;
		_constitution = _info.getAbilities().constitution;
		_wisdom = _info.getAbilities().wisdom;
		_intelligence = _info.getAbilities().intelligence;
		_charisma = _info.getAbilities().charisma;

		_remainingSectionsLabel->setText(Common::composeString(_remainingSections));

		_strengthButton->setText(Common::composeString(_strength));
		_dexterityButton->setText(Common::composeString(_dexterity));
		_constitutionButton->setText(Common::composeString(_charisma));
		_wisdomButton->setText(Common::composeString(_wisdom));
		_intelligenceButton->setText(Common::composeString(_intelligence));
		_charismaButton->setText(Common::composeString(_charisma));

		_costPointsLabel->setText("0");
		_abilityModLabel->setText("0");

		disablePlusButtons();
		// TODO reset ability description

		return;
	}

}

void CharacterGenerationAbilitiesMenu::disablePlusButtons() {
	if (_remainingSections < getAbilityCost(_strength)) {
		_strengthPlusButton->setInvisible(true);
		_strengthPlusButton->hide();
	}
	if (_remainingSections < getAbilityCost(_dexterity)) {
		_dexterityPlusButton->setInvisible(true);
		_dexterityPlusButton->hide();
	}
	if (_remainingSections < getAbilityCost(_constitution)) {
		_constitutionPlusButton->setInvisible(true);
		_constitutionPlusButton->hide();
	}
	if (_remainingSections < getAbilityCost(_wisdom)) {
		_wisdomPlusButton->setInvisible(true);
		_wisdomPlusButton->hide();
	}
	if (_remainingSections < getAbilityCost(_intelligence)) {
		_intelligencePlusButton->setInvisible(true);
		_intelligencePlusButton->hide();
	}
	if (_remainingSections < getAbilityCost(_charisma)) {
		_charismaPlusButton->setInvisible(true);
		_charismaPlusButton->hide();
	}
}

void CharacterGenerationAbilitiesMenu::enablePlusButtons() {
	if (AB_MAX_POINTS > _strength
			&& _remainingSections >= getAbilityCost(_strength)) {
		_strengthPlusButton->setInvisible(false);
		_strengthPlusButton->show();
	}
	if (AB_MAX_POINTS > _dexterity
			&& _remainingSections >= getAbilityCost(_dexterity)) {
		_dexterityPlusButton->setInvisible(false);
		_dexterityPlusButton->show();
	}
	if (AB_MAX_POINTS > _constitution
			&& _remainingSections >= getAbilityCost(_constitution)) {
		_constitutionPlusButton->setInvisible(false);
		_constitutionPlusButton->show();
	}
	if (AB_MAX_POINTS > _wisdom
			&& _remainingSections >= getAbilityCost(_wisdom)) {
		_wisdomPlusButton->setInvisible(false);
		_wisdomPlusButton->show();
	}
	if (AB_MAX_POINTS > _intelligence
			&& _remainingSections >= getAbilityCost(_intelligence)) {
		_intelligencePlusButton->setInvisible(false);
		_intelligencePlusButton->show();
	}
	if (AB_MAX_POINTS > _charisma
			&& _remainingSections >= getAbilityCost(_charisma)) {
		_charismaPlusButton->setInvisible(false);
		_charismaPlusButton->show();
	}
}

} // End of namespace KotOR

} // End of namespace Engines
