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
 *  The gender chooser in CharGen.
 */

#include "src/aurora/talkman.h"

#include "src/engines/nwn/creature.h"

#include "src/engines/nwn/gui/widgets/button.h"
#include "src/engines/nwn/gui/widgets/buttonsgroup.h"
#include "src/engines/nwn/gui/widgets/editbox.h"

#include "src/engines/nwn/gui/chargen/charsex.h"

namespace Engines {

namespace NWN {

CharSex::CharSex(CharGenChoices &choices, ::Engines::Console *console) : CharGenBase(console) {
	_choices = &choices;
	load("cg_sex");

	getEditBox("HelpBox", true)->setTitle("fnt_galahad14", TalkMan.getString(203));
	getEditBox("HelpBox", true)->setText("fnt_galahad14", TalkMan.getString(447), 1.0f);

	_buttons.reset(new ButtonsGroup(getEditBox("HelpBox", true)));
	_buttons->addButton(getButton("MaleButton", true), TalkMan.getString(203), TalkMan.getString(199));
	_buttons->addButton(getButton("FemaleButton", true), TalkMan.getString(203), TalkMan.getString(200));

	getButton("MaleButton", true)->setPressed(true);
}

CharSex::~CharSex() {
}

void CharSex::reset() {
	_buttons->setActive(getButton("MaleButton", true));

	getEditBox("HelpBox", true)->setTitle("fnt_galahad14", TalkMan.getString(203));
	getEditBox("HelpBox", true)->setText("fnt_galahad14", TalkMan.getString(447), 1.0f);
}

void CharSex::hide() {
	Engines::GUI::hide();

	if (_returnCode == 1) {
		// Set previous choice if any.
		Gender previousGender = _choices->getCharacter().getGender();

		if (previousGender != kGenderNone)
			_buttons->setActive((size_t) previousGender);
		else
			reset();
	}
}

void CharSex::callbackActive(Widget &widget) {
	if (widget.getTag() == "OkButton") {
		_choices->setGender((Gender) _buttons->getChoice());
		_returnCode = 2;
		return;
	}

	if (widget.getTag() == "CancelButton") {
		_returnCode = 1;
		return;
	}

	_buttons->setActive(dynamic_cast<WidgetButton *>(&widget));
}

} // End of namespace NWN

} // End of namespace Engines
