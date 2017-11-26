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

namespace Engines {

namespace KotOR {

class Menu : public GUI {
public:
	Menu(::Engines::Console *console = 0);

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

	Common::ScopedPtr<GUI> _menuEqu;
	Common::ScopedPtr<GUI> _menuInv;
	Common::ScopedPtr<GUI> _menuChar;
	Common::ScopedPtr<GUI> _menuAbi;
	Common::ScopedPtr<GUI> _menuMsg;
	Common::ScopedPtr<GUI> _menuJou;
	Common::ScopedPtr<GUI> _menuMap;
	Common::ScopedPtr<GUI> _menuOpt;

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
