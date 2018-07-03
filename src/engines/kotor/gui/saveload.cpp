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

#include "src/engines/kotor/gui/saveload.h"
#include "src/engines/aurora/kotorjadegui/button.h"
#include "src/engines/aurora/kotorjadegui/label.h"
#include "src/engines/aurora/kotorjadegui/listbox.h"
#include "src/engines/aurora/kotorjadegui/scrollbar.h"

namespace Engines {

namespace KotOR {

SaveLoadMenu::SaveLoadMenu(Module &module,
                           ::Engines::Console *console,
                           uint8 type,
                           bool frontBackground)
		: GUI(console),
		  _module(&module),
		  _type(type) {
	load("saveload");
	addBackground(kBackgroundTypeMenu, frontBackground);

	getLabel("LBL_PM1")->setFill("");
	getLabel("LBL_PM2")->setFill("");
	getLabel("LBL_PM3")->setFill("");
	getLabel("LBL_SCREENSHOT")->setFill("");

	WidgetListBox *lbGames = getListBox("LB_GAMES");
	if (!lbGames)
		throw Common::Exception("SaveLoadMenu: No games listbox");

	lbGames->setItemSelectionEnabled(true);
	lbGames->setHideScrollbar(false);
	lbGames->setPadding(3);
	lbGames->setItemBorderColor(0.0f, 0.667969f, 0.988281f, 1.0f);
	lbGames->createItemWidgets(6);

	if (_type == kSaveLoadMenuTypeLoad) {
		WidgetLabel *panelName = getLabel("LBL_PANELNAME");
		if (panelName)
			panelName->setText(TalkMan.getString(1585)); // Load Game

		WidgetLabel *planetName = getLabel("LBL_PLANETNAME");
		if (planetName)
			planetName->setText("");

		WidgetLabel *areaName = getLabel("LBL_AREANAME");
		if (areaName)
			areaName->setText("");

		WidgetButton *saveLoad = getButton("BTN_SAVELOAD");
		if (saveLoad)
			saveLoad->setText(TalkMan.getString(1589)); // Load

	} else
		lbGames->addItem(TalkMan.getString(1590));

	addSavedGameItems(lbGames);
	lbGames->refreshItemWidgets();

	lbGames->selectNextItem();
	refreshMetadata();
}

void SaveLoadMenu::refreshMetadata() {
	if (!_userData.getNumSaves())
		return;

	size_t currentIndex = getListBox("LB_GAMES")->getSelectedIndex();

	Aurora::NFOFile nfoFile(_userData.createMetadataReadStream(currentIndex));
	if (!nfoFile.getPortrait0().empty())
		getLabel("LBL_PM1")->setFill(nfoFile.getPortrait0());
	else
		getLabel("LBL_PM1")->setFill("");

	if (!nfoFile.getPortrait1().empty())
		getLabel("LBL_PM2")->setFill(nfoFile.getPortrait1());
	else
		getLabel("LBL_PM2")->setFill("");

	if (!nfoFile.getPortrait2().empty())
		getLabel("LBL_PM3")->setFill(nfoFile.getPortrait2());
	else
		getLabel("LBL_PM3")->setFill("");

	getLabel("LBL_SCREENSHOT")->setFill(_userData.getScreenshotTexture(currentIndex));

	std::vector<Common::UString> areaName;
	Common::UString::split(nfoFile.getAreaName(), '-', areaName);
	areaName[0].erase(areaName[0].findLast(' '));
	areaName[1].erase(areaName[1].findFirst(' '));
	getLabel("LBL_PLANETNAME")->setText(areaName[0]);
	getLabel("LBL_AREANAME")->setText(areaName[1]);
}

void SaveLoadMenu::callbackActive(Widget &widget) {
	const Common::UString &tag = widget.getTag();
	if (tag == "BTN_SAVELOAD") {
		int selectedIndex = getListBox("LB_GAMES")->getSelectedIndex();
		if (selectedIndex == -1)
			return;
		switch (_type) {
			case kSaveLoadMenuTypeLoad:
				tryLoadGame(_userData.getSaveDir(selectedIndex));
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
				refreshMetadata();
				break;
			case Events::kKeyDown:
				getListBox("LB_GAMES")->selectNextItem();
				refreshMetadata();
				break;
			default:
				break;
		}
	}
}

void SaveLoadMenu::addSavedGameItems(WidgetListBox *listBox) {
	Common::UString slotTextFormat = TalkMan.getString(1594);

	for (size_t i = 0; i < _userData.getNumSaves(); ++i) {
		Aurora::NFOFile nfoFile = Aurora::NFOFile(_userData.createMetadataReadStream(i));

		uint32 timePlayed = nfoFile.getTimePlayed();

		Common::UString slotText(slotTextFormat);
		slotText.replaceAll(TalkMan.getString(48280) + " <CUSTOM0>", _userData.getSaveId(i));
		slotText.replaceAll("<CUSTOM1>", Common::composeString(timePlayed / 3600));
		slotText.replaceAll("<CUSTOM2>", Common::composeString(timePlayed % 3600 / 60));

		listBox->addItem(slotText);
	}
}

void SaveLoadMenu::tryLoadGame(const Common::UString &dir) {
	try {
		Common::ScopedPtr<SavedGame> save(SavedGame::load(dir, true));
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

} // End of namespace KotOR

} // End of namespace Engines
