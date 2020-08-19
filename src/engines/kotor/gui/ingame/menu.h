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

#ifndef ENGINES_KOTOR_GUI_INGAME_MENU_H
#define ENGINES_KOTOR_GUI_INGAME_MENU_H

#include "src/engines/aurora/console.h"

#include "src/engines/odyssey/protoitem.h"

#include "src/engines/kotorbase/gui/gui.h"

#include "src/engines/kotor/gui/saveload.h"

#include "src/engines/kotor/gui/ingame/menu_equ.h"
#include "src/engines/kotor/gui/ingame/menu_inv.h"
#include "src/engines/kotor/gui/ingame/menu_char.h"
#include "src/engines/kotor/gui/ingame/menu_abi.h"
#include "src/engines/kotor/gui/ingame/menu_msg.h"
#include "src/engines/kotor/gui/ingame/menu_jou.h"
#include "src/engines/kotor/gui/ingame/menu_map.h"
#include "src/engines/kotor/gui/ingame/menu_opt.h"

namespace Engines {

namespace KotOR {

class Menu : public KotORBase::GUI {
public:
	Menu(KotORBase::Module &module, ::Engines::Console *console = 0);

	/** Set the return string ref. */
	void setReturnStrref(uint32_t);
	/** Set the return query string ref. */
	void setReturnQueryStrref(uint32_t);
	/** Set the return button enabled/disabled. */
	void setReturnEnabled(bool);

	bool isMenuImplemented(const Common::UString& tag);
	void showMenu(const Common::UString &tag);

private:
	enum MenuType {
		kMenuTypeEquipment,
		kMenuTypeInventory,
		kMenuTypeCharacter,
		kMenuTypeAbilities,
		kMenuTypeMessages,
		kMenuTypeJournal,
		kMenuTypeMap,
		kMenuTypeOptions,

		kMenuTypeMAX
	};

	struct SubMenu {
		Odyssey::WidgetProtoItem *protoItem;
		Odyssey::WidgetButton *button;

		std::unique_ptr<KotORBase::GUI> menu;

		SubMenu() : protoItem(0), button(0) { }
	};

	void callbackRun();
	void callbackActive(Widget &widget);

	KotORBase::Module &_module;

	SubMenu _menu[kMenuTypeMAX];
	SubMenu *_currentMenu;

	std::unique_ptr<SaveLoadMenu> _menuLoad;
	std::unique_ptr<SaveLoadMenu> _menuSave;

	MenuType getMenuTypeByButtonTag(const Common::UString &tag);
	void showMenu(MenuType type);
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_GUI_INGAME_MENU_H
