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
 *  The skill allocation menu for custom character creation.
 */

#ifndef ENGINES_KOTOR_GUI_CHARGEN_CHARGENSKILLS_H
#define ENGINES_KOTOR_GUI_CHARGEN_CHARGENSKILLS_H

#include <array>

#include "src/engines/kotorbase/types.h"

#include "src/engines/kotor/gui/chargen/chargenbase.h"

namespace Engines {

namespace KotOR {

/** Menu that lets the player distribute skill points during custom character creation.
 *
 *  Loads the "skilitems" GUI resource.  The number of skill points available is
 *  (4 + Intelligence modifier) for Soldiers, (6 + Int mod) for Scouts, and
 *  (8 + Int mod) for Scoundrels.
 */
class CharacterGenerationSkillsMenu : public CharacterGenerationBaseMenu {
public:
	CharacterGenerationSkillsMenu(KotORBase::CharacterGenerationInfo &info,
	                               ::Engines::Console *console = 0);

private:
	/** Remaining skill points. */
	int _remainingPoints;

	/** Working skill ranks for each skill. */
	std::array<uint32_t, KotORBase::kSkillMAX> _ranks;

	/** Compute initial skill points for the chosen class and Intelligence. */
	int computeSkillPoints() const;

	void updateLabels();

	void callbackActive(Widget &widget);
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_GUI_CHARGEN_CHARGENSKILLS_H
