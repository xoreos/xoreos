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
 *  The spell school chooser in CharGen.
 */

#include "src/aurora/talkman.h"

#include "src/engines/nwn/gui/widgets/button.h"
#include "src/engines/nwn/gui/widgets/editbox.h"
#include "src/engines/nwn/gui/widgets/buttonsgroup.h"

#include "src/engines/nwn/gui/chargen/charschool.h"

namespace Engines {

namespace NWN {

CharSchool::CharSchool(CharGenChoices &choices, Console *console) : CharGenBase(console) {
	_choices = &choices;
	load("cg_school");

	// Init buttons and helpbox.
	_buttons = std::make_unique<ButtonsGroup>(getEditBox("HelpBox", true));

	uint32_t textID = 10320;
	uint32_t titleID = 373;
	_buttons->addButton(getButton("General", true), TalkMan.getString(5633), TalkMan.getString(5634));
	_buttons->addButton(getButton("Abjuration", true), TalkMan.getString(titleID), TalkMan.getString(textID));
	_buttons->addButton(getButton("Conjuration", true), TalkMan.getString(++titleID), TalkMan.getString(++textID));
	_buttons->addButton(getButton("Divination", true), TalkMan.getString(++titleID), TalkMan.getString(++textID));
	_buttons->addButton(getButton("Enchantment", true), TalkMan.getString(++titleID), TalkMan.getString(++textID));
	_buttons->addButton(getButton("Evocation", true), TalkMan.getString(++titleID), TalkMan.getString(++textID));
	_buttons->addButton(getButton("Illusion", true), TalkMan.getString(++titleID), TalkMan.getString(++textID));
	_buttons->addButton(getButton("Necromancy", true), TalkMan.getString(++titleID), TalkMan.getString(++textID));
	_buttons->addButton(getButton("Transmutation", true), TalkMan.getString(++titleID), TalkMan.getString(++textID));

	reset();
}

CharSchool::~CharSchool() {
}

void CharSchool::reset() {
	_buttons->setActive(getButton("General", true));

	getEditBox("HelpBox", true)->setTitle("fnt_galahad14", TalkMan.getString(381));
	getEditBox("HelpBox", true)->setText("fnt_galahad14", TalkMan.getString(381), 1.0f);

	_choices->setSpellSchool(UINT8_MAX);
}

void CharSchool::hide() {
	Engines::GUI::hide();

	if (_returnCode == 1) {
		// Set previous choice if any.
		if (_choices->getSpellSchool() < UINT8_MAX)
			_buttons->setActive((size_t) _choices->getSpellSchool());
	}
}

void CharSchool::callbackActive(Widget &widget) {
	if (widget.getTag() == "OkButton") {
		_choices->setSpellSchool((uint8_t) _buttons->getChoice());
		_returnCode = 2;
		return;
	}

	if (widget.getTag() == "CancelButton") {
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "RecommendButton") {
		_buttons->setActive(size_t(0));
		return;
	}

	_buttons->setActive(dynamic_cast<WidgetButton *>(&widget));
}

} // End of namespace NWN

} // End of namespace Engines
