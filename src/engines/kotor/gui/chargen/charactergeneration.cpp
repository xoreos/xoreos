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
 *  The primary character generation menu.
 */

#include "src/engines/kotor/gui/widgets/label.h"

#include "src/engines/kotor/gui/chargen/charactergeneration.h"

namespace Engines {

namespace KotOR {

CharacterGenerationMenu::CharacterGenerationMenu(Module *UNUSED(module), Console *console) : GUI(console) {
	load("maincg");

	addBackground(kBackgroundTypeMenu);

	getLabel("VIT_ARROW_LBL")->setText("");
	getLabel("DEF_ARROW_LBL")->setText("");

	getLabel("LBL_NAME")->setText("");
	getLabel("LBL_CLASS")->setText("");

	getLabel("WILL_ARROW_LBL")->setText("");
	getLabel("REFL_ARROW_LBL")->setText("");
	getLabel("FORT_ARROW_LBL")->setText("");

	getWidget("NEW_LBL")->setInvisible(true);
	getWidget("OLD_LBL")->setInvisible(true);

	getWidget("LBL_LEVEL_VAL")->setInvisible(true);
	getWidget("LBL_LEVEL")->setInvisible(true);
}

} // End of namespace KotOR

} // End of namespace Engines
