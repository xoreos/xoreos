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

#include "src/engines/odyssey/button.h"
#include "src/engines/odyssey/label.h"

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
		return;
	}

	if (widget.getTag() == "BTN_RECOMMENDED") {
		_remainingSectionsLabel = 0;

		return;
	}

}


} // End of namespace KotOR

} // End of namespace Engines
