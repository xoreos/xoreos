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

	void callbackActive(Widget &widget);
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_GUI_CHARGEN_CHARGENABILITIES_H
