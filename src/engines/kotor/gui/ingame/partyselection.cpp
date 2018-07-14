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
 *  Party selection GUI for Star Wars: Knights of the Old Republic.
 */

#include "src/engines/aurora/widget.h"

#include "src/engines/kotor/gui/ingame/partyselection.h"

namespace Engines {

namespace KotOR {

PartySelectionGUI::PartySelectionGUI() {
	addBackground(kBackgroundTypeMenu, true);
	load("partyselection");
}

void PartySelectionGUI::callbackActive(Widget &widget) {
	const Common::UString &tag = widget.getTag();
	if (tag == "BTN_BACK")
		_returnCode = 1;
}

} // End of namespace KotOR

} // End of namespace Engines
