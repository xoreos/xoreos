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

#include "src/engines/kotor/gui/widgets/button.h"

#include "src/engines/kotor/gui/ingame/menu.h"

namespace Engines {

namespace KotOR {

Menu::Menu(Console *console) : GUI(console), _currentMenu(0), _lastProto(0) {
	load("top");

	addBackground(kBackgroundTypeMenu, true);

	_menuEqu.reset(new MenuEquipment(console));
	_menuInv.reset(new MenuInventory(console));
	_menuChar.reset(new MenuCharacter(console));
	_menuAbi.reset(new MenuAbilities(console));
	_menuMsg.reset(new MenuMessages(console));
	_menuJou.reset(new MenuJournal(console));
	_menuMap.reset(new MenuMap(console));
	_menuOpt.reset(new MenuOptions(console));

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

void Menu::setReturnStrref(uint32 id) {
	_menuMap->setReturnStrref(id);
}

void Menu::setReturnQueryStrref(uint32 id) {
	_menuMap->setReturnQueryStrref(id);
}

void Menu::setReturnEnabled(bool enabled) {
	_menuMap->setReturnEnabled(enabled);
}

void Menu::showEquipment() {
	if (_currentMenu != _menuEqu.get()) {
		if (_currentMenu)
			removeChild(_currentMenu);
		_currentMenu = _menuEqu.get();
		addChild(_menuEqu.get());
		_protoEqu->setHighlight(true);

		if (_lastProto)
			_lastProto->setHighlight(false);
		_protoEqu->setHighlight(true);
		_lastProto = _protoEqu;
	}
}

void Menu::showInventory() {
	if (_currentMenu != _menuInv.get()) {
		if (_currentMenu)
			removeChild(_currentMenu);
		_currentMenu = _menuInv.get();
		addChild(_menuInv.get());
		_protoInv->setHighlight(true);

		if (_lastProto)
			_lastProto->setHighlight(false);
		_protoInv->setHighlight(true);
		_lastProto = _protoInv;
	}
}

void Menu::showCharacter() {
	if (_currentMenu != _menuChar.get()) {
		if (_currentMenu)
			removeChild(_currentMenu);
		_currentMenu = _menuChar.get();
		addChild(_menuChar.get());
		_protoChar->setHighlight(true);

		if (_lastProto)
			_lastProto->setHighlight(false);
		_protoChar->setHighlight(true);
		_lastProto = _protoChar;
	}
}

void Menu::showAbilities() {
	if (_currentMenu != _menuAbi.get()) {
		if (_currentMenu)
			removeChild(_currentMenu);
		_currentMenu = _menuAbi.get();
		addChild(_menuAbi.get());

		if (_lastProto)
			_lastProto->setHighlight(false);
		_protoAbi->setHighlight(true);
		_lastProto = _protoAbi;
	}
}

void Menu::showMessages() {
	if (_currentMenu != _menuMsg.get()) {
		if (_currentMenu)
			removeChild(_currentMenu);
		_currentMenu = _menuMsg.get();
		addChild(_menuMsg.get());

		if (_lastProto)
			_lastProto->setHighlight(false);
		_protoMsg->setHighlight(true);
		_lastProto = _protoMsg;
	}
}

void Menu::showJournal() {
	if (_currentMenu != _menuJou.get()) {
		if (_currentMenu)
			removeChild(_currentMenu);
		_currentMenu = _menuJou.get();
		addChild(_menuJou.get());
		_protoJou->setHighlight(true);

		if (_lastProto)
			_lastProto->setHighlight(false);
		_protoJou->setHighlight(true);
		_lastProto = _protoJou;
	}
}

void Menu::showMap() {
	if (_currentMenu != _menuMap.get()) {
		if (_currentMenu)
			removeChild(_currentMenu);
		_currentMenu = _menuMap.get();
		addChild(_menuMap.get());
		_protoMap->setHighlight(true);

		if (_lastProto)
			_lastProto->setHighlight(false);
		_protoMap->setHighlight(true);
		_lastProto = _protoMap;
	}
}

void Menu::showOptions() {
	if (_currentMenu != _menuOpt.get()) {
		if (_currentMenu)
			removeChild(_currentMenu);
		_currentMenu = _menuOpt.get();
		addChild(_menuOpt.get());
		_protoOpt->setHighlight(true);

		if (_lastProto)
			_lastProto->setHighlight(false);
		_protoOpt->setHighlight(true);
		_lastProto = _protoOpt;
	}
}

void Menu::callbackActive(Widget &widget) {
	if (widget.getTag() == "BTN_EQU") {
		showEquipment();
		return;
	}
	if (widget.getTag() == "BTN_INV") {
		showInventory();
		return;
	}
	if (widget.getTag() == "BTN_CHAR") {
		showCharacter();
		return;
	}
	if (widget.getTag() == "BTN_ABI") {
		showAbilities();
		return;
	}
	if (widget.getTag() == "BTN_MSG") {
		showMessages();
		return;
	}
	if (widget.getTag() == "BTN_JOU") {
		showJournal();
		return;
	}
	if (widget.getTag() == "BTN_MAP") {
		showMap();
		return;
	}
	if (widget.getTag() == "BTN_OPT") {
		showOptions();
		return;
	}
}

} // End of namespace KotOR

} // End of namespace Engines
