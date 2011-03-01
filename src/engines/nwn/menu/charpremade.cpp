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

#include "engines/aurora/util.h"

#include "engines/nwn/menu/charpremade.h"
#include "engines/nwn/menu/charnew.h"

namespace Engines {

namespace NWN {

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

		if (dynamic_cast<WidgetEditBox &>(widget).wasDblClicked()) {
			playCharacter();
			_returnCode = 2;
		}

		return;
	}
}

void CharPremadeMenu::initCharacterList() {
	WidgetEditBox &charList = *getEditBox("ButtonList", true);

	charList.clear();
	charList.setMode(WidgetEditBox::kModeSelectable);

	Common::UString charDir = ConfigMan.getString("NWN_localCharDir");
	if (charDir.empty())
		return;

	Common::FileList charDirList;

	charDirList.addDirectory(charDir);

	std::list<Common::UString> characters;
	uint n = charDirList.getFileNames(characters);

	characters.sort(Common::UString::iless());

	_characters.reserve(n);
	charList.reserve(n);
	for (std::list<Common::UString>::const_iterator c = characters.begin(); c != characters.end(); ++c) {
		if (Common::FilePath::getExtension(*c).equalsIgnoreCase(".bic")) {

			Common::UString file = Common::FilePath::getStem(*c);
			Common::UString name = getCharacterName(file);

			if (name.empty())
				continue;

			_characters.push_back(file);
			charList.addLine(name);
		}
	}

	charList.selectLine(0);
}

Common::UString CharPremadeMenu::getCharacterName(const Common::UString &file) {
	Common::UString name;

	try {
		Aurora::GFFFile bic;

		loadGFF(bic, file, Aurora::kFileTypeBIC, MKID_BE('BIC '));

		const Aurora::GFFStruct &bicTop = bic.getTopLevel();

		Aurora::LocString firstName;
		bicTop.getLocString("FirstName", firstName);

		Aurora::LocString lastName;
		bicTop.getLocString("LastName", lastName);

		name = firstName.getFirstString() + " " + lastName.getFirstString();

	} catch (...) {
		name.clear();
	}

	return name;
}

Common::UString CharPremadeMenu::getSelectedCharacter() {
	uint n = getEditBox("ButtonList", true)->getSelectedLineNumber();
	if (n >= _characters.size())
		return "";

	return _characters[n];
}

void CharPremadeMenu::playCharacter() {
	Common::UString character = getSelectedCharacter();
	if (character.empty())
		return;

	ConfigMan.setString(Common::kConfigRealmGameTemp, "NWN_characterToUse", character);
}

} // End of namespace NWN

} // End of namespace Engines
