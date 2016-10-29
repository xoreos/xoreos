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
 *  The NWN character generator.
 */

#ifndef ENGINES_NWN_GUI_CHARGEN_CHARGEN_H
#define ENGINES_NWN_GUI_CHARGEN_CHARGEN_H

#include <vector>

#include "src/common/ptrvector.h"

#include "src/engines/nwn/gui/chargen/chargenbase.h"

namespace Engines {

namespace NWN {

class Module;

/** The NWN character generator. */
class CharGenMenu : public CharGenBase {
public:
	CharGenMenu(Module &module, ::Engines::Console *console = 0);
	~CharGenMenu();

	void reset();

protected:
	void callbackActive(Widget &widget);

private:
	void init();

	Module *_module;
	std::vector<WidgetButton *> _charButtons;

	Common::PtrVector<CharGenBase> _chargenGuis;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_CHARGEN_CHARGEN_H
