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
 *  The ingame GUI.
 */

#ifndef ENGINES_KOTOR2_GUI_INGAME_INGAME_H
#define ENGINES_KOTOR2_GUI_INGAME_INGAME_H

#include "src/engines/aurora/console.h"

#include "src/engines/kotor2/module.h"

namespace Engines {

namespace KotOR2 {

class IngameGUI {
public:
	IngameGUI(Module &module, ::Engines::Console *console = 0);

	void show(); ///< Show all ingame elements.
	void hide(); ///< Hide all ingame elements.

	void addEvent(const Events::Event &event);
	void processEventQueue();
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR2_GUI_INGAME_INGAME_H
