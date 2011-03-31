/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/main/charpremade.cpp
 *  The premade character chooser.
 */

#include "common/util.h"

#include "events/requests.h"

#include "graphics/aurora/text.h"
#include "graphics/aurora/model.h"

#include "engines/aurora/model.h"

#include "engines/nwn/charstore.h"
#include "engines/nwn/module.h"

#include "engines/nwn/gui/widgets/listbox.h"
#include "engines/nwn/gui/widgets/portrait.h"

#include "engines/nwn/gui/chargen/chargen.h"

#include "engines/nwn/gui/main/charpremade.h"

namespace Engines {

namespace NWN {

WidgetListItemCharacter::WidgetListItemCharacter(::Engines::GUI &gui,
    const Common::UString &font, const CharacterID &c, float spacing) :
	WidgetListItem(gui), _spacing(spacing) {

	_button = loadModelGUI("ctl_pre_btn_char");
	assert(_button);

	_button->setClickable(true);

	Common::UString portrait = c->getPortrait();

	_portrait = new Portrait(portrait, Portrait::kSizeTiny, 1.0);

	Common::UString name = c->getFullName();
	if (c.getNumber() > 0)
		name += Common::UString::sprintf(" (%d)", c.getNumber());

	Common::UString classes = "(" + c->getClassString() + ")";

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

	std::list<CharacterID> chars;
	uint n = CharStore.getCharacters(chars);

	WidgetListBox &charList = *getListBox("ButtonList", true);

	charList.lock();

	charList.clear();
	charList.setMode(WidgetListBox::kModeSelectable);

	_characters.reserve(n);
	charList.reserve(n);

	for (std::list<CharacterID>::const_iterator c = chars.begin(); c != chars.end(); ++c) {
		_characters.push_back(*c);

		charList.add(new WidgetListItemCharacter(*this, "fnt_galahad14", *c, 2.0));
	}

	charList.unlock();

	charList.select(0);

	RequestMan.sync();
}

CharacterID CharPremadeMenu::getSelectedCharacter() {
	uint n = getListBox("ButtonList", true)->getSelected();
	if (n >= _characters.size())
		return CharacterID();

	return _characters[n];
}

void CharPremadeMenu::playCharacter() {
	if (_module->usePC(getSelectedCharacter()))
		_returnCode = 2;
}

} // End of namespace NWN

} // End of namespace Engines
