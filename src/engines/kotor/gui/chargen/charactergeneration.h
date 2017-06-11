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
 *  The primary character generation menu.
 */

#ifndef ENGINES_KOTOR_GUI_CHARGEN_CHARACTERGENERATION_H
#define ENGINES_KOTOR_GUI_CHARGEN_CHARACTERGENERATION_H

#include "src/engines/kotor/module.h"
#include "src/engines/kotor/gui/gui.h"
#include "src/engines/kotor/gui/chargen/classselection.h"

namespace Engines {

namespace KotOR {

class CharacterGenerationMenu : public GUI {
public:
	CharacterGenerationMenu(Module *module, ::Engines::Console *console = 0);
	~CharacterGenerationMenu();

	void showQuickOrCustom();
	void showQuick();
	void showCustom();

private:
	Common::ScopedPtr<GUI> _quickOrCustom;
	Common::ScopedPtr<GUI> _quickChar;
	Common::ScopedPtr<GUI> _customChar;
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_GUI_CHARGEN_CHARACTERGENERATION_H
