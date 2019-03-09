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
 *  The ingame equipment menu.
 */

#ifndef ENGINES_KOTOR_GUI_INGAME_MENU_EQU_H
#define ENGINES_KOTOR_GUI_INGAME_MENU_EQU_H

#include "src/engines/kotorbase/gui/menubase.h"

namespace Engines {

namespace Odyssey {
	class WidgetButton;
	class WidgetLabel;
}

namespace KotORBase {
	class Creature;
	class Module;
}

namespace KotOR {

class MenuEquipment : public KotORBase::MenuBase {
public:
	MenuEquipment(KotORBase::Module &module, ::Engines::Console *console = 0);

	void update();

	void show();
	void hide();

protected:
	void callbackRun();
	void callbackActive(Widget &widget);
	void callbackKeyInput(const Events::Key &key, const Events::EventType &type);

private:
	KotORBase::InventorySlot _selectedSlot;
	bool _slotFixated;
	std::vector<Common::UString> _visibleItems;

	void fillEquipedItems();
	Common::UString getEquipedItemIcon(KotORBase::InventorySlot slot) const;
	void fillEquipableItemsList();
	KotORBase::InventorySlot getSlotByWidgetTag(const Common::UString &tag) const;
	Odyssey::WidgetButton *getSlotButton(KotORBase::InventorySlot slot);
	Common::UString getSlotName(KotORBase::InventorySlot slot) const;
	void fixateOnSlot(bool fixate);
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_GUI_INGAME_MENU_EQU_H
