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

#include <cassert>

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

	_menu[kMenuTypeEquipment].menu = std::make_unique<MenuEquipment>(_module, console);
	_menu[kMenuTypeInventory].menu = std::make_unique<MenuInventory>(_module, console);
	_menu[kMenuTypeCharacter].menu = std::make_unique<MenuCharacter>(_module, console);
	_menu[kMenuTypeAbilities].menu = std::make_unique<MenuAbilities>(_module, console);
	_menu[kMenuTypeMessages].menu = std::make_unique<MenuMessages>(console);
	_menu[kMenuTypeJournal].menu = std::make_unique<MenuJournal>(console);
	_menu[kMenuTypeMap].menu = std::make_unique<MenuMap>(console);
	_menu[kMenuTypeOptions].menu = std::make_unique<MenuOptions>(console);

	_menuLoad = std::make_unique<SaveLoadMenu>(module, console, kSaveLoadMenuTypeLoad, true);
	_menuSave = std::make_unique<SaveLoadMenu>(module, console, kSaveLoadMenuTypeSave, true);
}

void Menu::setReturnStrref(uint32_t id) {
	dynamic_cast<MenuMap &>(*_menu[kMenuTypeMap].menu.get()).setReturnStrref(id);
}

void Menu::setReturnQueryStrref(uint32_t id) {
	dynamic_cast<MenuMap &>(*_menu[kMenuTypeMap].menu.get()).setReturnQueryStrref(id);
}

void Menu::setReturnEnabled(bool enabled) {
	dynamic_cast<MenuMap &>(*_menu[kMenuTypeMap].menu.get()).setReturnEnabled(enabled);
}

void Menu::showMenu(MenuType type) {
	assert((type >= 0) && (type < kMenuTypeMAX));

	KotORBase::MenuBase *menuBase = dynamic_cast<KotORBase::MenuBase *>(_menu[type].menu.get());
	if (menuBase)
		menuBase->update();

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
		const uint8_t selectedItem = dynamic_cast<MenuOptions &>(*_currentMenu->menu.get()).pollSelectedItem();

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

bool Menu::isMenuImplemented(const Common::UString &tag) {
	return getMenuTypeByButtonTag(tag) != kMenuTypeMAX;
}

Menu::MenuType Menu::getMenuTypeByButtonTag(const Common::UString &tag) {
	for (size_t i = 0; i < kMenuTypeMAX; i++) {
		if (_menu[i].button && (_menu[i].button->getTag() == tag))
			return static_cast<MenuType>(i);
	}

	return kMenuTypeMAX;
}

void Menu::showMenu(const Common::UString &tag) {
	MenuType menuType = getMenuTypeByButtonTag(tag);
	if (menuType != kMenuTypeMAX)
		showMenu(menuType);
}

void Menu::callbackActive(Widget &widget) {
	showMenu(widget.getTag());
}

} // End of namespace KotOR

} // End of namespace Engines
