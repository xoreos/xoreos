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

#include "src/engines/kotor/gui/gui.h"
#include "src/engines/kotor/gui/widgets/protoitem.h"

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

class Menu : public GUI {
public:
	Menu(::Engines::Console *console = 0);

	/** Set the return string ref. */
	void setReturnStrref(uint32);
	/** Set the return query string ref. */
	void setReturnQueryStrref(uint32);
	/** Set the return button enabled/disabled. */
	void setReturnEnabled(bool);

	/** Show the equipment menu */
	void showEquipment();
	/** Show the inventory menu */
	void showInventory();
	/** Show the character menu */
	void showCharacter();
	/** Show the abilities menu */
	void showAbilities();
	/** Show the messages menu */
	void showMessages();
	/** Show the journal menu */
	void showJournal();
	/** Show the map menu */
	void showMap();
	/** Show the options menu */
	void showOptions();

private:
	void callbackActive(Widget &widget);

	GUI* _currentMenu;

	Common::ScopedPtr<MenuEquipment> _menuEqu;
	Common::ScopedPtr<MenuInventory> _menuInv;
	Common::ScopedPtr<MenuCharacter> _menuChar;
	Common::ScopedPtr<MenuAbilities> _menuAbi;
	Common::ScopedPtr<MenuMessages> _menuMsg;
	Common::ScopedPtr<MenuJournal> _menuJou;
	Common::ScopedPtr<MenuMap> _menuMap;
	Common::ScopedPtr<MenuOptions> _menuOpt;

	WidgetProtoItem *_protoEqu;
	WidgetProtoItem *_protoInv;
	WidgetProtoItem *_protoChar;
	WidgetProtoItem *_protoAbi;
	WidgetProtoItem *_protoMsg;
	WidgetProtoItem *_protoJou;
	WidgetProtoItem *_protoMap;
	WidgetProtoItem *_protoOpt;

	WidgetProtoItem *_lastProto;
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_GUI_INGAME_MENU_H
