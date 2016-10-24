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
 *  The new campaign menu.
 */

#ifndef ENGINES_NWN_GUI_MAIN_NEWCAMP_H
#define ENGINES_NWN_GUI_MAIN_NEWCAMP_H

#include "src/common/scopedptr.h"

#include "src/engines/nwn/gui/gui.h"

namespace Engines {

namespace NWN {

class Module;

/** The NWN new campaign menu. */
class NewCampMenu : public GUI {
public:
	NewCampMenu(Module &module, GUI &charType, ::Engines::Console *console = 0);
	~NewCampMenu();

protected:
	void callbackActive(Widget &widget);

private:
	Module *_module;

	GUI *_charType;

	Common::ScopedPtr<GUI> _base;
	Common::ScopedPtr<GUI> _xp1;
	Common::ScopedPtr<GUI> _xp2;
	Common::ScopedPtr<GUI> _modules;
	Common::ScopedPtr<GUI> _premium;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_MAIN_NEWCAMP_H
