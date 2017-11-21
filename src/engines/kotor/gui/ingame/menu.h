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

private:
	void callbackActive(Widget &widget);

	WidgetProtoItem *_protoEqu;
	WidgetProtoItem *_protoInv;
	WidgetProtoItem *_protoChar;
	WidgetProtoItem *_protoAbi;
	WidgetProtoItem *_protoMsg;
	WidgetProtoItem *_protoJou;
	WidgetProtoItem *_protoMap;
	WidgetProtoItem *_protoOpt;
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_GUI_INGAME_MENU_H
