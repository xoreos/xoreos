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
 *  The ability score allocation menu for custom character creation.
 */

#ifndef ENGINES_KOTOR_GUI_CHARGEN_CHARGENABILITIES_H
#define ENGINES_KOTOR_GUI_CHARGEN_CHARGENABILITIES_H

#include "src/engines/kotorbase/types.h"

#include "src/engines/kotor/gui/chargen/chargenbase.h"

namespace Engines {

namespace KotOR {

/** Menu that lets the player distribute ability points during custom character creation.
 *
 *  Loads the "abitems" GUI resource.  The player starts with 30 points to spend;
 *  each ability begins at 8 and costs one point per rank up to 13, then two points
 *  for 14–15, three for 16, four for 17, and five for 18.
 */
class CharacterGenerationAbilitiesMenu : public CharacterGenerationBaseMenu {
public:
	CharacterGenerationAbilitiesMenu(KotORBase::CharacterGenerationInfo &info,
	                                  ::Engines::Console *console = 0);

private:
	/** Remaining ability points the player may spend. */
	int _remainingPoints;

	/** Current working values for each ability (before accept). */
	uint32_t _str, _dex, _con, _intl, _wis, _cha;

	/** Cost to raise an ability from its current score to score+1. */
	static int raiseCost(uint32_t current);
	/** Cost refunded when lowering an ability from current to current-1. */
	static int lowerCost(uint32_t current);

	/** Refresh every displayed value label and the remaining-points label. */
	void updateLabels();

	void callbackActive(Widget &widget);
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_GUI_CHARGEN_CHARGENABILITIES_H
