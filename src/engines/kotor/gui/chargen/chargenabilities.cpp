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


namespace Engines {

namespace KotOR {

CharacterGenerationAbilitiesMenu::CharacterGenerationAbilitiesMenu(KotORBase::CharacterGenerationInfo &info,
                                                                 Console *console) :
		CharacterGenerationBaseMenu(info, console) {

	load("abchrgen");

	addBackground(KotORBase::kBackgroundTypeMenu);

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
}


} // End of namespace KotOR

} // End of namespace Engines
