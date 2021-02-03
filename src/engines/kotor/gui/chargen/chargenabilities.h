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

/**
 * @file chargenabilities.h
 * @brief The menu for modifying the attributes/abilities of the character.
 *
 * This is the implementation of the logic of buying ability points and the
 * behavior of the buttons, labels and the ability descriptions.
 */

#ifndef ENGINES_KOTOR_GUI_CHARGEN_CHARGENABILITIESS_H
#define ENGINES_KOTOR_GUI_CHARGEN_CHARGENABILITIESS_H

#include "src/engines/kotorbase/gui/gui.h"

#include "src/engines/kotor/gui/chargen/chargenbase.h"

namespace Engines {

namespace KotOR {

/**
 * @brief Class representing the ability selection menu.
 */
class CharacterGenerationAbilitiesMenu : public CharacterGenerationBaseMenu {
public:
	/**
	 * @brief Initializes the ability selection menu.
	 *
	 *  - sets initial values for the abilities, points to spend on them
	 *  - sets initial description labels
	 *  - disables subtraction buttons
	 *
	 * @param info information about the character
	 * @param console console to log to
	 */
	CharacterGenerationAbilitiesMenu(KotORBase::CharacterGenerationInfo &info,
	                            ::Engines::Console *console = 0);

private:
	uint8_t _remainingSections; ///< Remaining points to spend

	uint8_t _strength; ///< Current points spent on strength
	uint8_t _dexterity; ///< Current points spent on dexterity
	uint8_t _constitution; ///< Current points spent on constitution
	uint8_t _wisdom; ///< Current points spent on wisdom
	uint8_t _intelligence; ///< Current points spent on intelligence
	uint8_t _charisma; ///< Current points spent on charisma

	Odyssey::WidgetLabel *_remainingSectionsLabel; ///< Label displaying remaining points
	Odyssey::WidgetLabel *_costPointsLabel; ///< Label displaying the cost of increasing the ability
	Odyssey::WidgetLabel *_abilityModLabel; ///< Label displaying the modifier of the ability

	Odyssey::WidgetListBox *_descListBox; ///< List box containing the description of the ability

	Odyssey::WidgetLabel *_strengthLabel; ///< Label displaying "strength"
	Odyssey::WidgetLabel *_dexterityLabel; ///< Label displaying "dexterity"
	Odyssey::WidgetLabel *_constitutionLabel; ///< Label displaying "constitution"
	Odyssey::WidgetLabel *_wisdomLabel; ///< Label displaying wisdom "wisdom"
	Odyssey::WidgetLabel *_intelligenceLabel; ///< Label intelligence "intelligence"
	Odyssey::WidgetLabel *_charismaLabel; ///< Label displaying charisma "charisma"

	Odyssey::WidgetButton *_hoveredButton; ///< Pointer to the currently hovered button/ability

	Odyssey::WidgetButton *_strengthButton; ///< Label displaying strength points
	Odyssey::WidgetButton *_dexterityButton; ///< Label displaying dexterity points
	Odyssey::WidgetButton *_constitutionButton; ///< Label displaying constitution points
	Odyssey::WidgetButton *_wisdomButton; ///< Label displaying wisdom points
	Odyssey::WidgetButton *_intelligenceButton; ///< Label displaying intelligence points
	Odyssey::WidgetButton *_charismaButton; ///< Label displaying charisma points

	Odyssey::WidgetButton *_strengthPlusButton; ///< Button increasing strength
	Odyssey::WidgetButton *_dexterityPlusButton; ///< Button increasing dexterity
	Odyssey::WidgetButton *_constitutionPlusButton; ///< Button increasing constitution
	Odyssey::WidgetButton *_wisdomPlusButton; ///< Button increasing wisdom
	Odyssey::WidgetButton *_intelligencePlusButton; ///< Button increasing intelligence
	Odyssey::WidgetButton *_charismaPlusButton; ///< Button increasing charisma

	Odyssey::WidgetButton *_strengthMinusButton; ///< Button decreasing strength
	Odyssey::WidgetButton *_dexterityMinusButton; ///< Button decreasing dexterity
	Odyssey::WidgetButton *_constitutionMinusButton; ///< Button decreasing constitution
	Odyssey::WidgetButton *_wisdomMinusButton; ///< Button decreasing wisdom
	Odyssey::WidgetButton *_intelligenceMinusButton; ///< Button decreasing intelligence
	Odyssey::WidgetButton *_charismaMinusButton; ///< Button decreasing charisma

	Common::UString _strengthDesc; ///< Description text of strength
	Common::UString _dexterityDesc; ///< Description text of dexterity
	Common::UString _constitutionDesc; ///< Description text of constitution
	Common::UString _wisdomDesc; ///< Description text of wisdom
	Common::UString _intelligenceDesc; ///< Description text of intelligence
	Common::UString _charismaDesc; ///< Description text of charisma

	/**
	 * @brief Callback called on a button press
	 *
	 * @param widget widget representing pressed button
	 */
	void callbackActive(Widget &widget);

	/**
	 * @brief callback used to react to hovering abilities
	 *
	 * Updates the cost for increasing the ability,
	 * the current modifier
	 * and the description for the ability
	 */
	void callbackRun();

	/**
	 * @brief Calculates the cost of increasing an ability
	 *
	 * @param abilityPoints the current points of an ability
	 *
	 * @return cost of increasing an ability
	 */
	uint8_t getAbilityCost(uint8_t abilityPoints);

	/**
	 * @brief Update the display of cost and modifier
	 *
	 * @param abilityPoints the points of the ability for which
	 *        to update cost and modifier
	 */
	void updateCostModifier(uint8_t abilityPoints);

	/**
	 * @brief Disables plus buttons
	 *
	 * This disables plus buttons for abilities that are no longer
	 * able to be updated because the cost is higher than the
	 * remaining points
	 */
	void disablePlusButtons();

	/**
	 * @brief Enable plus buttons
	 *
	 * Enable all plus buttons that became available again because
	 * the cost sunk below the now available points
	 */
	void enablePlusButtons();
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_GUI_CHARGEN_CHARGENABILITIES_H
