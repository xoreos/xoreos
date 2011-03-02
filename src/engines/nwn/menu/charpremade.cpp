/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/charpremade.cpp
 *  The premade character chooser.
 */

#include "common/util.h"
#include "common/configman.h"
#include "common/filepath.h"
#include "common/filelist.h"

#include "aurora/types.h"
#include "aurora/locstring.h"
#include "aurora/gfffile.h"

#include "graphics/aurora/text.h"
#include "graphics/aurora/model.h"

#include "engines/aurora/util.h"
#include "engines/aurora/model.h"

#include "engines/nwn/menu/charpremade.h"
#include "engines/nwn/menu/charnew.h"

namespace Engines {

namespace NWN {

Character::Character() {
}

Character::Character(const Common::UString &f) {
	file = f;

	n = 0;

	try {
		Aurora::GFFFile bic;

		loadGFF(bic, file, Aurora::kFileTypeBIC, MKID_BE('BIC '));

		const Aurora::GFFStruct &bicTop = bic.getTopLevel();

		Aurora::LocString firstName;
		bicTop.getLocString("FirstName", firstName);

		Aurora::LocString lastName;
		bicTop.getLocString("LastName", lastName);

		fullName = firstName.getFirstString();
		if (!fullName.empty())
			fullName += ' ';

		fullName += lastName.getFirstString();

		if (bicTop.hasField("ClassList")) {
			const Aurora::GFFList &cClasses = bicTop.getList("ClassList");

			for (Aurora::GFFList::const_iterator c = cClasses.begin(); c != cClasses.end(); ++c) {
				const Aurora::GFFStruct &cClass = **c;

				if (!classes.empty())
					classes += '/';

				classes += Common::UString::sprintf("%d", cClass.getUint("Class", -1));
			}
		}

		classes = "(" + classes + ")";

	} catch (...) {
		file.clear();
		fullName.clear();
		classes.clear();
	}
}

bool Character::empty() const {
	return file.empty();
}


WidgetListItemCharacter::WidgetListItemCharacter(::Engines::GUI &gui,
    const Common::UString &font, const Character &c, float spacing) :
	WidgetListItem(gui), _spacing(spacing) {

	_button = loadModelGUI("ctl_pre_btn_char");
	assert(_button);

	if (c.n > 0) {
		Common::UString name = Common::UString::sprintf("%s (%d)", c.fullName.c_str(), c.n);

		_textName = new Graphics::Aurora::Text(FontMan.get(font), name);
	} else
		_textName = new Graphics::Aurora::Text(FontMan.get(font), c.fullName);

	_textClass = new Graphics::Aurora::Text(FontMan.get(font), c.classes);
}

WidgetListItemCharacter::~WidgetListItemCharacter() {
	delete _button;
	delete _textName;
	delete _textClass;
}

void WidgetListItemCharacter::show() {
	_button->show();
	_textName->show();
	_textClass->show();
}

void WidgetListItemCharacter::hide() {
	_textClass->hide();
	_textName->hide();
	_button->hide();
}

void WidgetListItemCharacter::setPosition(float x, float y, float z) {
	Widget::setPosition(x, y, z);

	getPosition(x, y, z);
	_button->setPosition(x, y, z);

	x += 32.0;

	_textName->setPosition (x, y + _button->getHeight() -     _textName->getHeight() - 4.0, -z);
	_textClass->setPosition(x, y + _button->getHeight() - 2 * _textName->getHeight() - 4.0, -z);
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


CharPremadeMenu::CharPremadeMenu() {
	load("pre_playmod");

	// TODO: "Title" misplaced!
	// TODO: Title bar misplaced!
	// TODO: "SaveLabel" should wrap!

	// TODO: Customize character
	getWidget("CustomCharButton", true)->setDisabled(true);

	// TODO: Show pregenerated characters
	getWidget("SaveGameBox"     , true)->setDisabled(true);

	// TODO: Delete character
	getWidget("DeleteCharButton", true)->setDisabled(true);

	_charNew = new CharNewMenu;
}

CharPremadeMenu::~CharPremadeMenu() {
	delete _charNew;
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
		sub(*_charNew);
		return;
	}

	if (widget.getTag() == "PlayButton") {
		playCharacter();
		_returnCode = 2;
		return;
	}

	if (widget.getTag() == "ButtonList") {
		// selectedCharacter();

		if (dynamic_cast<WidgetListBox &>(widget).wasDblClicked()) {
			playCharacter();
			_returnCode = 2;
		}

		return;
	}
}

void CharPremadeMenu::initCharacterList() {
	status("Collecting characters");

	Common::UString charDir = ConfigMan.getString("NWN_localCharDir");
	if (charDir.empty())
		return;

	// Get characters

	Common::FileList charDirList;

	charDirList.addDirectory(charDir);

	std::list<Common::UString> characters;
	uint n = charDirList.getFileNames(characters);

	characters.sort(Common::UString::iless());

	_characters.reserve(n);
	for (std::list<Common::UString>::const_iterator c = characters.begin(); c != characters.end(); ++c) {

		if (Common::FilePath::getExtension(*c).equalsIgnoreCase(".bic")) {
			Character character(Common::FilePath::getStem(*c));
			if (character.empty())
				continue;

			if (!_characters.empty() && (_characters.back().fullName == character.fullName))
				character.n = _characters.back().n + 1;

			_characters.push_back(character);
		}

	}


	// Add the characters to the list box
	status("Creating character list");

	WidgetListBox &charList = *getListBox("ButtonList", true);

	charList.lock();

	charList.clear();
	charList.setMode(WidgetListBox::kModeSelectable);

	charList.reserve(_characters.size());
	for (std::vector<Character>::const_iterator c = _characters.begin(); c != _characters.end(); ++c)
		charList.add(new WidgetListItemCharacter(*this, "fnt_galahad14", *c, 2.0));

	charList.unlock();

	charList.select(0);
}

Common::UString CharPremadeMenu::getSelectedCharacter() {
	uint n = getListBox("ButtonList", true)->getSelected();
	if (n >= _characters.size())
		return "";

	return _characters[n].file;
}

void CharPremadeMenu::playCharacter() {
	Common::UString character = getSelectedCharacter();
	if (character.empty())
		return;

	ConfigMan.setString(Common::kConfigRealmGameTemp, "NWN_characterToUse", character);
}

} // End of namespace NWN

} // End of namespace Engines
