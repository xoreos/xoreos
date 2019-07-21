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
 *  Character generation name menu.
 */

#include "src/aurora/talkman.h"

#include "src/events/events.h"

#include "src/engines/jade/gui/chargen/charactername.h"

#include "src/engines/odyssey/widget.h"

namespace Engines {

namespace Jade {

CharacterName::CharacterName(CharacterGeneration &chargen, CharacterInfo &charInfo) :
	CharacterGenerationBase(chargen, charInfo) {
	load("keyboard");

	_labelText = getLabel("LabelText");
}

void CharacterName::callbackActive(Widget &widget) {
	if (widget.getTag() == "ButtonDefault") {
		_charInfo.setName(_charInfo.getDefaultName());
		updateName();
		return;
	}

	if (widget.getTag() == "ButtonBack") {
		_charInfo.setName(_charInfo.getDefaultName());
		_chargen.showSelection();

		EventMan.enableTextInput(false);
		EventMan.enableKeyRepeat(false);

		return;
	}

	if (widget.getTag() == "ButtonDone") {
		_chargen.showSummary();

		EventMan.enableTextInput(false);
		EventMan.enableKeyRepeat(false);

		return;
	}

	if (widget.getTag() == "bClear") {
		_charInfo.setName("");
		updateName();

		return;
	}
}

void CharacterName::initWidget(Widget &widget) {
	if (widget.getTag() == "TitleLabel")
		reinterpret_cast<Odyssey::WidgetLabel &>(widget).setText(TalkMan.getString(301));
}

void CharacterName::updateName() {
	_labelText->setText(_charInfo.getName() + "_");
}

void CharacterName::show() {
	updateName();

	EventMan.enableTextInput(true);
	EventMan.enableKeyRepeat(true);

	GUI::show();
}

void CharacterName::callbackTextInput(const Common::UString &text) {
	if (_charInfo.getName().size() >= 20)
		return;

	_charInfo.setName(_charInfo.getName() + text);
	updateName();
}

void CharacterName::callbackKeyInput(const Events::Key &key, const Events::EventType &type) {
	if (key == Events::kKeyBackspace && type == Events::kEventKeyDown) {
		Common::UString name = _charInfo.getName();
		if (!name.empty()) {
			name.erase(--name.end());
			_charInfo.setName(name);
			updateName();
		}
	}
}

} // End of namespace Jade

} // End of namespace Engines
