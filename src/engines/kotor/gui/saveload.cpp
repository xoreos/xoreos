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
 *  Load/save game menu.
 */

#include "src/aurora/talkman.h"

#include "src/common/configman.h"
#include "src/common/filelist.h"
#include "src/common/filepath.h"
#include "src/common/strutil.h"

#include "src/graphics/graphics.h"

#include "src/engines/aurora/widget.h"

#include "src/engines/odyssey/button.h"
#include "src/engines/odyssey/label.h"
#include "src/engines/odyssey/listbox.h"
#include "src/engines/odyssey/scrollbar.h"

#include "src/engines/kotor/savedgame.h"

#include "src/engines/kotor/gui/saveload.h"

namespace Engines {

namespace KotOR {

SaveLoadMenu::SaveLoadMenu(KotORBase::Module &module,
                           ::Engines::Console *console,
                           uint8_t type,
                           bool frontBackground) :
		KotORBase::GUI(console),
		_module(&module),
		_type(type) {

	load("saveload");
	addBackground(KotORBase::kBackgroundTypeMenu, frontBackground);

	Odyssey::WidgetListBox *lbGames = getListBox("LB_GAMES");
	if (!lbGames)
		throw Common::Exception("SaveLoadMenu: No games listbox");

	lbGames->setItemSelectionEnabled(true);
	lbGames->setHideScrollbar(false);
	lbGames->setPadding(3);
	lbGames->setItemBorderColor(0.0f, 0.667969f, 0.988281f, 1.0f);
	lbGames->setSoundSelectItem("gui_actscroll");
	lbGames->createItemWidgets(6);

	if (_type == kSaveLoadMenuTypeLoad) {
		Odyssey::WidgetLabel *panelName = getLabel("LBL_PANELNAME");
		if (panelName)
			panelName->setText(TalkMan.getString(1585)); // Load Game

		Odyssey::WidgetLabel *planetName = getLabel("LBL_PLANETNAME");
		if (planetName)
			planetName->setText("");

		Odyssey::WidgetLabel *areaName = getLabel("LBL_AREANAME");
		if (areaName)
			areaName->setText("");

		Odyssey::WidgetButton *saveLoad = getButton("BTN_SAVELOAD");
		if (saveLoad)
			saveLoad->setText(TalkMan.getString(1589)); // Load

	} else
		lbGames->addItem(TalkMan.getString(1590));

	addSavedGameItems(lbGames);
	lbGames->refreshItemWidgets();
}

void SaveLoadMenu::callbackActive(Widget &widget) {
	const Common::UString &tag = widget.getTag();
	if (tag == "BTN_SAVELOAD") {
		int selectedIndex = getListBox("LB_GAMES")->getSelectedIndex();
		if (selectedIndex == -1)
			return;
		switch (_type) {
			case kSaveLoadMenuTypeLoad:
				tryLoadGame(_saveDirs[selectedIndex]);
				break;
			case kSaveLoadMenuTypeSave:
				trySaveGame(selectedIndex > 0 ? _saveDirs[selectedIndex - 1] : getNewSaveDirectory());
				break;
			default:
				break;
		}
	} else if (tag == "BTN_BACK")
		_returnCode = 1;
}

void SaveLoadMenu::callbackKeyInput(const Events::Key &key, const Events::EventType &type) {
	if (type == Events::kEventKeyDown) {
		switch (key) {
			case Events::kKeyUp:
				getListBox("LB_GAMES")->selectPreviousItem();
				break;
			case Events::kKeyDown:
				getListBox("LB_GAMES")->selectNextItem();
				break;
			default:
				break;
		}
	}
}

void SaveLoadMenu::addSavedGameItems(Odyssey::WidgetListBox *listBox) {
	Common::FileList dirs;
	Common::UString savesDir = Common::FilePath::normalize(ConfigMan.getString("path") + "/saves");
	dirs.addSubDirectories(savesDir);
	Common::UString slotTextFormat = TalkMan.getString(1594);

	dirs.sort(true);
	for (Common::FileList::const_iterator it = dirs.begin(); it != dirs.end(); ++it) {
		Common::UString saveDir = *it;
		Common::UString baseName;

		try {
			baseName = getBaseNameFromDirectory(saveDir);
		} catch (Common::Exception &e) {
			e.add("Failed to get save base name from directory \"%s\"", saveDir.c_str());

			printException(e, "WARNING: ");
			continue;
		}

		if (_type == kSaveLoadMenuTypeSave && !baseName.contains("Game"))
			continue;

		_saveDirs.push_back(saveDir);
		KotORBase::SavedGame *save = new SavedGame(saveDir);
		uint32_t timePlayed = save->getTimePlayed();
		Common::UString slotText(slotTextFormat);

		slotText.replaceAll("Game <CUSTOM0>", baseName);
		slotText.replaceAll("<CUSTOM1>", Common::composeString(timePlayed / 3600));
		slotText.replaceAll("<CUSTOM2>", Common::composeString(timePlayed % 3600 / 60));

		if (baseName.contains("Game"))
			slotText += "\r\n" + save->getName();

		delete save;
		listBox->addItem(slotText);
	}
}

void SaveLoadMenu::tryLoadGame(const Common::UString &dir) {
	try {
		hide();
		std::unique_ptr<KotORBase::SavedGame> save = std::make_unique<SavedGame>(dir, true);
		_module->loadSavedGame(save.get());
		GfxMan.lockFrame();
		_returnCode = 2;
	} catch (Common::Exception &e) {
		warning("Failed to load saved game: %s %s", dir.c_str(), e.what());
	}
}

void SaveLoadMenu::trySaveGame(const Common::UString &UNUSED(dir)) {

}

Common::UString SaveLoadMenu::getNewSaveDirectory() const {
	return "";
}

Common::UString SaveLoadMenu::getBaseNameFromDirectory(const Common::UString &dir) const {
	Common::UString result;
	Common::UString relativeDir(Common::FilePath::getFile(dir));

	if (relativeDir.contains("QUICKSAVE"))
		result = "Quick Save";
	else if (relativeDir.contains("AUTOSAVE"))
		result = "Auto Save";
	else if (relativeDir.contains("Game")) {
		/* The format of a normal save directory is something like "000043 - Game42".
		 * The original game seems to ignore the GameXX part, and instead parses
		 * the first number, and then substracts 1 from the result.
		 *
		 * I.e. "000062 - Game42" will appears as "Game 61".
		 *
		 * Directories that fail this parsing, for example "abc - Game 42", simply
		 * won't appear in the save list.
		 */

		Common::UString::iterator it = relativeDir.begin();
		while ((it != relativeDir.end()) && (*it == '0'))
			++it;

		Common::UString tmp(it, relativeDir.end());
		tmp.truncate(tmp.findFirst(' '));

		int gameIndex = -1;

		Common::parseString(tmp, gameIndex);
		gameIndex--;

		if (gameIndex < 0)
			throw Common::Exception("Game index is negative (%d)", gameIndex);

		result = "Game " + Common::composeString(gameIndex);

	} else
		throw Common::Exception("Unknown save type");

	return result;
}

} // End of namespace KotOR

} // End of namespace Engines
