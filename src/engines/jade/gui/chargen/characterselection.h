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
 *  Character generation selection menu.
 */

#ifndef ENGINES_JADE_GUI_CHARGEN_CHARACTERSELECTION_H
#define ENGINES_JADE_GUI_CHARGEN_CHARACTERSELECTION_H

#include "src/engines/jade/gui/gui.h"
#include "src/engines/jade/gui/chargen/characterinfo.h"
#include "src/engines/jade/gui/chargen/charactergeneration.h"
#include "src/engines/jade/gui/chargen/charactergenerationbase.h"

namespace Engines {

namespace Jade {

class CharacterSelection : public CharacterGenerationBase {
public:
	CharacterSelection(CharacterGeneration &chargen, CharacterInfo &charInfo);

protected:
	void callbackActive(Widget &widget) override;

	void initWidget(Widget &widget) override;

private:
	void updateCharacterInfo();

	Odyssey::WidgetButton *_nameButton;

	size_t _characterId;
	std::vector<CharacterInfo> _defaultCharacters;
};

}

}

#endif // ENGINES_JADE_GUI_CHARGEN_CHARACTERSELECTION_H
