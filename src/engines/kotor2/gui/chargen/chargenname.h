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
 *  The KotOR 2 character generation name menu.
 */

#ifndef ENGINES_KOTOR2_GUI_CHARGEN_CHARGENNAME_H
#define ENGINES_KOTOR2_GUI_CHARGEN_CHARGENNAME_H

#include "src/aurora/ltrfile.h"

#include "src/engines/aurora/console.h"

#include "src/engines/odyssey/label.h"

#include "src/engines/kotor2/gui/chargen/chargenbase.h"
#include "src/engines/kotor2/gui/chargen/chargeninfo.h"

namespace Engines {

namespace KotOR2 {

class CharacterGenerationNameMenu : public CharacterGenerationBaseMenu {
public:
	CharacterGenerationNameMenu(CharacterGenerationInfo &info, ::Engines::Console *console = 0);

private:
	virtual void callbackActive(Widget &widget);
	virtual void callbackTextInput(const Common::UString &text);
	virtual void callbackKeyInput(const Events::Key &key, const Events::EventType &type);

	Aurora::LTRFile _humanFirst;
	Aurora::LTRFile _humanLast;

	Common::UString _name;
	Odyssey::WidgetLabel *_nameLabel;
};

} // End of namespace KotOR2

} // End of namespace Engines

#endif // ENGINES_KOTOR2_GUI_CHARGEN_CHARGENNAME_H
