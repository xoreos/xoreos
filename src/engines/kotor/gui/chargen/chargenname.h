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
 *  The menu for modifying the name of the character.
 */

#ifndef ENGINES_KOTOR_GUI_CHARGEN_CHARGENNAME_H
#define ENGINES_KOTOR_GUI_CHARGEN_CHARGENNAME_H

#include "src/aurora/ltrfile.h"

#include "src/engines/kotorbase/gui/gui.h"

#include "src/engines/kotor/gui/chargen/chargenbase.h"

namespace Engines {

namespace KotOR {

class CharacterGenerationNameMenu : public CharacterGenerationBaseMenu {
public:
	CharacterGenerationNameMenu(KotORBase::CharacterGenerationInfo &info,
	                            ::Engines::Console *console = 0);

private:
	Aurora::LTRFile _humanFirst;
	Aurora::LTRFile _humanLast;

	Odyssey::WidgetLabel *_nameLabel;
	Common::UString _name;

	void callbackActive(Widget &widget);
	void callbackTextInput(const Common::UString &text);
	void callbackKeyInput(const Events::Key &key, const Events::EventType &type);
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_GUI_CHARGEN_CHARGENNAME_H
