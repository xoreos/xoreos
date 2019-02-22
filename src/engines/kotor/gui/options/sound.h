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
 *  The sound menu.
 */

#ifndef ENGINES_KOTOR_GUI_OPTIONS_SOUND_H
#define ENGINES_KOTOR_GUI_OPTIONS_SOUND_H

#include "src/common/scopedptr.h"

#include "src/engines/kotorbase/gui/gui.h"

namespace Engines {

namespace KotOR {

class OptionsSoundMenu : public KotORBase::GUI {
public:
	OptionsSoundMenu(::Engines::Console *console = 0);
	~OptionsSoundMenu();

protected:
	void callbackActive(Widget &widget);

private:
	Common::ScopedPtr<KotORBase::GUI> _advanced;
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_GUI_OPTIONS_SOUND_H
