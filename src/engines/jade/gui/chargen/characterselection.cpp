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

#include "src/aurora/2dareg.h"

#include "src/engines/jade/gui/chargen/characterselection.h"

#include "src/engines/odyssey/widget.h"
#include "src/engines/odyssey/label.h"
#include "src/engines/odyssey/button.h"

namespace Engines {

namespace Jade {

CharacterSelection::CharacterSelection(CharacterGeneration &chargen, CharacterInfo &charInfo) :
	CharacterGenerationBase(chargen, charInfo) {
	load("char_select2");

	_nameButton = getButton("bNameStr");

	_defaultCharacters = CharacterInfo::getDefaultCharacterInfos();

	_characterId = 0;
	updateCharacterInfo();
}

void CharacterSelection::updateCharacterInfo() {
	if (_nameButton)
		_nameButton->setText(_defaultCharacters[_characterId].getName());
}

void CharacterSelection::initWidget(Widget &widget) {
	if (widget.getTag() == "ButtonHelp")
		widget.setInvisible(true);
}

void CharacterSelection::callbackActive(Widget &widget) {
	if (widget.getTag() == "ButtonBack") {
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "ButtonAccept") {
		_charInfo = _defaultCharacters[_characterId];
		_chargen.showName();
		_chargen.setCustom(false);
		return;
	}
}

} // End of namespace Jade

} // End of namespace Engines
