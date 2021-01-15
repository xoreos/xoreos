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

#ifndef ENGINES_KOTOR_GUI_CHARGEN_CHARGENABILITIESS_H
#define ENGINES_KOTOR_GUI_CHARGEN_CHARGENABILITIESS_H

#include "src/engines/kotorbase/gui/gui.h"

#include "src/engines/kotor/gui/chargen/chargenbase.h"

namespace Engines {

namespace KotOR {

class CharacterGenerationAbilitiesMenu : public CharacterGenerationBaseMenu {
public:
	CharacterGenerationAbilitiesMenu(KotORBase::CharacterGenerationInfo &info,
	                            ::Engines::Console *console = 0);

private:
	uint8_t _remainingSections;

	uint8_t _strength;
	uint8_t _dexterity;
	uint8_t _constitution;
	uint8_t _wisdom;
	uint8_t _intelligence;
	uint8_t _charisma;

	Odyssey::WidgetLabel *_remainingSectionsLabel;
	Odyssey::WidgetLabel *_costPointsLabel;
	Odyssey::WidgetLabel *_abilityModLabel;

	Odyssey::WidgetListBox *_descListBox;

	Odyssey::WidgetLabel *_strengthLabel;
	Odyssey::WidgetLabel *_dexterityLabel;
	Odyssey::WidgetLabel *_constitutionLabel;
	Odyssey::WidgetLabel *_wisdomLabel;
	Odyssey::WidgetLabel *_intelligenceLabel;
	Odyssey::WidgetLabel *_charismaLabel;

	Odyssey::WidgetButton *_hoveredButton;

	Odyssey::WidgetButton *_strengthButton;
	Odyssey::WidgetButton *_dexterityButton;
	Odyssey::WidgetButton *_constitutionButton;
	Odyssey::WidgetButton *_wisdomButton;
	Odyssey::WidgetButton *_intelligenceButton;
	Odyssey::WidgetButton *_charismaButton;

	Odyssey::WidgetButton *_strengthPlusButton;
	Odyssey::WidgetButton *_dexterityPlusButton;
	Odyssey::WidgetButton *_constitutionPlusButton;
	Odyssey::WidgetButton *_wisdomPlusButton;
	Odyssey::WidgetButton *_intelligencePlusButton;
	Odyssey::WidgetButton *_charismaPlusButton;

	Odyssey::WidgetButton *_strengthMinusButton;
	Odyssey::WidgetButton *_dexterityMinusButton;
	Odyssey::WidgetButton *_constitutionMinusButton;
	Odyssey::WidgetButton *_wisdomMinusButton;
	Odyssey::WidgetButton *_intelligenceMinusButton;
	Odyssey::WidgetButton *_charismaMinusButton;

	Common::UString _strengthDesc;
	Common::UString _dexterityDesc;
	Common::UString _constitutionDesc;
	Common::UString _wisdomDesc;
	Common::UString _intelligenceDesc;
	Common::UString _charismaDesc;

	void callbackActive(Widget &widget);
	void callbackRun();
	uint8_t getAbilityCost(uint8_t abilityPoints);
	void updateCostModifier(uint8_t abilityPoints);
	void disablePlusButtons();
	void enablePlusButtons();
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_GUI_CHARGEN_CHARGENABILITIES_H
