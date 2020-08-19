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
 *  The ingame map menu.
 */

#include "src/aurora/talkman.h"

#include "src/engines/aurora/widget.h"

#include "src/engines/odyssey/button.h"

#include "src/engines/kotor/gui/ingame/menu_map.h"

#include "src/engines/kotor/gui/dialogs/confirm.h"

namespace Engines {

namespace KotOR {

MenuMap::MenuMap(Console *console) : KotORBase::GUI(console) {
	load("map");
}

void MenuMap::setReturnStrref(uint32_t id) {
	Odyssey::WidgetButton *btnReturn = getButton("BTN_RETURN");
	if (btnReturn)
		btnReturn->setText(TalkMan.getString(id));
}

void MenuMap::setReturnQueryStrref(uint32_t id) {
	_returnQueryMessage = TalkMan.getString(id);
}

void MenuMap::setReturnEnabled(bool enabled) {
	Odyssey::WidgetButton *btnReturn = getButton("BTN_RETURN");
	if (btnReturn)
		btnReturn->setDisabled(!enabled);
}

void MenuMap::callbackActive(Widget &widget) {
	if (widget.getTag() == "BTN_RETURN") {
		ConfirmDialog dialog(_console);
		dialog.setText(_returnQueryMessage);

		sub(dialog, kStartCodeNone, true, false);

		// TODO: Return to the hideout/ebon hawk
	}

	if (widget.getTag() == "BTN_EXIT") {
		_returnCode = 1;
		return;
	}
}

} // End of namespace KotOR

} // End of namespace Engines
