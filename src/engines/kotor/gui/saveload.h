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

#ifndef ENGINES_KOTOR_GUI_SAVELOAD_H
#define ENGINES_KOTOR_GUI_SAVELOAD_H

#include <vector>
#include <memory>

#include "src/engines/aurora/console.h"

#include "src/engines/kotorbase/module.h"

#include "src/engines/kotorbase/gui/gui.h"

namespace Engines {

namespace Odyssey {
	class WidgetListBox;
}

namespace KotOR {

enum {
	kSaveLoadMenuTypeSave = 0,
	kSaveLoadMenuTypeLoad = 1
};

class SaveLoadMenu : public KotORBase::GUI {
public:
	SaveLoadMenu(KotORBase::Module &module,
	             ::Engines::Console *console,
	             uint8_t type = kSaveLoadMenuTypeLoad,
	             bool frontBackground = false);
private:
	KotORBase::Module *_module;
	uint8_t _type;
	std::vector<Common::UString> _saveDirs;

	void callbackActive(Widget &widget);
	void callbackKeyInput(const Events::Key &key, const Events::EventType &type);
	void addSavedGameItems(Odyssey::WidgetListBox *listBox);
	void tryLoadGame(const Common::UString &dir);
	void trySaveGame(const Common::UString &dir);
	Common::UString getNewSaveDirectory() const;
	Common::UString getBaseNameFromDirectory(const Common::UString &dir) const;
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_GUI_SAVELOAD_H
