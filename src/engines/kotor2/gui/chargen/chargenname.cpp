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

#include "src/events/events.h"

#include "src/engines/odyssey/label.h"

#include "src/engines/kotor2/gui/chargen/chargenname.h"

namespace Engines {

namespace KotOR2 {

CharacterGenerationNameMenu::CharacterGenerationNameMenu(CharacterGenerationInfo &info, Engines::Console *console) :
		CharacterGenerationBaseMenu(info, console),
		_humanFirst(info.getGender() == KotOR::kGenderMale ? "humanm" : "humanf"), _humanLast("humanl"),
		_name(_info.getName()) {
	load("name_p");

	_nameLabel = getLabel("NAME_BOX_EDIT");
	_nameLabel->setText(_info.getName() + "_");

	EventMan.enableTextInput(true);
	EventMan.enableKeyRepeat(true);
}

void CharacterGenerationNameMenu::callbackActive(Widget &widget) {
	if (widget.getTag() == "BTN_RANDOM") {
		_name = _humanFirst.generateRandomName(8) + " " + _humanLast.generateRandomName(8);
		_nameLabel->setText(_name + "_");
		_info.setName(_name);
		return;
	}

	if (widget.getTag() == "BTN_BACK") {
		_returnCode = kReturnCodeAbort;
		EventMan.enableTextInput(false);
		EventMan.enableKeyRepeat(false);
		return;
	}
	if (widget.getTag() == "END_BTN") {
		accept();
		_returnCode = kReturnCodeAbort;
		EventMan.enableTextInput(false);
		EventMan.enableKeyRepeat(false);
		return;
	}
}

void CharacterGenerationNameMenu::callbackKeyInput(const Events::Key &key, const Events::EventType &type) {
	if (key == Events::kKeyBackspace && type == Events::kEventKeyDown) {
		if (!_name.empty()) {
			_name.erase(--_name.end());
			_nameLabel->setText(_name + "_");
			_info.setName(_name);
		}
	}
}

void CharacterGenerationNameMenu::callbackTextInput(const Common::UString &text) {
	// The name should not be longer than 18 letters (according to the original game).
	if (_name.size() == 18) {
		return;
	}

	_name += text;
	_info.setName(_name);
	_nameLabel->setText(_name + "_");
}

} // End of namespace KotOR

} // End of namespace Engines
