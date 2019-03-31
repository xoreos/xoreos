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
 *  Common base for in-game menus in KotOR games.
 */

#include "src/common/strutil.h"

#include "src/engines/odyssey/label.h"
#include "src/engines/odyssey/button.h"

#include "src/engines/kotorbase/module.h"
#include "src/engines/kotorbase/creature.h"

#include "src/engines/kotorbase/gui/menubase.h"

namespace Engines {

namespace KotORBase {

MenuBase::MenuBase(Module &module, Console *console) :
		GUI(console),
		_module(&module) {
}

MenuBase::~MenuBase() {
}

void MenuBase::update() {
	updatePartyMember(1);
	updatePartyMember(2);
}

void MenuBase::callbackActive(Widget &widget) {
	const Common::UString &tag = widget.getTag();

	if (tag.beginsWith("BTN_CHANGE")) {
		int memberIndex = (tag == "BTN_CHANGE1") ? 1 : 2;
		_module->setPartyLeaderByIndex(memberIndex);
		update();
		return;
	}
}

void MenuBase::updatePartyLeader(const Common::UString &widgetTag) {
	const Common::UString &portrait = _module->getPartyLeader()->getPortrait();
	getLabel(widgetTag)->setFill(portrait);
}

void MenuBase::updatePartyMember(int index) {
	Odyssey::WidgetButton *btnChange = getButton("BTN_CHANGE" + Common::composeString(index));
	KotORBase::Creature *partyMember = _module->getPartyMemberByIndex(index);
	if (partyMember) {
		Common::UString texture = partyMember->getPortrait();
		btnChange->setInvisible(false);
		btnChange->setFill(texture);
		btnChange->setHighlight(texture);
	} else {
		btnChange->setInvisible(true);
	}
}

} // End of namespace KotORBase

} // End of namespace Engines
