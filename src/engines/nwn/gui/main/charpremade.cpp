/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file engines/nwn/gui/main/charpremade.cpp
 *  The premade character chooser.
 */

#include "common/util.h"
#include "common/error.h"

#include "events/requests.h"

#include "graphics/aurora/text.h"
#include "graphics/aurora/model.h"

#include "engines/aurora/model.h"

#include "engines/nwn/nwn.h"
#include "engines/nwn/creature.h"
#include "engines/nwn/module.h"

#include "engines/nwn/gui/widgets/listbox.h"
#include "engines/nwn/gui/widgets/portrait.h"

#include "engines/nwn/gui/chargen/chargen.h"

#include "engines/nwn/gui/main/charpremade.h"

namespace Engines {

namespace NWN {

WidgetListItemCharacter::WidgetListItemCharacter(::Engines::GUI &gui,
		const Common::UString &font   , const Common::UString &name,
		const Common::UString &classes, const Common::UString &portrait, float spacing) :
	WidgetListItem(gui), _spacing(spacing) {

	_button = loadModelGUI("ctl_pre_btn_char");
	assert(_button);

	_button->setClickable(true);

	_portrait  = new Portrait(portrait, Portrait::kSizeTiny, 1.0);
	_textName  = new Graphics::Aurora::Text(FontMan.get(font), name);
	_textClass = new Graphics::Aurora::Text(FontMan.get(font), classes);
}

WidgetListItemCharacter::~WidgetListItemCharacter() {
	delete _button;
	delete _portrait;
	delete _textName;
	delete _textClass;
}

void WidgetListItemCharacter::show() {
	_button->show();
	_textName->show();
	_textClass->show();
	_portrait->show();
}

void WidgetListItemCharacter::hide() {
	_textClass->hide();
	_textName->hide();
	_button->hide();
	_portrait->hide();
}

void WidgetListItemCharacter::setPosition(float x, float y, float z) {
	NWNWidget::setPosition(x, y, z);

	getPosition(x, y, z);
	_button->setPosition(x, y, z);

	z -= 5.0;

	_portrait->setPosition(x + 8.0, y + 7.0, z);

	x += 32.0;

	_textName->setPosition (x, y + _button->getHeight() -     _textName->getHeight() - 4.0, z);
	_textClass->setPosition(x, y + _button->getHeight() - 2 * _textName->getHeight() - 4.0, z);
}

float WidgetListItemCharacter::getWidth() const {
	return _button->getWidth();
}

float WidgetListItemCharacter::getHeight() const {
	return _button->getHeight() + _spacing;
}

void WidgetListItemCharacter::setTag(const Common::UString &tag) {
	WidgetListItem::setTag(tag);

	_button->setTag(tag);
}

bool WidgetListItemCharacter::activate() {
	if (!WidgetListItem::activate())
		return false;

	_button->setState("down");

	return true;
}

bool WidgetListItemCharacter::deactivate() {
	if (!WidgetListItem::deactivate())
		return false;

	_button->setState("");

	return true;
}


bool CharPremadeMenu::Character::operator<(const Character &c) const {
	int cmp = name.strcmp(c.name);

	if (cmp < 0)
		return true;
	if (cmp > 0)
		return false;

	return number < c.number;
}


CharPremadeMenu::CharPremadeMenu(Module &module) : _module(&module) {
	load("pre_playmod");

	// TODO: "Title" misplaced!
	// TODO: Title bar misplaced!
	// TODO: "SaveLabel" should wrap!

	// TODO: Customize character
	Widget *customChar = getWidget("CustomCharButton");
	if (customChar)
		customChar->setDisabled(true);

	// TODO: Show pregenerated characters
	getWidget("SaveGameBox"     , true)->setDisabled(true);

	// TODO: Delete character
	getWidget("DeleteCharButton", true)->setDisabled(true);

	_charGen = new CharGenMenu(*_module);
}

CharPremadeMenu::~CharPremadeMenu() {
	delete _charGen;
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
	NWNEngine::getCharacters(characters, true);

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
		} catch (Common::Exception &e) {
			_characters.pop_back();

			e.add("Can't read PC \"%s\"", c->c_str());
			Common::printException(e, "WARNING: ");
			continue;
		}

		if (sscanf(ch.file.c_str(), "%*[^0-9]%d", &ch.number) != 1)
			ch.number = 0;

		ch.displayName = ch.name;
		if (ch.number > 0)
			ch.displayName += Common::UString::sprintf(" (%d)", ch.number);
	}

	std::sort(_characters.begin(), _characters.end());

	// Create the listbox items
	charList.reserve(characters.size());
	for (std::vector<Character>::iterator c = _characters.begin();
	     c != _characters.end(); ++c)
		charList.add(new WidgetListItemCharacter(*this, "fnt_galahad14",
		                                         c->displayName, c->classes, c->portrait, 2.0));

	charList.unlock();

	charList.select(0);
}

static const Common::UString kStringEmpty;
const Common::UString &CharPremadeMenu::getSelectedCharacter() {
	uint n = getListBox("ButtonList", true)->getSelected();
	if (n >= _characters.size())
		return kStringEmpty;

	return _characters[n].file;
}

void CharPremadeMenu::playCharacter() {
	if (_module->usePC(getSelectedCharacter(), true))
		_returnCode = 2;
}

} // End of namespace NWN

} // End of namespace Engines
