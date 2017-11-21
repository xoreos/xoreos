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
 *  The ingame menu.
 */

#include "src/engines/kotor/gui/ingame/menu.h"

#include "src/engines/kotor/gui/widgets/button.h"

namespace Engines {

namespace KotOR {

Menu::Menu(Console *console) : GUI(console) {
	load("top");

	addBackground(kBackgroundTypeMenu, true);

	_protoEqu = getProtoItem("LBLH_EQU");
	_protoInv = getProtoItem("LBLH_INV");
	_protoChar = getProtoItem("LBLH_CHA");
	_protoAbi = getProtoItem("LBLH_ABI");
	_protoMsg = getProtoItem("LBLH_MSG");
	_protoJou = getProtoItem("LBLH_JOU");
	_protoMap = getProtoItem("LBLH_MAP");
	_protoOpt = getProtoItem("LBLH_OPT");

	// Place the widgets as the top bar.
	float x, y, z;
	_protoEqu->getPosition(x, y, z);
	_protoEqu->setPosition(x, y + 197, z);

	_protoInv->getPosition(x, y, z);
	_protoInv->setPosition(x, y + 197, z);

	_protoChar->getPosition(x, y, z);
	_protoChar->setPosition(x, y + 197, z);

	_protoAbi->getPosition(x, y, z);
	_protoAbi->setPosition(x, y + 197, z);

	_protoMsg->getPosition(x, y, z);
	_protoMsg->setPosition(x, y + 197, z);

	_protoJou->getPosition(x, y, z);
	_protoJou->setPosition(x, y + 197, z);

	_protoMap->getPosition(x, y, z);
	_protoMap->setPosition(x, y + 197, z);

	_protoOpt->getPosition(x, y, z);
	_protoOpt->setPosition(x, y + 197, z);

	// Do the same for the buttons
	WidgetButton *buttonEqu = getButton("BTN_EQU");
	WidgetButton *buttonInv = getButton("BTN_INV");
	WidgetButton *buttonChar = getButton("BTN_CHAR");
	WidgetButton *buttonAbi = getButton("BTN_ABI");
	WidgetButton *buttonMsg = getButton("BTN_MSG");
	WidgetButton *buttonJou = getButton("BTN_JOU");
	WidgetButton *buttonMap = getButton("BTN_MAP");
	WidgetButton *buttonOpt = getButton("BTN_OPT");

	buttonEqu->getPosition(x, y, z);
	buttonEqu->setPosition(x, y + 197, z);

	buttonInv->getPosition(x, y, z);
	buttonInv->setPosition(x, y + 197, z);

	buttonChar->getPosition(x, y, z);
	buttonChar->setPosition(x, y + 197, z);

	buttonAbi->getPosition(x, y, z);
	buttonAbi->setPosition(x, y + 197, z);

	buttonMsg->getPosition(x, y, z);
	buttonMsg->setPosition(x, y + 197, z);

	buttonJou->getPosition(x, y, z);
	buttonJou->setPosition(x, y + 197, z);

	buttonMap->getPosition(x, y, z);
	buttonMap->setPosition(x, y + 197, z);

	buttonOpt->getPosition(x, y, z);
	buttonOpt->setPosition(x, y + 197, z);
}

void Menu::callbackActive(Widget &widget) {
	if (widget.getTag() == "BTN_EQU") {
		return;
	}
	if (widget.getTag() == "BTN_INV") {
		return;
	}
	if (widget.getTag() == "BTN_CHAR") {
		return;
	}
	if (widget.getTag() == "BTN_ABI") {
		return;
	}
	if (widget.getTag() == "BTN_JOU") {
		return;
	}
	if (widget.getTag() == "BTN_MAP") {
		return;
	}
	if (widget.getTag() == "BTN_OPT") {
		return;
	}
}

} // End of namespace KotOR

} // End of namespace Engines
