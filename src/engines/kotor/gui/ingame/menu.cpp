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

#include "src/graphics/graphics.h"

#include "src/engines/odyssey/button.h"

#include "src/engines/kotor/gui/ingame/menu.h"

namespace Engines {

namespace KotOR {

Menu::Menu(KotORBase::Module &module, Console *console) :
		KotORBase::GUI(console),
		_module(module),
		_currentMenu(0) {

	load("top");

	addBackground(KotORBase::kBackgroundTypeMenu, true);

	static const char * const kButtonTags[kMenuTypeMAX] = {
		"BTN_EQU" , "BTN_INV" , "BTN_CHAR", "BTN_ABI" , "BTN_MSG" , "BTN_JOU" , "BTN_MAP" , "BTN_OPT"
	};
	static const char * const kProtoTags[kMenuTypeMAX] = {
		"LBLH_EQU", "LBLH_INV", "LBLH_CHA", "LBLH_ABI", "LBLH_MSG", "LBLH_JOU", "LBLH_MAP", "LBLH_OPT"
	};

	for (size_t i = 0; i < kMenuTypeMAX; i++) {
		SubMenu &menu = _menu[i];

		// Place the widget as the top bar
		menu.protoItem = getProtoItem(kProtoTags[i]);
		if (menu.protoItem) {
			float x, y, z;

			menu.protoItem->getPosition(x, y, z);
			menu.protoItem->setPosition(x, y + 197, z);
		}

		// Do the same for the button
		menu.button = getButton(kButtonTags[i]);
		if (menu.button) {
			float x, y, z;

			menu.button->getPosition(x, y, z);
			menu.button->setPosition(x, y + 197, z);
		}
	}

	_menu[kMenuTypeEquipment].menu.reset(new MenuEquipment(_module, console));
	_menu[kMenuTypeInventory].menu.reset(new MenuInventory(console));
	_menu[kMenuTypeCharacter].menu.reset(new MenuCharacter(console));
	_menu[kMenuTypeAbilities].menu.reset(new MenuAbilities(console));
	_menu[kMenuTypeMessages].menu.reset(new MenuMessages(console));
	_menu[kMenuTypeJournal].menu.reset(new MenuJournal(console));
	_menu[kMenuTypeMap].menu.reset(new MenuMap(console));
	_menu[kMenuTypeOptions].menu.reset(new MenuOptions(console));

	_menuLoad.reset(new SaveLoadMenu(module, console, kSaveLoadMenuTypeLoad, true));
	_menuSave.reset(new SaveLoadMenu(module, console, kSaveLoadMenuTypeSave, true));
}

void Menu::setReturnStrref(uint32 id) {
	dynamic_cast<MenuMap &>(*_menu[kMenuTypeMap].menu.get()).setReturnStrref(id);
}

void Menu::setReturnQueryStrref(uint32 id) {
	dynamic_cast<MenuMap &>(*_menu[kMenuTypeMap].menu.get()).setReturnQueryStrref(id);
}

void Menu::setReturnEnabled(bool enabled) {
	dynamic_cast<MenuMap &>(*_menu[kMenuTypeMap].menu.get()).setReturnEnabled(enabled);
}

void Menu::showMenu(MenuType type) {
	assert((type >= 0) && (type < kMenuTypeMAX));

	if (type == kMenuTypeEquipment) {
		MenuEquipment *equipment = dynamic_cast<MenuEquipment *>(_menu[type].menu.get());
		if (equipment)
			equipment->refresh();
	}

	if (_currentMenu == &_menu[type])
		return;

	if (_currentMenu) {
		removeChild(_currentMenu->menu.get());
		_currentMenu->protoItem->setHighlight(false);
	}

	_currentMenu = &_menu[type];
	addChild(_currentMenu->menu.get());
	_currentMenu->protoItem->setHighlight(true);
}

void Menu::callbackRun() {
	if (_currentMenu == &_menu[kMenuTypeOptions]) {
		const uint8 selectedItem = dynamic_cast<MenuOptions &>(*_currentMenu->menu.get()).pollSelectedItem();

		switch (selectedItem) {
			case kOptionsItemLoadGame:
				if (sub(*_menuLoad) == 2) {
					GfxMan.unlockFrame();
					_returnCode = 2;
				}
				break;
			case kOptionsItemSaveGame:
				if (sub(*_menuSave) == 2)
					_returnCode = 2;
				break;
			case kOptionsItemExitGame:
				_module.leave();
				_returnCode = 1;
				break;
		}
	}
}

void Menu::showMenu(const Common::UString &tag) {
	for (size_t i = 0; i < kMenuTypeMAX; i++) {
		if (_menu[i].button && _menu[i].button->getTag() == tag) {
			showMenu(static_cast<MenuType>(i));
			return;
		}
	}
}

void Menu::callbackActive(Widget &widget) {
	showMenu(widget.getTag());
}

} // End of namespace KotOR

} // End of namespace Engines
