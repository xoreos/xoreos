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
 *  The premade character chooser.
 */

#include <cstdio>

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/string.h"

#include "src/events/requests.h"

#include "src/graphics/aurora/fontman.h"
#include "src/graphics/aurora/text.h"
#include "src/graphics/aurora/model.h"

#include "src/engines/aurora/model.h"

#include "src/engines/nwn/creature.h"
#include "src/engines/nwn/game.h"
#include "src/engines/nwn/module.h"

#include "src/engines/nwn/gui/widgets/listbox.h"
#include "src/engines/nwn/gui/widgets/portrait.h"

#include "src/engines/nwn/gui/chargen/chargen.h"

#include "src/engines/nwn/gui/main/charpremade.h"

namespace Engines {

namespace NWN {

WidgetListItemCharacter::WidgetListItemCharacter(::Engines::GUI &gui, const Common::UString &font,
                                                 const Common::UString &name,
                                                 const Common::UString &classes,
                                                 const Common::UString &portrait, float spacing) :
                                                 WidgetListItemBaseButton(gui, "ctl_pre_btn_char",
                                                                          spacing) {

	_portrait  = std::make_unique<Portrait>(portrait, Portrait::kSizeTiny, 1.0f);
	_textName  = std::make_unique<Graphics::Aurora::Text>(FontMan.get(font), name);
	_textClass = std::make_unique<Graphics::Aurora::Text>(FontMan.get(font), classes);
}

WidgetListItemCharacter::~WidgetListItemCharacter() {
}

void WidgetListItemCharacter::show() {
	WidgetListItemBaseButton::show();

	_textName->show();
	_textClass->show();
	_portrait->show();
}

void WidgetListItemCharacter::hide() {
	WidgetListItemBaseButton::hide();

	_textClass->hide();
	_textName->hide();
	_portrait->hide();
}

void WidgetListItemCharacter::setPosition(float x, float y, float z) {
	WidgetListItemBaseButton::setPosition(x, y, z);

	getPosition(x, y, z);

	z -= 5.0f;
	_portrait->setPosition(x + 8.0f, y + 7.0f, z);

	x += 32.0f;

	_textName->setPosition (x, y + getHeight() -     _textName->getHeight() - 4.0f, z);
	_textClass->setPosition(x, y + getHeight() - 2 * _textName->getHeight() - 4.0f, z);
}

bool CharPremadeMenu::Character::operator<(const Character &c) const {
	int cmp = name.strcmp(c.name);

	if (cmp < 0)
		return true;
	if (cmp > 0)
		return false;

	return number < c.number;
}


CharPremadeMenu::CharPremadeMenu(Module &module, ::Engines::Console *console) : GUI(console),
	_module(&module) {

	load("pre_playmod");

	// TODO: "SaveLabel" should wrap!

	// TODO: Customize character
	Widget *customChar = getWidget("CustomCharButton");
	if (customChar)
		customChar->setDisabled(true);

	// TODO: Show pregenerated characters
	getWidget("SaveGameBox"     , true)->setDisabled(true);

	// TODO: Delete character
	getWidget("DeleteCharButton", true)->setDisabled(true);

	_charGen = std::make_unique<CharGenMenu>(*_module, _console);
}

CharPremadeMenu::~CharPremadeMenu() {
}

void CharPremadeMenu::show() {
	if (_characters.empty())
		initCharacterList();

	getWidget("PlayButton", true)->setDisabled(_characters.empty());

	GUI::show();
}

void CharPremadeMenu::fixWidgetType(const Common::UString &tag, WidgetType &type) {
	if (tag == "ButtonList")
		type = kWidgetTypeListBox;
}

void CharPremadeMenu::callbackActive(Widget &widget) {
	if (widget.getTag() == "CancelButton") {
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "NewCharButton") {
		sub(*_charGen);
		return;
	}

	if (widget.getTag() == "PlayButton") {
		playCharacter();
		return;
	}

	if (widget.getTag() == "ButtonList") {
		// selectedCharacter();

		if (dynamic_cast<WidgetListBox &>(widget).wasDblClicked())
			playCharacter();

		return;
	}
}

void CharPremadeMenu::initCharacterList() {
	status("Creating character list");

	std::vector<Common::UString> characters;
	Game::getCharacters(characters, true);

	WidgetListBox &charList = *getListBox("ButtonList", true);

	charList.lock();

	charList.clear();
	charList.setMode(WidgetListBox::kModeSelectable);

	// Get the character display info
	_characters.reserve(characters.size());
	for (std::vector<Common::UString>::iterator c = characters.begin();
	     c != characters.end(); ++c) {

		_characters.push_back(Character());

		Character &ch = _characters.back();

		ch.file = *c;

		try {
			Creature::getPCListInfo(ch.file, true, ch.name, ch.classes, ch.portrait);
		} catch (...) {
			_characters.pop_back();

			Common::exceptionDispatcherWarning("Can't read PC \"%s\"", c->c_str());
			continue;
		}

		if (std::sscanf(ch.file.c_str(), "%*[^0-9]%u", &ch.number) != 1)
			ch.number = 0;

		ch.displayName = ch.name;
		if (ch.number > 0)
			ch.displayName += Common::String::format(" (%u)", ch.number);
	}

	std::sort(_characters.begin(), _characters.end());

	// Create the listbox items
	charList.reserve(characters.size());
	for (std::vector<Character>::iterator c = _characters.begin();
	     c != _characters.end(); ++c)
		charList.add(new WidgetListItemCharacter(*this, "fnt_galahad14",
		                                         c->displayName, c->classes, c->portrait, 2.0f));

	charList.unlock();

	charList.select(0);
}

static const Common::UString kStringEmpty;
const Common::UString &CharPremadeMenu::getSelectedCharacter() {
	size_t n = getListBox("ButtonList", true)->getSelected();
	if (n >= _characters.size())
		return kStringEmpty;

	return _characters[n].file;
}

void CharPremadeMenu::playCharacter() {
	try {
		_module->usePC(getSelectedCharacter(), true);
	} catch (...) {
		Common::exceptionDispatcherWarning();
		return;
	}

	_returnCode = 2;
}

} // End of namespace NWN

} // End of namespace Engines
