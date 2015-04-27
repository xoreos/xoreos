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
 *  The new game menu, expansion 2.
 */

#ifndef ENGINES_NWN_GUI_MAIN_NEWXP2_H
#define ENGINES_NWN_GUI_MAIN_NEWXP2_H

#include "src/engines/nwn/gui/gui.h"

namespace Engines {

namespace NWN {

class Module;

/** The NWN new game menu, expansion 2. */
class NewXP2Menu : public GUI {
public:
	NewXP2Menu(Module &module, GUI &charType, ::Engines::Console *console = 0);
	~NewXP2Menu();

protected:
	void initWidget(Widget &widget);
	void callbackActive(Widget &widget);

private:
	Module *_module;

	GUI *_charType;

	void loadModule(const Common::UString &module);
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_MAIN_NEWXP2_H
