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

#include "src/engines/aurora/widget.h"

#include "src/engines/kotor/gui/saveload.h"
#include "src/engines/kotor/gui/widgets/button.h"
#include "src/engines/kotor/gui/widgets/label.h"
#include "src/engines/kotor/gui/widgets/listbox.h"

namespace Engines {

namespace KotOR {

static const int kMaxVisibleSlots = 6;

SaveLoadMenu::SaveLoadMenu(::Engines::Console *console, uint8 type,
		bool frontBackground) : GUI(console), _type(type) {
	load("saveload");
	addBackground(kBackgroundTypeMenu, frontBackground);
	createSlotWidgets();

	if (_type == kSaveLoadMenuTypeLoad) {
		getLabel("LBL_PANELNAME")->setText(TalkMan.getString(1585)); // Load Game
		getButton("BTN_SAVELOAD")->setText(TalkMan.getString(1589)); // Load
	} else
		addNewSlotItem();

	addSavedGameItems();
}

void SaveLoadMenu::callbackActive(Widget &widget) {
	if (widget.getTag() == "BTN_BACK")
		_returnCode = 1;
}

void SaveLoadMenu::createSlotWidgets() {
	WidgetListBox *lbGames = static_cast<WidgetListBox *>(getWidget("LB_GAMES"));
	for (int i = 0; i < kMaxVisibleSlots; ++i) {
		Common::UString tag = Common::UString::format("BTN_SLOT%d", i);
		KotORWidget *slot = lbGames->createItem(tag);
		_slots.push_back(slot);
		addWidget(slot);
	}
}

void SaveLoadMenu::addNewSlotItem() {
	_slots[0]->setText(TalkMan.getString(1590)); // New Slot
}

void SaveLoadMenu::addSavedGameItems() {
	Common::FileList dirs;
	Common::UString savesDir = Common::FilePath::normalize(ConfigMan.getString("path") + "/saves");
	dirs.addSubDirectories(savesDir);
	Common::UString slotTextFormat = TalkMan.getString(1594);
	int gameIndex = 1;
	int slotIndex = _type == kSaveLoadMenuTypeSave ? 1 : 0;

	for (Common::FileList::const_iterator it = dirs.begin(); it != dirs.end(); ++it) {
		_savedGames.push_back(*it);

		if (slotIndex < kMaxVisibleSlots) {
			Common::UString slotText(slotTextFormat);
			slotText.replaceAll("<CUSTOM0>", Common::composeString(gameIndex++));
			slotText.replaceAll("<CUSTOM1>", "0");
			slotText.replaceAll("<CUSTOM2>", "0");
			_slots[slotIndex++]->setText(slotText);
		}
	}
}

} // End of namespace KotOR

} // End of namespace Engines
