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
 *  Common base for in-game menus in KotOR games.
 */

#ifndef ENGINES_KOTORBASE_GUI_MENUBASE_H
#define ENGINES_KOTORBASE_GUI_MENUBASE_H

#include "src/engines/kotorbase/gui/gui.h"

namespace Engines {

namespace KotORBase {

class Module;

class MenuBase : public GUI {
public:
	MenuBase(Module &module, Console *console = nullptr);
	virtual ~MenuBase();

	virtual void update();

protected:
	Module *_module;
	
	virtual void callbackActive(Widget &widget);

	void updatePartyLeader(const Common::UString &widgetTag);

private:
	void updatePartyMember(int index);
};

} // End of namespace KotORBase

} // End of namespace Engines

#endif // ENGINES_KOTORBASE_GUI_MENUBASE_H
