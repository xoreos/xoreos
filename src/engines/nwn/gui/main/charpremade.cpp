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

#include "graphics/guiman.h"

#include "graphics/aurora/text.h"
#include "graphics/aurora/model_nwn.h"

#include "events/requests.h"

#include "engines/nwn/nwn.h"
#include "engines/nwn/model.h"
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
	WidgetListItem(gui), _button(0), _textName(0), _textClass(0), _portrait(0), _spacing(spacing) {

	_button = createGUIModel("ctl_pre_btn_char");
	_button->setSelectable(true);
	GUIMan.addRenderable(_button);

	_portrait = new Portrait(portrait, Portrait::kSizeTiny, 1.0);
	GUIMan.addRenderable(_portrait);

	_textName = new Graphics::Aurora::Text(FontMan.get(font), name);
	GUIMan.addRenderable(_textName);

	_textClass = new Graphics::Aurora::Text(FontMan.get(font), classes);
	GUIMan.addRenderable(_textClass);

	_ids.push_back(_button->getID());

	updateSize();
}

WidgetListItemCharacter::~WidgetListItemCharacter() {
	GUIMan.removeRenderable(_button);
	GUIMan.removeRenderable(_portrait);
	GUIMan.removeRenderable(_textName);
	GUIMan.removeRenderable(_textClass);

	destroyModel(_button);
	delete _portrait;
	delete _textName;
	delete _textClass;
}

void WidgetListItemCharacter::updateSize() {
	_button->getSize(_width, _height, _depth);

	_width  = floor(_width);
	_height = floor(_height);
}

void WidgetListItemCharacter::setVisible(bool visible) {
	_button->setVisible(visible);
	_portrait->setVisible(visible);
	_textName->setVisible(visible);
	_textClass->setVisible(visible);
}

void WidgetListItemCharacter::setPosition(float x, float y, float z) {
	float buttonWidth, buttonHeight, buttonDepth;
	_button->getSize(buttonWidth, buttonHeight, buttonDepth);

	float nameWidth, nameHeight, nameDepth;
	_textName->getSize(nameWidth, nameHeight, nameDepth);

	NWNWidget::setPosition(x, y, z);
	getPosition(x, y, z);

	float buttonPos  [3] = { x        , y - buttonHeight                     + 1.0f, z       };
	float portraitPos[3] = { x +  7.0f, y                                    - 7.0f, z + 5.0f };
	float namePos    [3] = { x + 32.0f, y - buttonHeight +        nameHeight + 4.0f, z + 5.0f };
	float classPos   [3] = { x + 32.0f, y - buttonHeight + 2.0f * nameHeight + 4.0f, z + 5.0f };

	_button->setPosition   (floor(buttonPos  [0]), floor(buttonPos  [1]), buttonPos  [2]);
	_portrait->setPosition (floor(portraitPos[0]), floor(portraitPos[1]), portraitPos[2]);
	_textName->setPosition (floor(namePos    [0]), floor(namePos    [1]), namePos    [2]);
	_textClass->setPosition(floor(classPos   [0]), floor(classPos   [1]), classPos   [2]);
}

float WidgetListItemCharacter::getWidth() const {
	return _width;
}

float WidgetListItemCharacter::getHeight() const {
	return _height + _spacing;
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


CharPremadeMenu::CharPremadeMenu(Module &module) : _module(&module), _charGen(0) {
	setPosition(0.0, 0.0, -300.0);
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
}

CharPremadeMenu::~CharPremadeMenu() {
	delete _charGen;
}

void CharPremadeMenu::setVisible(bool visible) {
	if (visible) {
		if (_characters.empty())
			initCharacterList();

		getWidget("PlayButton", true)->setDisabled(_characters.empty());
	}

	GUI::setVisible(visible);
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
		if (!_charGen)
			_charGen = new CharGenMenu(*_module);

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
	_returnCode = 2;
	/*
	if (_module->usePC(getSelectedCharacter(), true))
		_returnCode = 2;
	*/
}

} // End of namespace NWN

} // End of namespace Engines
